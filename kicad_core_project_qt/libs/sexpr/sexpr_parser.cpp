// QT_TRANSFORMATION_COMPLETED

#include "sexpr/sexpr_parser.h"
#include "sexpr/sexpr_exception.h"
#include <cctype>
#include <cstdlib>
#include <iterator>
#include <stdexcept>

#include <fstream>
#include <streambuf>
#include <QFile>
#include <QTextStream>

#include <string_utils.h>

namespace SEXPR
{
    const std::string PARSER::whitespaceCharacters = " \t\n\r\b\f\v";

    PARSER::PARSER() : m_lineNumber( 1 )
    {
    }

    PARSER::~PARSER()
    {
    }

    std::unique_ptr<SEXPR> PARSER::Parse( const std::string& aString )
    {
        std::string::const_iterator it = aString.begin();
        return parseString( aString, it );
    }

    std::unique_ptr<SEXPR> PARSER::ParseFromFile( const std::string& aFileName )
    {
        std::string str = GetFileContents( aFileName );

        std::string::const_iterator it = str.begin();
        return parseString( str, it );
    }

    std::string PARSER::GetFileContents( const std::string &aFileName )
    {
        std::string str;

        // the filename is not always a UTF7 string, so do not use ifstream
        // that do not work with unicode chars.
        QString fname = From_UTF8( aFileName.c_str() );
        QFile file( fname );
        
        if( !file.open( QIODevice::ReadOnly ) )
        {
            throw PARSE_EXCEPTION( "Error occurred attempting to read in file or empty file" );
        }

        qint64 length = file.size();

        if( length <= 0 )
        {
            throw PARSE_EXCEPTION( "Error occurred attempting to read in file or empty file" );
        }

        str.resize( length );
        qint64 bytesRead = file.read( &str[0], str.length() );
        
        if( bytesRead != length )
        {
            throw PARSE_EXCEPTION( "Error occurred attempting to read file contents" );
        }

        return str;
    }

    std::unique_ptr<SEXPR> PARSER::parseString( const std::string& aString,
                                                std::string::const_iterator& it )
    {
        for( ; it != aString.end(); ++it )
        {
            if( *it == '\n' )
                m_lineNumber++;

            if( whitespaceCharacters.find(*it) != std::string::npos )
                continue;

            if( *it == '(' )
            {
                std::advance( it, 1 );

                auto list = std::make_unique<SEXPR_LIST>( m_lineNumber );

                while( it != aString.end() && *it != ')' )
                {
                    //there may be newlines in between atoms of a list, so detect these here
                    if( *it == '\n' )
                        m_lineNumber++;

                    if( whitespaceCharacters.find( *it ) != std::string::npos )
                    {
                        std::advance( it, 1 );
                        continue;
                    }

                    std::unique_ptr<SEXPR> item = parseString( aString, it );
                    list->AddChild( item.release() );
                }

                if( it != aString.end() )
                    std::advance( it, 1 );

                return list;
            }
            else if( *it == ')' )
            {
                return nullptr;
            }
            else if( *it == '"' )
            {
                ++it;

                auto starting_it = it;

                for( ; it != aString.end(); ++it )
                {
                    auto ch = *it;

                    if( ch == '\\' )
                    {
                        // Skip the next escaped character
                        if( ++it == aString.end() )
                            break;

                        continue;
                    }

                    if( ch == '"' )
                        break;
                }

                if( it == aString.end() )
                    throw PARSE_EXCEPTION("missing closing quote");

                auto str = std::make_unique<SEXPR_STRING>( std::string( starting_it, it ),
                        m_lineNumber );

                ++it;
                return str;

            }
            else
            {
                size_t startPos = std::distance( aString.begin(), it );
                size_t closingPos = aString.find_first_of( whitespaceCharacters + "()", startPos );

                std::string tmp = aString.substr( startPos, closingPos - startPos );


                if( closingPos != std::string::npos )
                {
                    if( tmp.find_first_not_of( "0123456789." ) == std::string::npos ||
                        ( tmp.size() > 1 && tmp[0] == '-'
                          && tmp.find_first_not_of( "0123456789.", 1 ) == std::string::npos ) )
                    {
                        std::unique_ptr<SEXPR> res;

                        if( tmp.find( '.' ) != std::string::npos )
                        {
                            res = std::make_unique<SEXPR_DOUBLE>(
                                    strtod( tmp.c_str(), nullptr ), m_lineNumber );
                            //floating point type
                        }
                        else
                        {
                            res = std::make_unique<SEXPR_INTEGER>(
                                    strtoll( tmp.c_str(), nullptr, 0 ), m_lineNumber );
                        }

                        std::advance( it, closingPos - startPos );
                        return res;
                    }
                    else
                    {
                        auto str = std::make_unique<SEXPR_SYMBOL>( tmp, m_lineNumber );
                        std::advance( it, closingPos - startPos );

                        return str;
                    }
                }
                else
                {
                    throw PARSE_EXCEPTION( "format error" );
                }
            }
        }

        return nullptr;
    }
}
