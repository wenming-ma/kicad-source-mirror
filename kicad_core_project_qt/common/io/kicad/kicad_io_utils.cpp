#include "io/kicad/kicad_io_utils.h"

#include <QByteArray>
#include <QIODevice>
#include <QString>

#include <fmt/format.h>

#include <kiid.h>
#include <richio.h>
#include <string_utils.h>

namespace KICAD_FORMAT {

void FormatBool( OUTPUTFORMATTER* aOut, const QString& aKey, bool aValue )
{
    aOut->Print( "(%s %s)", aKey.toUtf8().constData(), aValue ? "yes" : "no" );
}


void FormatUuid( OUTPUTFORMATTER* aOut, const KIID& aUuid )
{
    aOut->Print( "(uuid %s)", aOut->Quotew( aUuid.AsString() ).c_str() );
}


void FormatStreamData( OUTPUTFORMATTER& aOut, QIODevice& aStream )
{
    aOut.Print( "(data" );

    // Read all data from the QIODevice
    QByteArray data = aStream.readAll();
    const QString out = QString::fromLatin1( data.toBase64() );

    static constexpr unsigned MIME_BASE64_LENGTH = 76;

    size_t first = 0;

    while( first < static_cast<size_t>( out.length() ) )
    {
        aOut.Print( "\n\"%s\"", TO_UTF8( out.mid( first, MIME_BASE64_LENGTH ) ) );
        first += MIME_BASE64_LENGTH;
    }

    aOut.Print( ")" );
}


void Prettify( std::string& aSource, bool aCompactSave )
{
    // Configuration
    const char quoteChar = '"';
    const char indentChar = '\t';
    const int  indentSize = 1;

    const int  xySpecialCaseColumnLimit = 99;
    const int  consecutiveTokenWrapThreshold = 72;

    std::string formatted;
    formatted.reserve( aSource.length() );

    auto cursor = aSource.begin();
    auto seek = cursor;

    int  listDepth = 0;
    char lastNonWhitespace = 0;
    bool inQuote = false;
    bool hasInsertedSpace = false;
    bool inMultiLineList = false;
    bool inXY = false;
    bool inShortForm = false;
    int  shortFormDepth = 0;
    int  column = 0;
    int  backslashCount = 0;

    auto isWhitespace = []( const char aChar )
            {
                return ( aChar == ' ' || aChar == '\t' || aChar == '\n' || aChar == '\r' );
            };

    auto nextNonWhitespace =
            [&]( std::string::iterator aIt )
            {
                seek = aIt;

                while( seek != aSource.end() && isWhitespace( *seek ) )
                    seek++;

                if( seek == aSource.end() )
                    return (char)0;

                return *seek;
            };

    auto isXY =
            [&]( std::string::iterator aIt )
            {
                seek = aIt;

                if( ++seek == aSource.end() || *seek != 'x' )
                    return false;

                if( ++seek == aSource.end() || *seek != 'y' )
                    return false;

                if( ++seek == aSource.end() || *seek != ' ' )
                    return false;

                return true;
            };

    auto isShortForm =
            [&]( std::string::iterator aIt )
            {
                seek = aIt;
                std::string token;

                while( ++seek != aSource.end() && isalpha( *seek ) )
                    token += *seek;

                return token == "font" || token == "stroke" || token == "fill"
                        || token == "offset" || token == "rotate" || token == "scale";
            };

    while( cursor != aSource.end() )
    {
        char next = nextNonWhitespace( cursor );

        if( isWhitespace( *cursor ) && !inQuote )
        {
            if( !hasInsertedSpace
                && listDepth > 0
                && lastNonWhitespace != '('
                && next != ')'
                && next != '(' )
            {
                if( inXY || column < consecutiveTokenWrapThreshold )
                {
                    formatted.push_back( ' ' );
                    column++;
                }
                else if( inShortForm )
                {
                    formatted.push_back( ' ' );
                }
                else
                {
                    formatted += fmt::format( "\n{}",
                                              std::string( listDepth * indentSize, indentChar ) );
                    column = listDepth * indentSize;
                    inMultiLineList = true;
                }

                hasInsertedSpace = true;
            }
        }
        else
        {
            hasInsertedSpace = false;

            if( *cursor == '(' && !inQuote )
            {
                bool currentIsXY = isXY( cursor );
                bool currentIsShortForm = aCompactSave && isShortForm( cursor );

                if( formatted.empty() )
                {
                    formatted.push_back( '(' );
                    column++;
                }
                else if( inXY && currentIsXY && column < xySpecialCaseColumnLimit )
                {
                    formatted += " (";
                    column += 2;
                }
                else if( inShortForm )
                {
                    formatted += " (";
                    column += 2;
                }
                else
                {
                    formatted += fmt::format( "\n{}(",
                                              std::string( listDepth * indentSize, indentChar ) );
                    column = listDepth * indentSize + 1;
                }

                inXY = currentIsXY;

                if( currentIsShortForm )
                {
                    inShortForm = true;
                    shortFormDepth = listDepth;
                }

                listDepth++;
            }
            else if( *cursor == ')' && !inQuote )
            {
                if( listDepth > 0 )
                    listDepth--;

                if( inShortForm )
                {
                    formatted.push_back( ')' );
                    column++;
                }
                else if( lastNonWhitespace == ')' || inMultiLineList )
                {
                    formatted += fmt::format( "\n{})",
                                              std::string( listDepth * indentSize, indentChar ) );
                    column = listDepth * indentSize + 1;
                    inMultiLineList = false;
                }
                else
                {
                    formatted.push_back( ')' );
                    column++;
                }

                if( shortFormDepth == listDepth )
                {
                    inShortForm = false;
                    shortFormDepth = 0;
                }
            }
            else
            {
                if( *cursor == '\\' )
                    backslashCount++;
                else if( *cursor == quoteChar && ( backslashCount & 1 ) == 0 )
                    inQuote = !inQuote;

                if( *cursor != '\\' )
                    backslashCount = 0;

                formatted.push_back( *cursor );
                column++;
            }

            lastNonWhitespace = *cursor;
        }

        ++cursor;
    }

    formatted += '\n';

    aSource = std::move( formatted );
}

} // namespace KICAD_FORMAT
