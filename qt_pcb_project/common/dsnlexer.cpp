
#include <charconv>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>         // bsearch()
#include <cctype>

#include <dsnlexer.h>
#include <QString>
#include <QStringList>
#include <QCoreApplication>

#define _(s) QCoreApplication::translate("", (s))

#define FMT_CLIPBOARD       _( "clipboard" )


void DSNLEXER::init()
{
    curTok  = DSN_NONE;
    prevTok = DSN_NONE;

    stringDelimiter = '"';

    specctraMode = false;
    space_in_quoted_tokens = false;
    commentsAreTokens = false;
    SetKnowsBar( true );
    curOffset = 0;
}


DSNLEXER::DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
                    FILE* aFile, const QString& aFilename ) :
    iOwnReaders( true ),
    start( nullptr ),
    next( nullptr ),
    limit( nullptr ),
    reader( nullptr ),
    specctraMode( false ),
    m_knowsBar( false ),
    space_in_quoted_tokens( false ),
    commentsAreTokens( false ),
    keywords( aKeywordTable ),
    keywordCount( aKeywordCount ),
    keywordsLookup( aKeywordMap )
{
    PushReader( new FILE_LINE_READER( aFile, aFilename ) );
    init();
}


DSNLEXER::DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
                    const std::string& aClipboardTxt, const QString& aSource ) :
        iOwnReaders( true ),
        start( nullptr ),
        next( nullptr ),
        limit( nullptr ),
        reader( nullptr ),
        specctraMode( false ),
        m_knowsBar( false ),
        space_in_quoted_tokens( false ),
        commentsAreTokens( false ),
        keywords( aKeywordTable ),
        keywordCount( aKeywordCount ),
        keywordsLookup( aKeywordMap )
{
    PushReader( new STRING_LINE_READER( aClipboardTxt, aSource.isEmpty() ? QString( FMT_CLIPBOARD )
                                                                         : aSource ) );
    init();
}


DSNLEXER::DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
                    LINE_READER* aLineReader ) :
        iOwnReaders( false ),
        start( nullptr ),
        next( nullptr ),
        limit( nullptr ),
        reader( nullptr ),
        specctraMode( false ),
        m_knowsBar( false ),
        space_in_quoted_tokens( false ),
        commentsAreTokens( false ),
        keywords( aKeywordTable ),
        keywordCount( aKeywordCount ),
        keywordsLookup( aKeywordMap )
{
    if( aLineReader )
        PushReader( aLineReader );

    init();
}


static const KEYWORD empty_keywords[1] = {};

DSNLEXER::DSNLEXER( const std::string& aSExpression, const QString& aSource ) :
        iOwnReaders( true ),
        start( nullptr ),
        next( nullptr ),
        limit( nullptr ),
        reader( nullptr ),
        specctraMode( false ),
        m_knowsBar( false ),
        space_in_quoted_tokens( false ),
        commentsAreTokens( false ),
        keywords( empty_keywords ),
        keywordCount( 0 ),
        keywordsLookup( nullptr )
{
    PushReader( new STRING_LINE_READER( aSExpression, aSource.isEmpty() ? QString( FMT_CLIPBOARD )
                                                                        : aSource ) );
    init();
}


DSNLEXER::~DSNLEXER()
{
    if( iOwnReaders )
    {
        for( READER_STACK::iterator it = readerStack.begin(); it!=readerStack.end();  ++it )
            delete *it;
    }
}


void DSNLEXER::SetSpecctraMode( bool aMode )
{
    specctraMode = aMode;

    if( aMode )
    {
        space_in_quoted_tokens = true;
    }
    else
    {
        space_in_quoted_tokens = false;
        stringDelimiter = '"';
    }
}


void DSNLEXER::InitParserState()
{
    curTok  = DSN_NONE;
    prevTok = DSN_NONE;
    commentsAreTokens = false;

    curOffset = 0;
}


bool DSNLEXER::SyncLineReaderWith( DSNLEXER& aLexer )
{
    if( reader != aLexer.reader )
        return false;

    start = aLexer.start;
    next = aLexer.next;
    limit = aLexer.limit;

    curText = aLexer.curText;
    curOffset = aLexer.curOffset;

    return true;
}


void DSNLEXER::PushReader( LINE_READER* aLineReader )
{
    readerStack.push_back( aLineReader );
    reader = aLineReader;
    start  = (const char*) (*reader);

    limit = start;
    next  = start;
}


LINE_READER* DSNLEXER::PopReader()
{
    LINE_READER* ret = nullptr;

    if( readerStack.size() )
    {
        ret = reader;
        readerStack.pop_back();

        if( readerStack.size() )
        {
            reader = readerStack.back();
            start  = reader->Line();

            limit = start;
            next  = start;
        }
        else
        {
            reader = nullptr;
            start  = dummy;
            limit  = dummy;
        }
    }
    return ret;
}


int DSNLEXER::findToken( const std::string& tok ) const
{
    if( keywordsLookup != nullptr )
    {
        KEYWORD_MAP::const_iterator it = keywordsLookup->find( tok.c_str() );

        if( it != keywordsLookup->end() )
            return it->second;
    }

    return DSN_SYMBOL;
}


const char* DSNLEXER::Syntax( int aTok )
{
    const char* ret;

    switch( aTok )
    {
    case DSN_NONE:
        ret = "NONE";
        break;
    case DSN_STRING_QUOTE:
        ret = "string_quote";
        break;
    case DSN_QUOTE_DEF:
        ret = "quoted text delimiter";
        break;
    case DSN_DASH:
        ret = "-";
        break;
    case DSN_SYMBOL:
        ret = "symbol";
        break;
    case DSN_NUMBER:
        ret = "number";
        break;
    case DSN_RIGHT:
        ret = ")";
        break;
    case DSN_LEFT:
        ret = "(";
        break;
    case DSN_STRING:
        ret = "quoted string";
        break;
    case DSN_EOF:
        ret = "end of input";
        break;
    case DSN_BAR:
        ret = "|";
        break;
    default:
        ret = "???";
    }

    return ret;
}


const char* DSNLEXER::GetTokenText( int aTok ) const
{
    const char* ret;

    if( aTok < 0 )
        return Syntax( aTok );
    else if( (unsigned) aTok < keywordCount )
        ret = keywords[aTok].name;
    else
        ret = "token too big";

    return ret;
}


QString DSNLEXER::GetTokenString( int aTok ) const
{
    QString ret;

    ret += "'" + QString::fromUtf8( GetTokenText(aTok) ) + "'";

    return ret;
}


bool DSNLEXER::IsSymbol( int aTok )
{
    return aTok == DSN_SYMBOL || aTok == DSN_STRING || aTok >= 0;
}


bool DSNLEXER::IsNumber( int aTok )
{
    return aTok == DSN_NUMBER;
}


void DSNLEXER::Expecting( int aTok ) const
{
    QString errText = QString(
        _( "Expecting %s" ) ).arg( GetTokenString( aTok ) );
    THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
}


void DSNLEXER::Expecting( const char* text ) const
{
    QString errText = QString(
        _( "Expecting '%s'" ) ).arg( QString::fromUtf8( text ) );
    THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
}


void DSNLEXER::Unexpected( int aTok ) const
{
    QString errText = QString(
        _( "Unexpected %s" ) ).arg( GetTokenString( aTok ) );
    THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
}


void DSNLEXER::Duplicate( int aTok )
{
    QString errText = QString(
        _("%s is a duplicate") ).arg( GetTokenString( aTok ) );
    THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
}


void DSNLEXER::Unexpected( const char* text ) const
{
    QString errText = QString(
        _( "Unexpected '%s'" ) ).arg( QString::fromUtf8( text ) );
    THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
}


void DSNLEXER::NeedLEFT()
{
    int tok = NextTok();

    if( tok != DSN_LEFT )
        Expecting( DSN_LEFT );
}


void DSNLEXER::NeedRIGHT()
{
    int tok = NextTok();

    if( tok != DSN_RIGHT )
        Expecting( DSN_RIGHT );
}


void DSNLEXER::NeedBAR()
{
    int tok = NextTok();

    if( tok != DSN_BAR )
        Expecting( DSN_BAR );
}


int DSNLEXER::NeedSYMBOL()
{
    int tok = NextTok();

    if( !IsSymbol( tok ) )
        Expecting( DSN_SYMBOL );

    return tok;
}


int DSNLEXER::NeedSYMBOLorNUMBER()
{
    int  tok = NextTok();

    if( !IsSymbol( tok ) && !IsNumber( tok ) )
        Expecting( "a symbol or number" );

    return tok;
}


int DSNLEXER::NeedNUMBER( const char* aExpectation )
{
    int tok = NextTok();

    if( !IsNumber( tok ) )
    {
        QString errText = QString( _( "need a number for '%s'" ) ).arg(
                                             QString::fromUtf8( aExpectation ) );
        THROW_PARSE_ERROR( errText, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
    }

    return tok;
}


static bool isSpace( char cc )
{
    if( (unsigned char) cc <= ' ' )
    {
        switch( (unsigned char) cc )
        {
        case ' ':
        case '\n':
        case '\r':
        case '\t':
        case '\0':
            return true;
        }
    }

    return false;
}


inline bool isDigit( char cc )
{
    return '0' <= cc && cc <= '9';
}


inline bool DSNLEXER::isSep( char cc )
{
    return isSpace( cc ) || cc == '(' || cc == ')' || ( m_knowsBar && cc == '|' );
}


static bool isNumber( const char* cp, const char* limit )
{
    bool sawNumber = false;

    if( cp < limit && ( *cp=='-' || *cp=='+' ) )
        ++cp;

    while( cp < limit && isDigit( *cp ) )
    {
        ++cp;
        sawNumber = true;
    }

    if( cp < limit && *cp == '.' )
    {
        ++cp;

        while( cp < limit && isDigit( *cp ) )
        {
            ++cp;
            sawNumber = true;
        }
    }

    if( sawNumber )
    {
        if( cp < limit && ( *cp=='E' || *cp=='e' ) )
        {
            ++cp;

            sawNumber = false;

            if( cp < limit && ( *cp=='-' || *cp=='+' )  )
                ++cp;

            while( cp < limit && isDigit( *cp ) )
            {
                ++cp;
                sawNumber = true;
            }
        }
    }

    return sawNumber && cp==limit;
}


int DSNLEXER::NextTok()
{
    const char*   cur  = next;
    const char*   head = cur;

    prevTok = curTok;

    if( curTok == DSN_EOF )
        goto exit;

    if( cur >= limit )
    {
L_read:
        int len = readLine();

        if( len == 0 )
        {
            cur = start;
            curTok = DSN_EOF;
            goto exit;
        }

        cur = start;

        while( cur < limit && isSpace( *cur ) )
            ++cur;

        if( cur<limit && *cur=='#' )
        {
            if( commentsAreTokens )
            {
                while( limit[-1] == '\n' || limit[-1] == '\r' )
                    --limit;

                curText.clear();
                curText.append( start, limit );

                cur     = start;
                curTok  = DSN_COMMENT;
                head    = limit;
                goto exit;
            }
            else
            {
                goto L_read;
            }
        }
    }
    else
    {
        while( cur < limit && isSpace( *cur ) )
            ++cur;
    }

    if( cur >= limit )
        goto L_read;

    if( *cur == '(' )
    {
        curText = *cur;
        curTok = DSN_LEFT;
        head = cur+1;
        goto exit;
    }

    if( *cur == ')' )
    {
        curText = *cur;
        curTok = DSN_RIGHT;
        head = cur+1;
        goto exit;
    }

    if( m_knowsBar && *cur == '|' )
    {
        curText = *cur;
        curTok = DSN_BAR;
        head = cur+1;
        goto exit;
    }

    if( !specctraMode )
    {
        if( *cur == stringDelimiter )
        {
            curText.clear();

            ++cur;

            head = cur;

            while( head<limit )
            {
                if( *head =='\\' )
                {
                    char    tbuf[8];
                    char    c;
                    int     i;

                    if( ++head >= limit )
                        break;

                    switch( *head++ )
                    {
                    case '"':
                    case '\\':  c = head[-1];   break;
                    case 'a':   c = '\x07';     break;
                    case 'b':   c = '\x08';     break;
                    case 'f':   c = '\x0c';     break;
                    case 'n':   c = '\n';       break;
                    case 'r':   c = '\r';       break;
                    case 't':   c = '\x09';     break;
                    case 'v':   c = '\x0b';     break;

                    case 'x':
                        for( i = 0; i < 2; ++i )
                        {
                            if( !isxdigit( head[i] ) )
                                break;

                            tbuf[i] = head[i];
                        }

                        tbuf[i] = '\0';

                        if( i > 0 )
                            c = (char) strtoul( tbuf, nullptr, 16 );
                        else
                            c = 'x';

                        head += i;
                        break;

                    default:
                        --head;

                        for( i=0; i<3; ++i )
                        {
                            if( head[i] < '0' || head[i] > '7' )
                                break;

                            tbuf[i] = head[i];
                        }

                        tbuf[i] = '\0';

                        if( i > 0 )
                            c = (char) strtoul( tbuf, nullptr, 8 );
                        else
                            c = '\\';

                        head += i;
                        break;
                    }

                    curText += c;
                }

                else if( *head == '"' )
                {
                    curTok = DSN_STRING;
                    ++head;
                    goto exit;
                }

                else
                    curText += *head++;

            }

            QString errtxt( _( "Un-terminated delimited string" ) );
            THROW_PARSE_ERROR( errtxt, CurSource(), CurLine(), CurLineNumber(),
                               cur - start + curText.length() );
        }
    }
    else
    {
        if( *cur == '-' && cur>start && !isSpace( cur[-1] ) )
        {
            curText = '-';
            curTok = DSN_DASH;
            head = cur+1;
            goto exit;
        }

        if( prevTok == DSN_STRING_QUOTE )
        {
            static const QString errtxt( _("String delimiter must be a single character of "
                                            "', \", or $") );

            char cc = *cur;
            switch( cc )
            {
            case '\'':
            case '$':
            case '"':
                break;
            default:
                THROW_PARSE_ERROR( errtxt, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
            }

            curText = cc;

            head = cur+1;

            if( head<limit && !isSep( *head ) )
            {
                THROW_PARSE_ERROR( errtxt, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
            }

            curTok = DSN_QUOTE_DEF;
            goto exit;
        }

        if( *cur == stringDelimiter )
        {
            ++cur;

            head = cur;

            while( head<limit  &&  !isStringTerminator( *head ) )
                ++head;

            if( head >= limit )
            {
                QString errtxt( _( "Un-terminated delimited string" ) );
                THROW_PARSE_ERROR( errtxt, CurSource(), CurLine(), CurLineNumber(), CurOffset() );
            }

            curText.clear();
            curText.append( cur, head );

            ++head;

            curTok  = DSN_STRING;
            goto exit;
        }
    }

    curText.clear();

    head = cur;
    while( head<limit && !isSep( *head ) )
        curText += *head++;

    if( isNumber( curText.c_str(), curText.c_str() + curText.size() ) )
    {
        curTok = DSN_NUMBER;
        goto exit;
    }

    if( specctraMode && curText == "string_quote" )
    {
        curTok = DSN_STRING_QUOTE;
        goto exit;
    }

    curTok = findToken( curText );

exit:

    curOffset = cur - start;

    next = head;

    return curTok;
}


QStringList* DSNLEXER::ReadCommentLines()
{
    QStringList*    ret = nullptr;
    bool            cmt_setting = SetCommentsAreTokens( true );
    int             tok = NextTok();

    if( tok == DSN_COMMENT )
    {
        ret = new QStringList();

        do
        {
            ret->append( FromUTF8() );
        }
        while( ( tok = NextTok() ) == DSN_COMMENT );
    }

    SetCommentsAreTokens( cmt_setting );

    return ret;
}


double DSNLEXER::parseDouble()
{
#if ( defined( __GNUC__ ) && __GNUC__ < 11 ) || ( defined( __clang__ ) && __clang_major__ < 13 )
    char* tmp;

    errno = 0;

    double fval = strtod( CurText(), &tmp );

    if( errno )
    {
        QString error;
        error = QString( _( "Invalid floating point number in\nfile: '%s'\nline: %d\noffset: %d" ) )
                    .arg( CurSource() ).arg( CurLineNumber() ).arg( CurOffset() );

        THROW_IO_ERROR( error );
    }

    if( CurText() == tmp )
    {
        QString error;
        error = QString( _( "Missing floating point number in\nfile: '%s'\nline: %d\noffset: %d" ) )
                    .arg( CurSource() ).arg( CurLineNumber() ).arg( CurOffset() );

        THROW_IO_ERROR( error );
    }

    return fval;
#else
    const std::string& str = CurStr();

    size_t woff = 0;

    while( std::isspace( str[woff] ) && woff < str.length() )
    {
        woff++;
    }

    double                 dval{};
    std::from_chars_result res =
            std::from_chars( str.data() + woff, str.data() + str.size(), dval );

    if( res.ec != std::errc() )
    {
        THROW_PARSE_ERROR( _( "Invalid floating point number" ), CurSource(), CurLine(),
                           CurLineNumber(), CurOffset() );
    }

    return dval;
#endif
}
