
#include <mutex>
#include <limits>
#include <font/fontconfig.h>
#include <QString>
#include <QStringList>
#include <trace_helpers.h>
#include <string_utils.h>
#include <macros.h>
#include <cstdint>
#include <reporter.h>
#include <embedded_files.h>

#if defined(__WIN32__) || defined(_MSC_VER)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winnt.h>
#include <excpt.h>
#endif

using namespace fontconfig;

static FONTCONFIG* g_config = nullptr;
static bool        g_fcInitSuccess = false;

REPORTER* FONTCONFIG::s_reporter = nullptr;
static std::mutex g_fontConfigMutex;

/**
 * A simple wrapper to avoid exporting fontconfig in the header
 */
struct fontconfig::FONTCONFIG_PAT
{
    FcPattern* pat;
};


QString FONTCONFIG::Version()
{
    return QString("%1.%2.%3").arg(FC_MAJOR).arg(FC_MINOR).arg(FC_REVISION);
}


FONTCONFIG::FONTCONFIG()
{
};


void fontconfig::FONTCONFIG::SetReporter( REPORTER* aReporter )
{
    std::lock_guard lock( g_fontConfigMutex );
    s_reporter = aReporter;
}


static void bootstrapFc()
{
#if defined( _MSC_VER )
    __try
    {
#endif
        FcInit();
        g_fcInitSuccess = true;
#if defined( _MSC_VER )
    }
    __except( GetExceptionCode() == STATUS_IN_PAGE_ERROR ? EXCEPTION_EXECUTE_HANDLER
                                                         : EXCEPTION_CONTINUE_SEARCH )
    {
        g_fcInitSuccess = false;

    }
#endif
}


FONTCONFIG* Fontconfig()
{
    if( !g_config )
    {
        bootstrapFc();
        g_config = new FONTCONFIG();
    }

    return g_config;
}


bool FONTCONFIG::isLanguageMatch( const QString& aSearchLang, const QString& aSupportedLang )
{
    if( aSearchLang.toLower() == aSupportedLang.toLower() )
        return true;

    if( aSupportedLang.isEmpty() )
        return false;

    if( aSearchLang.isEmpty() )
        return false;

    QStringList supportedLangBits = aSupportedLang.toLower().split('-');
    QStringList searhcLangBits = aSearchLang.toLower().split('-');

    if( searhcLangBits.size() == 1 || supportedLangBits.size() == 1 )
    {
        return searhcLangBits[0] == supportedLangBits[0];
    }

    return false;
}


std::string FONTCONFIG::getFcString( FONTCONFIG_PAT& aPat, const char* aObj, int aIdx )
{
    FcChar8*    str;
    std::string res;

    if( FcPatternGetString( aPat.pat, aObj, aIdx, &str ) == FcResultMatch )
        res = std::string( reinterpret_cast<char*>( str ) );

    return res;
}


void FONTCONFIG::getAllFamilyStrings( FONTCONFIG_PAT&                               aPat,
                                      std::unordered_map<std::string, std::string>& aFamStringMap )
{
    std::string famLang;
    std::string fam;

    int langIdx = 0;

    do
    {
        famLang = getFcString( aPat, FC_FAMILYLANG, langIdx );

        if( famLang.empty() && langIdx != 0 )
        {
            break;
        }
        else
        {
            fam = getFcString( aPat, FC_FAMILY, langIdx );
            aFamStringMap.insert_or_assign( famLang, fam );
        }
    } while( langIdx++ < 127 ); //arbitrary to avoid getting stuck for any reason (int8_t max)
}


std::string FONTCONFIG::getFamilyStringByLang( FONTCONFIG_PAT& aPat, const QString& aDesiredLang )
{
    std::unordered_map<std::string, std::string> famStrings;
    getAllFamilyStrings( aPat, famStrings );

    if( famStrings.empty() )
        return "";

    for( auto const& [key, val] : famStrings )
    {
        if( isLanguageMatch( aDesiredLang, From_UTF8( key.c_str() ) ) )
        {
            return val;
        }
    }

    return famStrings.begin()->second;
}


FONTCONFIG::FF_RESULT FONTCONFIG::FindFont( const QString& aFontName, QString& aFontFile,
                                            int& aFaceIndex, bool aBold, bool aItalic,
                                            const std::vector<QString>* aEmbeddedFiles )
{
    FF_RESULT retval = FF_RESULT::FF_ERROR;

    if( !g_fcInitSuccess )
        return retval;

    if( aFontName.toLower().contains( "bold" )
        || aFontName.toLower().contains( "heavy" )
        || aFontName.toLower().contains( "black" )
        || aFontName.toLower().contains( "thick" )
        || aFontName.toLower().contains( "dark" ) )
    {
        aBold = true;
    }

    FcConfig* config = FcConfigGetCurrent();

    if( aEmbeddedFiles )
    {
        for( const auto& file : *aEmbeddedFiles )
        {
            FcConfigAppFontAddFile( config, (const FcChar8*) file.toUtf8().constData() );
        }
    }

    QString qualifiedFontName = aFontName;

    QByteArray fcBuffer = qualifiedFontName.toUtf8();

    FcPattern* pat = FcPatternCreate();

    if( aBold )
        FcPatternAddString( pat, FC_STYLE, (const FcChar8*) "Bold" );

    if( aItalic )
        FcPatternAddString( pat, FC_STYLE, (const FcChar8*) "Italic" );

    FcPatternAddString( pat, FC_FAMILY, (FcChar8*) fcBuffer.constData() );

    FcConfigSubstitute( config, pat, FcMatchPattern );
    FcDefaultSubstitute( pat );

    FcResult   r = FcResultNoMatch;
    FcPattern* font = FcFontMatch( config, pat, &r );

    QString fontName;

    if( font )
    {
        FcChar8* file = nullptr;

        if( FcPatternGetString( font, FC_FILE, 0, &file ) == FcResultMatch )
        {
            aFontFile = QString::fromUtf8( (char*) file );
            aFaceIndex = 0;

            QString styleStr;
            FcChar8* family = nullptr;
            FcChar8* style = nullptr;

            retval = FF_RESULT::FF_SUBSTITUTE;

            std::unordered_map<std::string, std::string> famStrings;
            FONTCONFIG_PAT                               patHolder{ font };

            getAllFamilyStrings( patHolder, famStrings );

            if( FcPatternGetString( font, FC_FAMILY, 0, &family ) == FcResultMatch )
            {
                FcPatternGetInteger( font, FC_INDEX, 0, &aFaceIndex );

                fontName = QString::fromUtf8( (char*) family );

                if( FcPatternGetString( font, FC_STYLE, 0, &style ) == FcResultMatch )
                {
                    styleStr = QString::fromUtf8( (char*) style );

                    if( !styleStr.isEmpty() )
                    {
                        styleStr.replace( ' ', ':' );
                        fontName += ':' + styleStr;
                    }
                }

                bool has_bold = false;
                bool has_ital = false;
                QString lower_style = styleStr.toLower();

                if( lower_style.contains( "thin" )
                         || lower_style.contains( "light" )
                         || lower_style.contains( "regular" )
                         || lower_style.contains( "roman" )
                         || lower_style.contains( "book" ) )
                {
                    has_bold = false;
                }
                else if( lower_style.contains( "medium" )
                         || lower_style.contains( "semibold" )
                         || lower_style.contains( "demibold" ) )
                {
                    has_bold = aBold;
                }
                else if( lower_style.contains( "bold" )
                         || lower_style.contains( "heavy" )
                         || lower_style.contains( "black" )
                         || lower_style.contains( "thick" )
                         || lower_style.contains( "dark" ) )
                {
                    has_bold = true;
                }

                if( lower_style.contains( "italic" )
                        || lower_style.contains( "oblique" )
                        || lower_style.contains( "slant" ) )
                {
                    has_ital = true;
                }

                for( auto const& [key, val] : famStrings )
                {
                    QString searchFont;
                    searchFont = QString::fromUtf8( (char*) val.data() );

                    if( searchFont.toLower().startsWith( aFontName.toLower() ) )
                    {
                        if( ( aBold && !has_bold ) && ( aItalic && !has_ital ) )
                            retval = FF_RESULT::FF_MISSING_BOLD_ITAL;
                        else if( aBold && !has_bold )
                            retval = FF_RESULT::FF_MISSING_BOLD;
                        else if( aItalic && !has_ital )
                            retval = FF_RESULT::FF_MISSING_ITAL;
                        else if( ( aBold != has_bold ) || ( aItalic != has_ital ) )
                            retval = FF_RESULT::FF_SUBSTITUTE;
                        else
                            retval = FF_RESULT::FF_OK;

                        break;
                    }
                }
            }
        }

        FcPatternDestroy( font );
    }

    if( retval == FF_RESULT::FF_ERROR )
    {
        if( s_reporter )
            s_reporter->Report( QString( "Error loading font '%1'." ).arg( qualifiedFontName ) );
    }
    else if( retval == FF_RESULT::FF_SUBSTITUTE )
    {
        fontName.replace( ':', ' ' );

        if( fontName.compare( qualifiedFontName, Qt::CaseInsensitive ) == 0 )
            retval = FF_RESULT::FF_OK;
        else if( s_reporter )
            s_reporter->Report( QString( "Font '%1' not found; substituting '%2'." )
                                .arg( qualifiedFontName ).arg( fontName ) );
    }

    FcPatternDestroy( pat );
    return retval;
}


void FONTCONFIG::ListFonts( std::vector<std::string>& aFonts, const std::string& aDesiredLang,
                            const std::vector<QString>* aEmbeddedFiles, bool aForce )
{
    if( !g_fcInitSuccess )
        return;

    if( m_fontInfoCache.empty() || m_fontCacheLastLang != aDesiredLang || aForce )
    {
        FcConfig* config = FcConfigGetCurrent();

        if( aEmbeddedFiles )
        {
            for( const auto& file : *aEmbeddedFiles )
            {
                FcConfigAppFontAddFile( config, (const FcChar8*) file.toUtf8().constData() );
            }
        }

        FcPattern*   pat = FcPatternCreate();
        FcObjectSet* os = FcObjectSetBuild( FC_FAMILY, FC_FAMILYLANG, FC_STYLE, FC_LANG, FC_FILE,
                                            FC_OUTLINE, nullptr );
        FcFontSet*   fs = FcFontList( config, pat, os );

        for( int i = 0; fs && i < fs->nfont; ++i )
        {
            FcPattern* font = fs->fonts[i];
            FcChar8*   file;
            FcChar8*   style;
            FcLangSet* langSet;
            FcBool     outline;

            if( FcPatternGetString( font, FC_FILE, 0, &file ) == FcResultMatch
                && FcPatternGetString( font, FC_STYLE, 0, &style ) == FcResultMatch
                && FcPatternGetLangSet( font, FC_LANG, 0, &langSet ) == FcResultMatch
                && FcPatternGetBool( font, FC_OUTLINE, 0, &outline ) == FcResultMatch )
            {
                if( !outline )
                    continue;

                FONTCONFIG_PAT patHolder{ font };
                std::string    theFamily =
                        getFamilyStringByLang( patHolder, From_UTF8( aDesiredLang.c_str() ) );

#ifdef __WXMAC__
                static std::map<QString, bool> availableLanguages;

                FcStrSet*  langStrSet = FcLangSetGetLangs( langSet );
                FcStrList* langStrList = FcStrListCreate( langStrSet );
                FcChar8*   langStr = FcStrListNext( langStrList );
                bool       langSupported = false;

                if( !langStr )
                {
                    langSupported = true;
                }
                else while( langStr )
                {
                    QString langQStr( reinterpret_cast<char *>( langStr ) );

                    if( availableLanguages.find( langQStr ) == availableLanguages.end() )
                    {
                        availableLanguages[ langQStr ] = true;
                    }

                    if( availableLanguages[ langQStr ] )
                    {
                        langSupported = true;
                        break;
                    }

                    langStr = FcStrListNext( langStrList );
                }

                FcStrListDone( langStrList );
                FcStrSetDestroy( langStrSet );

                if( !langSupported )
                    continue;
#endif

                std::string theFile( reinterpret_cast<char *>( file ) );
                std::string theStyle( reinterpret_cast<char *>( style ) );
                FONTINFO    fontInfo( std::move( theFile ), std::move( theStyle ), theFamily );

                if( theFamily.length() > 0 && theFamily.front() == '.' )
                    continue;

                std::map<std::string, FONTINFO>::iterator it = m_fontInfoCache.find( theFamily );

                if( it == m_fontInfoCache.end() )
                    m_fontInfoCache.emplace( theFamily, fontInfo );
                else
                    it->second.Children().push_back( fontInfo );
            }
        }

        if( fs )
            FcFontSetDestroy( fs );

        m_fontCacheLastLang = QString::fromStdString(aDesiredLang);
    }

    for( const std::pair<const std::string, FONTINFO>& entry : m_fontInfoCache )
        aFonts.push_back( entry.second.Family() );
}

