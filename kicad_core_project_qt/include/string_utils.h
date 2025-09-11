// QT_TRANSFORMATION_COMPLETED

#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <string>
#include <vector>
#include <QString>
#include <vector>
#include <algorithm>

#include <kicommon.h>

void ConvertMarkdown2Html( const QString& aMarkdownInput, QString& aHtmlOutput );

KICOMMON_API QString ConvertToNewOverbarNotation( const QString& aOldStr );

KICOMMON_API bool ConvertSmartQuotesAndDashes( QString* aString );

enum ESCAPE_CONTEXT
{
    CTX_NETNAME,
    CTX_LIBID,
    CTX_LEGACY_LIBID,
    CTX_IPC,
    CTX_QUOTED_STR,
    CTX_JS_STR,
    CTX_LINE,
    CTX_CSV,
    CTX_FILENAME,
    CTX_NO_SPACE
};

KICOMMON_API QString EscapeString( const QString& aSource, ESCAPE_CONTEXT aContext );

KICOMMON_API QString UnescapeString( const QString& aSource );

KICOMMON_API QString PrettyPrintForMenu( const QString& aString );

KICOMMON_API QString TitleCaps( const QString& aString );

KICOMMON_API int ReadDelimitedText( char* aDest, const char* aSource, int aDestSize );

KICOMMON_API int ReadDelimitedText( QString* aDest, const char* aSource );

KICOMMON_API std::string EscapedUTF8( const QString& aString );

KICOMMON_API QString EscapeHTML( const QString& aString );

KICOMMON_API QString UnescapeHTML( const QString& aString );

KICOMMON_API QString RemoveHTMLTags( const QString& aInput );

KICOMMON_API QString LinkifyHTML( QString aStr );

KICOMMON_API bool IsURL( QString aStr );

KICOMMON_API char* GetLine( FILE* aFile, char* Line, int* LineNum = nullptr, int SizeLine = 255 );

KICOMMON_API bool NoPrintableChars( const QString& aString );

KICOMMON_API int PrintableCharCount( const QString& aString );

KICOMMON_API char* StrPurge( char* text );

KICOMMON_API QString GetISO8601CurrentDateTime();

KICOMMON_API int StrNumCmp( const QString& aString1, const QString& aString2,
                           bool aIgnoreCase = false );

KICOMMON_API bool WildCompareString( const QString& pattern,
                        const QString& string_to_tst,
                        bool            case_sensitive = true );

KICOMMON_API int ValueStringCompare( const QString& strFWord, const QString& strSWord );

KICOMMON_API int SplitString( const QString& strToSplit,
                 QString* strBeginning,
                 QString* strDigits,
                 QString* strEnd );

KICOMMON_API int GetTrailingInt( const QString& aStr );

KICOMMON_API QString GetIllegalFileNameQtChars();

KICOMMON_API bool IsFullFileNameValid( const QString& aFullFilename );

KICOMMON_API bool ReplaceIllegalFileNameChars( std::string* aName, int aReplaceChar = 0 );
KICOMMON_API bool  ReplaceIllegalFileNameChars( QString& aName, int aReplaceChar = 0 );


struct rsort_QString
{
    bool operator() ( const QString& strA, const QString& strB ) const
    {
        QString::const_reverse_iterator sA = strA.crbegin();
        QString::const_reverse_iterator eA = strA.crend();

        QString::const_reverse_iterator sB = strB.crbegin();
        QString::const_reverse_iterator eB = strB.crend();

        if( strA.isEmpty() )
        {
            if( strB.isEmpty() )
                return false;

            return true;
        }

        if( strB.isEmpty() )
            return false;

        while( sA != eA && sB != eB )
        {
            if( ( *sA ) == ( *sB ) )
            {
                ++sA;
                ++sB;
                continue;
            }

            if( ( *sA ) < ( *sB ) )
                return true;
            else
                return false;
        }

        if( sB == eB )
            return false;

        return true;
    }
};

static inline std::vector<std::string> split( const std::string& aStr, const std::string& aDelim )
{
    size_t pos = 0;
    size_t last_pos = 0;
    size_t len;

    std::vector<std::string> tokens;

    while( pos < aStr.size() )
    {
        pos = aStr.find_first_of( aDelim, last_pos );

        if( pos == std::string::npos )
            pos = aStr.size();

        len = pos - last_pos;

        tokens.push_back( aStr.substr( last_pos, len ) );

        last_pos = pos + 1;
    }

    return tokens;
}

inline void AccumulateDescription( QString& aDesc, const QString& aItem )
{
    if( !aDesc.isEmpty() )
        aDesc += ", ";

    aDesc += aItem;
}

KICOMMON_API void qtStringSplit( const QString& aText, std::vector<std::string>& aStrings, QChar aSplitter );

KICOMMON_API void StripTrailingZeros( QString& aStringValue, unsigned aTrailingZeroAllowed = 1 );

KICOMMON_API std::string UIDouble2Str( double aValue );

KICOMMON_API std::string FormatDouble2Str( double aValue );

#define TO_UTF8( qtstring ) ( (const char*) ( qtstring ).toUtf8().constData() )

KICOMMON_API QString From_UTF8( const std::string& aString );
KICOMMON_API QString  From_UTF8( const char* cstring );

KICOMMON_API QString NormalizeFileUri( const QString& aFileUri );


#endif  // STRING_UTILS_H
