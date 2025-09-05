
#include <clocale>
#include <cmath>
#include <map>
#include <core/map_helpers.h>
#include <fmt/core.h>
#include <macros.h>
#include <richio.h>                        // StrPrintf
#include <string_utils.h>
#include <fmt/chrono.h>
#include <QRegularExpression>
#include <QString>
#include <QChar>
#include <QRegExp>
#include <vector>
#include <algorithm>
#include "locale_io.h"


static const char illegalFileNameChars[] = "\\/:\"<>|*?";

bool IsFullFileNameValid( const QString& aFullFilename )
{

    // Test for forbidden chars in aFullFilename.
    // '\'and '/' are allowed here because aFullFilename can be a full path, and
    // ':' is allowed on Windows as second char in string.
    // So remove allowed separators from string to test
    QString filtered_fullpath = aFullFilename;

#ifdef __WINDOWS__
    // On MSW, path separators need special handling
    // '\'and '/'
    filtered_fullpath.replace( "/", "_" );
    filtered_fullpath.replace( "\\", "_" );

    // A disk identifier is allowed, and therefore remove its separator
    if( filtered_fullpath.length() > 1 && filtered_fullpath[1] == ':' )
        filtered_fullpath[1] = ' ';
#endif

    if( -1 != filtered_fullpath.indexOf( QRegExp( "[\\/:*?\"<>|]" ) ) )
        return false;

    return true;
}


QString ConvertToNewOverbarNotation( const QString& aOldStr )
{
    QString newStr;
    bool inOverbar = false;

    // Don't get tripped up by the legacy empty-string token.
    if( aOldStr == "~" )
        return aOldStr;

    newStr.reserve( aOldStr.length() );

    for( QString::const_iterator chIt = aOldStr.begin(); chIt != aOldStr.end(); ++chIt )
    {
        if( *chIt == '~' )
        {
            QString::const_iterator lookahead = chIt + 1;

            if( lookahead != aOldStr.end() && *lookahead == '~' )
            {
                if( ++lookahead != aOldStr.end() && *lookahead == '{' )
                {
                    // This way the subsequent opening curly brace will not start an
                    // overbar.
                    newStr += "~~{}";
                    continue;
                }

                // Two subsequent tildes mean a tilde.
                newStr += "~";
                ++chIt;
                continue;
            }
            else if( lookahead != aOldStr.end() && *lookahead == '{' )
            {
                // Could mean the user wants "{" with an overbar, but more likely this
                // is a case of double notation conversion.  Bail out.
                return aOldStr;
            }
            else
            {
                if( inOverbar )
                {
                    newStr += "}";
                    inOverbar = false;
                }
                else
                {
                    newStr += "~{";
                    inOverbar = true;
                }

                continue;
            }
        }
        else if( ( *chIt == ' ' || *chIt == '}' || *chIt == ')' ) && inOverbar )
        {
            // Spaces were used to terminate overbar as well
            newStr += "}";
            inOverbar = false;
        }

        newStr += *chIt;
    }

    // Explicitly end the overbar even if there was no terminating '~' in the aOldStr.
    if( inOverbar )
        newStr += "}";

    return newStr;
}


bool ConvertSmartQuotesAndDashes( QString* aString )
{
    bool retVal = false;

    for( QString::iterator ii = aString->begin(); ii != aString->end(); ++ii )
    {
        if( *ii == L'\u00B4' || *ii == L'\u2018' || *ii == L'\u2019' )
        {
            *ii = '\'';
            retVal = true;
        }
        if( *ii == L'\u201C' || *ii == L'\u201D' )
        {
            *ii = '"';
            retVal = true;
        }
        if( *ii == L'\u2013' || *ii == L'\u2014' )
        {
            *ii = '-';
            retVal = true;
        }
    }

    return retVal;
}


QString EscapeString( const QString& aSource, ESCAPE_CONTEXT aContext )
{
    QString          converted;
    std::vector<bool> braceStack;    // true == formatting construct

    converted.reserve( aSource.length() );

    for( QChar c: aSource )
    {
        if( aContext == CTX_NETNAME )
        {
            if( c == '/' )
                converted += "{slash}";
            else if( c == '\n' || c == '\r' )
                converted += QString();    // drop
            else
                converted += c;
        }
        else if( aContext == CTX_LIBID || aContext == CTX_LEGACY_LIBID )
        {
            // We no longer escape '/' in LIB_IDs, but we used to
            if( c == '/' && aContext == CTX_LEGACY_LIBID )
                converted += "{slash}";
            else if( c == '\\' )
                converted += "{backslash}";
            else if( c == '<' )
                converted += "{lt}";
            else if( c == '>' )
                converted += "{gt}";
            else if( c == ':' )
                converted += "{colon}";
            else if( c == '\"' )
                converted += "{dblquote}";
            else if( c == '\n' || c == '\r' )
                converted += QString();    // drop
            else
                converted += c;
        }
        else if( aContext == CTX_IPC )
        {
            if( c == '/' )
                converted += "{slash}";
            else if( c == ',' )
                converted += "{comma}";
            else if( c == '\"' )
                converted += "{dblquote}";
            else
                converted += c;
        }
        else if( aContext == CTX_QUOTED_STR )
        {
            if( c == '\"' )
                converted += "{dblquote}";
            else
                converted += c;
        }
        else if( aContext == CTX_JS_STR )
        {
            if( c >= 0x7F || c == '\'' || c == '\\' || c == '(' || c == ')' )
            {
                unsigned int code = c;
                char buffer[16];
                snprintf( buffer, sizeof(buffer), "\\u%4.4X", code );
                converted += buffer;
            }
            else
            {
                converted += c;
            }
        }
        else if( aContext == CTX_LINE )
        {
            if( c == '\n' || c == '\r' )
                converted += "{return}";
            else
                converted += c;
        }
        else if( aContext == CTX_FILENAME )
        {
            if( c == '/' )
                converted += "{slash}";
            else if( c == '\\' )
                converted += "{backslash}";
            else if( c == '\"' )
                converted += "{dblquote}";
            else if( c == '<' )
                converted += "{lt}";
            else if( c == '>' )
                converted += "{gt}";
            else if( c == '|' )
                converted += "{bar}";
            else if( c == ':' )
                converted += "{colon}";
            else if( c == '\t' )
                converted += "{tab}";
            else if( c == '\n' || c == '\r' )
                converted += "{return}";
            else
                converted += c;
        }
        else if( aContext == CTX_NO_SPACE )
        {
            if( c == ' ' )
                converted += "{space}";
            else
                converted += c;
        }
        else if( aContext == CTX_CSV )
        {
            if( c == ',' )
                converted += "{comma}";
            else if( c == '\n' || c == '\r' )
                converted += "{return}";
            else
                converted += c;
        }
        else
        {
            converted += c;
        }
    }

    return converted;
}


QString UnescapeString( const QString& aSource )
{
    size_t sourceLen = aSource.length();

    // smallest escape string is three characters, shortcut everything else
    if( sourceLen <= 2 )
    {
        return aSource;
    }

    QString newbuf;
    newbuf.reserve( sourceLen );

    QChar prev = QChar(0);
    QChar ch = QChar(0);

    for( size_t i = 0; i < sourceLen; ++i )
    {
        prev = ch;
        ch = aSource[i];

        if( ch == '{' )
        {
            QString token;
            int      depth = 1;
            bool     terminated = false;

            for( i = i + 1; i < sourceLen; ++i )
            {
                ch = aSource[i];

                if( ch == '{' )
                    depth++;
                else if( ch == '}' )
                    depth--;

                if( depth <= 0 )
                {
                    terminated = true;
                    break;
                }
                else
                {
                    token += ch;
                }
            }

            if( !terminated )
            {
                newbuf += "{" + UnescapeString( token );
            }
            else if( prev == '$' || prev == '~' || prev == '^' || prev == '_' )
            {
                newbuf += "{" + UnescapeString( token ) + "}";
            }
            else if( token == "dblquote" )  newbuf += "\"";
            else if( token == "quote" )     newbuf += "'";
            else if( token == "lt" )        newbuf += "<";
            else if( token == "gt" )        newbuf += ">";
            else if( token == "backslash" ) newbuf += "\\";
            else if( token == "slash" )     newbuf += "/";
            else if( token == "bar" )       newbuf += "|";
            else if( token == "comma" )     newbuf += ",";
            else if( token == "colon" )     newbuf += ":";
            else if( token == "space" )     newbuf += " ";
            else if( token == "dollar" )    newbuf += "$";
            else if( token == "tab" )       newbuf += "\t";
            else if( token == "return" )    newbuf += "\n";
            else if( token == "brace" )     newbuf += "{";
            else
            {
                newbuf += "{" + UnescapeString( token ) + "}";
            }
        }
        else
        {
            newbuf += ch;
        }
    }

    return newbuf;
}


QString TitleCaps( const QString& aString )
{
    auto words = aString.split( ' ' );
    QString result;

    result.reserve( aString.length() );

    for( const QString& word : words )
    {
        if( !result.isEmpty() )
            result += " ";

        QString capitalizedWord = word;
        if( !capitalizedWord.isEmpty() )
            capitalizedWord[0] = capitalizedWord[0].toUpper();
        result += capitalizedWord;
    }

    return result;
}


int ReadDelimitedText( QString* aDest, const char* aSource )
{
    std::string utf8;               // utf8 but without escapes and quotes.
    bool        inside = false;
    const char* start = aSource;
    char        cc;

    while( (cc = *aSource++) != 0  )
    {
        if( cc == '"' )
        {
            if( inside )
                break;          // 2nd double quote is end of delimited text

            inside = true;      // first delimiter found, make note, do not copy
        }

        else if( inside )
        {
            if( cc == '\\' )
            {
                cc = *aSource++;

                if( !cc )
                    break;

                // do no copy the escape byte if it is followed by \ or "
                if( cc != '"' && cc != '\\' )
                    utf8 += '\\';

                utf8 += cc;
            }
            else
            {
                utf8 += cc;
            }
        }
    }

    *aDest = From_UTF8( utf8.c_str() );

    return aSource - start;
}


int ReadDelimitedText( char* aDest, const char* aSource, int aDestSize )
{
    if( aDestSize <= 0 )
        return 0;

    bool        inside = false;
    const char* start = aSource;
    char*       limit = aDest + aDestSize - 1;
    char        cc;

    while( ( cc = *aSource++ ) != 0 && aDest < limit )
    {
        if( cc == '"' )
        {
            if( inside )
                break;          // 2nd double quote is end of delimited text

            inside = true;      // first delimiter found, make note, do not copy
        }
        else if( inside )
        {
            if( cc == '\\' )
            {
                cc = *aSource++;

                if( !cc )
                    break;

                // do no copy the escape byte if it is followed by \ or "
                if( cc != '"' && cc != '\\' )
                    *aDest++ = '\\';

                if( aDest < limit )
                    *aDest++ = cc;
            }
            else
            {
                *aDest++ = cc;
            }
        }
    }

    *aDest = 0;

    return aSource - start;
}


std::string EscapedUTF8( const QString& aString )
{
    QString str = aString;

    // No new-lines allowed in quoted strings
    str.replace( "\r\n", "\r" );
    str.replace( "\n", "\r" );

    std::string utf8 = aString.toUtf8().constData();

    std::string ret;

    ret.reserve( utf8.length() + 2 );

    ret += '"';

    for( std::string::const_iterator it = utf8.begin();  it!=utf8.end();  ++it )
    {
        // this escaping strategy is designed to be compatible with ReadDelimitedText():
        if( *it == '"' )
        {
            ret += '\\';
            ret += '"';
        }
        else if( *it == '\\' )
        {
            ret += '\\';    // double it up
            ret += '\\';
        }
        else
        {
            ret += *it;
        }
    }

    ret += '"';

    return ret;
}


QString EscapeHTML( const QString& aString )
{
    QString converted;

    converted.reserve( aString.length() );

    for( QChar c : aString )
    {
        if( c == '\"' )
            converted += "&quot;";
        else if( c == '\'' )
            converted += "&apos;";
        else if( c == '&' )
            converted += "&amp;";
        else if( c == '<' )
            converted += "&lt;";
        else if( c == '>' )
            converted += "&gt;";
        else
            converted += c;
    }

    return converted;
}


QString UnescapeHTML( const QString& aString )
{
    // clang-format off
    static const std::map<QString, QString> c_replacements = {
        { "quot", "\"" },
        { "apos", "'" },
        { "amp", "&" },
        { "lt", "<" },
        { "gt", ">" }
    };
    // clang-format on

    // Construct regex
    QString regexStr = "&(#(\\d*)|#x([a-zA-Z0-9]{4})";

    for( auto& [key, value] : c_replacements )
        regexStr += '|' + key;

    regexStr += ");";

    QRegularExpression regex( regexStr );

    // Process matches
    QString result;
    QString str = aString;
    int offset = 0;

    QRegularExpressionMatchIterator it = regex.globalMatch( str );
    while( it.hasNext() )
    {
        QRegularExpressionMatch match = it.next();
        
        result += str.mid( offset, match.capturedStart() - offset );

        QString code = match.captured( 1 );
        QString codeDec = match.captured( 2 );
        QString codeHex = match.captured( 3 );

        if( !codeDec.isEmpty() || !codeHex.isEmpty() )
        {
            unsigned long codeVal = 0;

            if( !codeDec.isEmpty() )
                codeVal = codeDec.toULong();
            else if( !codeHex.isEmpty() )
                codeVal = codeHex.toULong( nullptr, 16 );

            if( codeVal != 0 )
                result += QChar( codeVal );
        }
        else if( auto val = get_opt( c_replacements, code ) )
        {
            result += *val;
        }

        offset = match.capturedEnd();
    }

    result += str.mid( offset );

    return result;
}


QString RemoveHTMLTags( const QString& aInput )
{
    QString str = aInput;
    str.replace( QRegularExpression( "<[^>]*>" ), QString() );

    return str;
}


QString LinkifyHTML( QString aStr )
{
    static QRegularExpression regex( "\\b(https?|ftp|file)://([-\\w+&@#/%?=~|!:,.;]*[^.,:;<>\\(\\)\\s\u00b6])",
                                    QRegularExpression::CaseInsensitiveOption );

    aStr.replace( regex, "<a href=\"\\0\">\\0</a>" );

    return aStr;
}


bool IsURL( QString aStr )
{
    static QRegularExpression regex( "(https?|ftp|file)://([-\\w+&@#/%?=~|!:,.;]*[^.,:;<>\\s\u00b6])",
                                    QRegularExpression::CaseInsensitiveOption );

    return regex.match( aStr ).hasMatch();
}


bool NoPrintableChars( const QString& aString )
{
    QString tmp = aString;

    return tmp.trimmed().isEmpty();
}


int PrintableCharCount( const QString& aString )
{
    int char_count = 0;
    int overbarDepth = -1;
    int superSubDepth = -1;
    int braceNesting = 0;

    for( auto chIt = aString.begin(), end = aString.end(); chIt < end; ++chIt )
    {
        if( *chIt == '\t' )
        {
            // We don't format tabs in bitmap text (where this is currently used), so just
            // drop them from the count.
            continue;
        }
        else if( *chIt == '^' && superSubDepth == -1 )
        {
            auto lookahead = chIt;

            if( ++lookahead != end && *lookahead == '{' )
            {
                chIt = lookahead;
                superSubDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if( *chIt == '_' && superSubDepth == -1 )
        {
            auto lookahead = chIt;

            if( ++lookahead != end && *lookahead == '{' )
            {
                chIt = lookahead;
                superSubDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if( *chIt == '~' && overbarDepth == -1 )
        {
            auto lookahead = chIt;

            if( ++lookahead != end && *lookahead == '{' )
            {
                chIt = lookahead;
                overbarDepth = braceNesting;
                braceNesting++;
                continue;
            }
        }
        else if( *chIt == '{' )
        {
            braceNesting++;
        }
        else if( *chIt == '}' )
        {
            if( braceNesting > 0 )
                braceNesting--;

            if( braceNesting == superSubDepth )
            {
                superSubDepth = -1;
                continue;
            }

            if( braceNesting == overbarDepth )
            {
                overbarDepth = -1;
                continue;
            }
        }

        char_count++;
    }

    return char_count;
}


char* StrPurge( char* text )
{
    static const char whitespace[] = " \t\n\r\f\v";

    if( text )
    {
        while( *text && strchr( whitespace, *text ) )
            ++text;

        char* cp = text + strlen( text ) - 1;

        while( cp >= text && strchr( whitespace, *cp ) )
            *cp-- = '\0';
    }

    return text;
}


char* GetLine( FILE* File, char* Line, int* LineNum, int SizeLine )
{
    do {
        if( fgets( Line, SizeLine, File ) == nullptr )
            return nullptr;

        if( LineNum )
            *LineNum += 1;

    } while( Line[0] == '#' || Line[0] == '\n' ||  Line[0] == '\r' || Line[0] == 0 );

    strtok( Line, "\n\r" );
    return Line;
}


QString GetISO8601CurrentDateTime()
{
    // on msys2 variant mingw64, in fmt::format the %z format
    // (offset from UTC in the ISO 8601 format, e.g. -0430) does not work,
    // and is in fact %Z (locale-dependent time zone name or abbreviation) and breaks our date.
    // However, on msys2 variant ucrt64, it works (this is not the same code in fmt::format)
#if defined(__MINGW32__) && !defined(_UCRT)
    return QString::fromStdString( fmt::format( "{:%FT%T}", fmt::localtime( std::time( nullptr ) ) ) );
#else
    return QString::fromStdString( fmt::format( "{:%FT%T%z}", fmt::localtime( std::time( nullptr ) ) ) );
#endif
}


int StrNumCmp( const QString& aString1, const QString& aString2, bool aIgnoreCase )
{
    int nb1 = 0, nb2 = 0;

    auto str1 = aString1.begin();
    auto str2 = aString2.begin();

    while( str1 != aString1.end() && str2 != aString2.end() )
    {
        QChar c1 = *str1;
        QChar c2 = *str2;

        if( c1.isDigit() && c2.isDigit() ) // Both characters are digits, do numeric compare.
        {
            nb1 = 0;
            nb2 = 0;

            do
            {
                c1 = *str1;
                nb1 = nb1 * 10 + (int) c1.unicode() - '0';
                ++str1;
            } while( str1 != aString1.end() && (*str1).isDigit() );

            do
            {
                c2 = *str2;
                nb2 = nb2 * 10 + (int) c2.unicode() - '0';
                ++str2;
            } while( str2 != aString2.end() && (*str2).isDigit() );

            if( nb1 < nb2 )
                return -1;

            if( nb1 > nb2 )
                return 1;

            c1 = ( str1 != aString1.end() ) ? *str1 : QChar( 0 );
            c2 = ( str2 != aString2.end() ) ? *str2 : QChar( 0 );
        }

        // Any numerical comparisons to here are identical.
        if( aIgnoreCase )
        {
            if( c1 != c2 )
            {
                QChar uc1 = c1.toUpper();
                QChar uc2 = c2.toUpper();

                if( uc1 != uc2 )
                    return uc1 < uc2 ? -1 : 1;
            }
        }
        else
        {
            if( c1 < c2 )
                return -1;

            if( c1 > c2 )
                return 1;
        }

        if( str1 != aString1.end() )
            ++str1;

        if( str2 != aString2.end() )
            ++str2;
    }

    if( str1 == aString1.end() && str2 != aString2.end() )
    {
        return -1;   // Identical to here but aString1 is longer.
    }
    else if( str1 != aString1.end() && str2 == aString2.end() )
    {
        return 1;    // Identical to here but aString2 is longer.
    }

    return 0;
}


bool WildCompareString( const QString& pattern, const QString& string_to_tst,
                        bool case_sensitive )
{
    const QChar* cp = nullptr;
    const QChar* mp = nullptr;
    const QChar* wild = nullptr;
    const QChar* str = nullptr;
    QString      _pattern, _string_to_tst;

    if( case_sensitive )
    {
        wild = pattern.constData();
        str = string_to_tst.constData();
    }
    else
    {
        _pattern = pattern.toUpper();
        _string_to_tst = string_to_tst.toUpper();
        wild = _pattern.constData();
        str = _string_to_tst.constData();
    }

    while( ( *str ) && ( *wild != '*' ) )
    {
        if( ( *wild != *str ) && ( *wild != '?' ) )
            return false;

        wild++;
        str++;
    }

    while( *str )
    {
        if( *wild == '*' )
        {
            if( !*++wild )
                return true;

            mp = wild;
            cp = str + 1;
        }
        else if( ( *wild == *str ) || ( *wild == '?' ) )
        {
            wild++;
            str++;
        }
        else
        {
            wild   = mp;
            str = cp++;
        }
    }

    while( *wild == '*' )
    {
        wild++;
    }

    return !*wild;
}


bool ApplyModifier( double& value, const QString& aString )
{
    static const QString modifiers( "pnuµμmkKM" );

    if( !aString.length() )
        return false;

    QChar   modifier;
    QString units;

    if( modifiers.indexOf( aString[ 0 ] ) >= 0 )
    {
        modifier = aString[ 0 ];
        units = aString.mid( 1 ).trimmed();
    }
    else
    {
        modifier = ' ';
        units = aString.mid( 0 ).trimmed();
    }

    if( units.length()
            && units.compare( "F", Qt::CaseInsensitive ) != 0
            && units.compare( "hz", Qt::CaseInsensitive ) != 0
            && units.compare( "W", Qt::CaseInsensitive ) != 0
            && units.compare( "V", Qt::CaseInsensitive ) != 0
            && units.compare( "A", Qt::CaseInsensitive ) != 0
            && units.compare( "H", Qt::CaseInsensitive ) != 0 )
    {
        return false;
    }

    if( modifier == 'p' )
        value *= 1.0e-12;
    if( modifier == 'n' )
        value *= 1.0e-9;
    else if( modifier == 'u' || modifier == QString( "µ" )[0] || modifier == QString( "μ" )[0] )
        value *= 1.0e-6;
    else if( modifier == 'm' )
        value *= 1.0e-3;
    else if( modifier == 'k' || modifier == 'K' )
        value *= 1.0e3;
    else if( modifier == 'M' )
        value *= 1.0e6;
    else if( modifier == 'G' )
        value *= 1.0e9;

    return true;
}


bool convertSeparators( QString* value )
{
    // Note: fetching the decimal separator from the current locale isn't a silver bullet because
    // it assumes the current computer's locale is the same as the locale the schematic was
    // authored in -- something that isn't true, for instance, when sharing designs through
    // DIYAudio.com.
    //
    // Some values are self-describing: multiple instances of a single separator character must be
    // thousands separators; a single instance of each character must be a thousands separator
    // followed by a decimal separator; etc.
    //
    // Only when presented with an ambiguous value do we fall back on the current locale.

    value->replace( " ", QString() );

    QChar ambiguousSeparator = '?';
    QChar thousandsSeparator = '?';
    bool   thousandsSeparatorFound = false;
    QChar decimalSeparator = '?';
    bool   decimalSeparatorFound = false;
    int    digits = 0;

    for( int ii = (int) value->length() - 1; ii >= 0; --ii )
    {
        QChar c = value->at( ii );

        if( c >= '0' && c <= '9' )
        {
            digits += 1;
        }
        else if( c == '.' || c == ',' )
        {
            if( decimalSeparator != '?' || thousandsSeparator != '?' )
            {
                // We've previously found a non-ambiguous separator...

                if( c == decimalSeparator )
                {
                    if( thousandsSeparatorFound )
                        return false;       // decimal before thousands
                    else if( decimalSeparatorFound )
                        return false;       // more than one decimal
                    else
                        decimalSeparatorFound = true;
                }
                else if( c == thousandsSeparator )
                {
                    if( digits != 3 )
                        return false;       // thousands not followed by 3 digits
                    else
                        thousandsSeparatorFound = true;
                }
            }
            else if( ambiguousSeparator != '?' )
            {
                // We've previously found a separator, but we don't know for sure which...

                if( c == ambiguousSeparator )
                {
                    // They both must be thousands separators
                    thousandsSeparator = ambiguousSeparator;
                    thousandsSeparatorFound = true;
                    decimalSeparator = c == '.' ? ',' : '.';
                }
                else
                {
                    // The first must have been a decimal, and this must be a thousands.
                    decimalSeparator = ambiguousSeparator;
                    decimalSeparatorFound = true;
                    thousandsSeparator = c;
                    thousandsSeparatorFound = true;
                }
            }
            else
            {
                // This is the first separator...

                // If it's preceded by a '0' (only), or if it's followed by some number of
                // digits not equal to 3, then it -must- be a decimal separator.
                //
                // In all other cases we don't really know what it is yet.

                if( ( ii == 1 && value->at( 0 ) == '0' ) || digits != 3 )
                {
                    decimalSeparator = c;
                    decimalSeparatorFound = true;
                    thousandsSeparator = c == '.' ? ',' : '.';
                }
                else
                {
                    ambiguousSeparator = c;
                }
            }

            digits = 0;
        }
        else
        {
            digits = 0;
        }
    }

    // If we found nothing definitive then we have to look at the current locale
    if( decimalSeparator == '?' && thousandsSeparator == '?' )
    {
        const struct lconv* lc = localeconv();

        decimalSeparator = lc->decimal_point[0];
        thousandsSeparator = decimalSeparator == '.' ? ',' : '.';
    }

    // Convert to C-locale
    value->replace( thousandsSeparator, QString() );
    value->replace( decimalSeparator, '.' );

    return true;
}


int ValueStringCompare( const QString& strFWord, const QString& strSWord )
{
    // Compare unescaped text
    QString fWord = UnescapeString( strFWord );
    QString sWord = UnescapeString( strSWord );

    // The different sections of the two strings
    QString strFWordBeg, strFWordMid, strFWordEnd;
    QString strSWordBeg, strSWordMid, strSWordEnd;

    // Split the two strings into separate parts
    SplitString( fWord, &strFWordBeg, &strFWordMid, &strFWordEnd );
    SplitString( sWord, &strSWordBeg, &strSWordMid, &strSWordEnd );

    // Compare the Beginning section of the strings
    int isEqual = strFWordBeg.compare( strSWordBeg, Qt::CaseInsensitive );

    if( isEqual > 0 )
    {
        return 1;
    }
    else if( isEqual < 0 )
    {
        return -1;
    }
    else
    {
        // If the first sections are equal compare their digits
        double lFirstNumber  = 0;
        double lSecondNumber = 0;
        bool   endingIsModifier = false;

        convertSeparators( &strFWordMid );
        convertSeparators( &strSWordMid );

        lFirstNumber = strFWordMid.toDouble();
        lSecondNumber = strSWordMid.toDouble();

        endingIsModifier |= ApplyModifier( lFirstNumber, strFWordEnd );
        endingIsModifier |= ApplyModifier( lSecondNumber, strSWordEnd );

        if( lFirstNumber > lSecondNumber )
            return 1;
        else if( lFirstNumber < lSecondNumber )
            return -1;
        // If the first two sections are equal and the endings are modifiers then compare them
        else if( !endingIsModifier )
            return strFWordEnd.compare( strSWordEnd, Qt::CaseInsensitive );
        // Ran out of things to compare; they must match
        else
            return 0;
    }
}


int SplitString( const QString& strToSplit,
                 QString* strBeginning,
                 QString* strDigits,
                 QString* strEnd )
{
    static const QString separators( ".," );

    // Clear all the return strings
    strBeginning->clear();
    strDigits->clear();
    strEnd->clear();

    // There no need to do anything if the string is empty
    if( strToSplit.length() == 0 )
        return 0;

    // Starting at the end of the string look for the first digit
    int ii;

    for( ii = (strToSplit.length() - 1); ii >= 0; ii-- )
    {
        if( strToSplit[ii].isDigit() )
            break;
    }

    // If there were no digits then just set the single string
    if( ii < 0 )
    {
        *strBeginning = strToSplit;
    }
    else
    {
        // Since there is at least one digit this is the trailing string
        *strEnd = strToSplit.mid( ii + 1 );

        // Go to the end of the digits
        int position = ii + 1;

        for( ; ii >= 0; ii-- )
        {
            if( !strToSplit[ii].isDigit() && separators.indexOf( strToSplit[ii] ) < 0 )
                break;
        }

        // If all that was left was digits, then just set the digits string
        if( ii < 0 )
            *strDigits = strToSplit.mid( 0, position );

        else
        {
            *strDigits    = strToSplit.mid( ii + 1, position - ii - 1 );
            *strBeginning = strToSplit.mid( 0, ii + 1 );
        }
    }

    return 0;
}


int GetTrailingInt( const QString& aStr )
{
    int number = 0;
    int base = 1;

    // Trim and extract the trailing numeric part
    int index = aStr.length() - 1;

    while( index >= 0 )
    {
        const char chr = aStr.at( index ).toLatin1();

        if( chr < '0' || chr > '9' )
            break;

        number += ( chr - '0' ) * base;
        base *= 10;
        index--;
    }

    return number;
}


QString GetIllegalFileNameWxChars()
{
    return QString::fromUtf8( illegalFileNameChars );
}


bool ReplaceIllegalFileNameChars( std::string* aName, int aReplaceChar )
{
    bool changed = false;
    std::string result;
    result.reserve( aName->length() );

    for( std::string::iterator it = aName->begin();  it != aName->end();  ++it )
    {
        if( strchr( illegalFileNameChars, *it ) )
        {
            if( aReplaceChar )
                StrPrintf( &result, "%c", aReplaceChar );
            else
                StrPrintf( &result, "%%%02x", *it );

            changed = true;
        }
        else
        {
            result += *it;
        }
    }

    if( changed )
        *aName = result;

    return changed;
}


bool ReplaceIllegalFileNameChars( QString& aName, int aReplaceChar )
{
    bool changed = false;
    QString result;
    result.reserve( aName.length() );
    QString illWChars = GetIllegalFileNameWxChars();

    for( QString::iterator it = aName.begin();  it != aName.end();  ++it )
    {
        if( illWChars.indexOf( *it ) != -1 )
        {
            if( aReplaceChar )
                result += aReplaceChar;
            else
                result += QString::asprintf( "%%%02x", it->unicode() );

            changed = true;
        }
        else
        {
            result += *it;
        }
    }

    if( changed )
        aName = result;

    return changed;
}


void QStringSplit( const QString& aText, std::vector<std::string>& aStrings, QChar aSplitter )
{
    QString tmp;

    for( int ii = 0; ii < aText.length(); ii++ )
    {
        if( aText[ii] == aSplitter )
        {
            aStrings.push_back( tmp.toStdString() );
            tmp.clear();
        }
        else
        {
            tmp += aText[ii];
        }
    }

    if( !tmp.isEmpty() )
        aStrings.push_back( tmp.toStdString() );
}


void StripTrailingZeros( QString& aStringValue, unsigned aTrailingZeroAllowed )
{
    struct lconv* lc      = localeconv();
    char          sep     = lc->decimal_point[0];
    int           sep_pos = aStringValue.indexOf( sep );

    if( sep_pos > 0 )
    {
        // We want to keep at least aTrailingZeroAllowed digits after the separator
        unsigned min_len = sep_pos + aTrailingZeroAllowed + 1;

        while( aStringValue.length() > (int)min_len )
        {
            if( aStringValue[aStringValue.length()-1] == '0' )
                aStringValue.chop( 1 );
            else
                break;
        }
    }
}


std::string FormatDouble2Str( double aValue )
{
    std::string buf;

    if( aValue != 0.0 && std::fabs( aValue ) <= 0.0001 )
    {
        buf = fmt::format( "{:.16f}", aValue );

        // remove trailing zeros (and the decimal marker if needed)
        while( !buf.empty() && buf[buf.size() - 1] == '0' )
        {
            buf.pop_back();
        }

        // if the value was really small
        // we may have just stripped all the zeros after the decimal
        if( buf[buf.size() - 1] == '.' )
        {
            buf.pop_back();
        }
    }
    else
    {
        buf = fmt::format( "{:.10g}", aValue );
    }

    return buf;
}


std::string UIDouble2Str( double aValue )
{
    char    buf[50];
    int     len;

    if( aValue != 0.0 && std::fabs( aValue ) <= 0.0001 )
    {
        // For these small values, %f works fine,
        // and %g gives an exponent
        len = snprintf( buf, sizeof( buf ), "%.16f", aValue );

        while( --len > 0 && buf[len] == '0' )
            buf[len] = '\0';

        if( buf[len] == '.' || buf[len] == ',' )
            buf[len] = '\0';
        else
            ++len;
    }
    else
    {
        // For these values, %g works fine, and sometimes %f
        // gives a bad value (try aValue = 1.222222222222, with %.16f format!)
        len = snprintf( buf, sizeof( buf ), "%.10g", aValue );
    }

    return std::string( buf, len );
}


QString From_UTF8( const char* cstring )
{
    // Convert an expected UTF8 encoded C string to a QString
    QString line = QString::fromUtf8( cstring );

    if( line.isEmpty() )  // happens when cstring is not a valid UTF8 sequence
    {
        line = QString::fromLocal8Bit( cstring );    // try to use locale conversion

        if( line.isEmpty() )
            line = QString::fromLatin1( cstring );    // try to use latin1 string
    }

    return line;
}


QString From_UTF8( const std::string& aString )
{
    // Convert an expected UTF8 encoded std::string to a QString
    QString line = QString::fromUtf8( aString.c_str() );

    if( line.isEmpty() )  // happens when aString is not a valid UTF8 sequence
    {
        line = QString::fromLocal8Bit( aString.c_str() );    // try to use locale conversion

        if( line.isEmpty() )
            line = QString::fromLatin1( aString.c_str() );    // try to use latin1 string
    }

   return line;
}


QString NormalizeFileUri( const QString& aFileUri )
{
    QString uriPathAndFileName;

    if( !aFileUri.startsWith( "file://" ) )
        return aFileUri;
    
    uriPathAndFileName = aFileUri.mid( 7 ); // Remove "file://"

    QString tmp = uriPathAndFileName;
    QString retv = "file://";

    tmp.replace( "\\", "/" );
    tmp.replace( ":", "" );

    if( !tmp.isEmpty() && tmp[0] != '/' )
        tmp = "/" + tmp;

    retv += tmp;

    return retv;
}
