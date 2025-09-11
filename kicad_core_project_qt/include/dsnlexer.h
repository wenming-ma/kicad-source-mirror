
#ifndef DSNLEXER_H_
#define DSNLEXER_H_

#include <kicommon.h>
#include <cstdio>
#include <hashtables.h>
#include <string>
#include <vector>

#include <richio.h>

#ifndef SWIG
struct KICOMMON_API KEYWORD
{
    const char* name;
    int         token;
};
#endif // SWIG

// something like this macro can be used to help initialize a KEYWORD table.
// see SPECCTRA_DB::keywords[] as an example.

//#define TOKDEF(x)    { #x, T_##x }


enum DSN_SYNTAX_T
{
    DSN_NONE         = -12,
    DSN_BAR          = -11, // Also called pipe '|'
    DSN_COMMENT      = -10,
    DSN_STRING_QUOTE = -9,
    DSN_QUOTE_DEF    = -8,
    DSN_DASH         = -7,
    DSN_SYMBOL       = -6,
    DSN_NUMBER       = -5,
    DSN_RIGHT        = -4,  // right bracket, ')'
    DSN_LEFT         = -3,  // left bracket, '('
    DSN_STRING       = -2,  // a quoted string, stripped of the quotes
    DSN_EOF          = -1   // special case for end of file
};


class KICOMMON_API DSNLEXER
{
public:
    DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
              FILE* aFile, const QString& aFileName );

    DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
              const std::string& aSExpression, const QString& aSource = QString() );

    DSNLEXER( const std::string& aSExpression, const QString& aSource = QString() );

    DSNLEXER( const KEYWORD* aKeywordTable, unsigned aKeywordCount, const KEYWORD_MAP* aKeywordMap,
              LINE_READER* aLineReader = nullptr );

    virtual ~DSNLEXER();

    void InitParserState();

    bool SyncLineReaderWith( DSNLEXER& aLexer );

    void SetSpecctraMode( bool aMode );

    void PushReader( LINE_READER* aLineReader );

    LINE_READER* PopReader();

    int NextTok();

    int NeedSYMBOL();

    int NeedSYMBOLorNUMBER();

    int NeedNUMBER( const char* aExpectation );

    int CurTok() const
    {
        return curTok;
    }

    int PrevTok() const
    {
        return prevTok;
    }

    int GetCurStrAsToken() const
    {
        return findToken( curText );
    }

    char SetStringDelimiter( char aStringDelimiter )
    {
        char old = stringDelimiter;

        if( specctraMode )
            stringDelimiter = aStringDelimiter;

        return old;
    }

    bool SetSpaceInQuotedTokens( bool val )
    {
        bool old = space_in_quoted_tokens;

        if( specctraMode )
            space_in_quoted_tokens = val;

        return old;
    }

    void SetKnowsBar( bool knowsBar = true )
    {
        m_knowsBar = knowsBar;
    }

    bool SetCommentsAreTokens( bool val )
    {
        bool old = commentsAreTokens;
        commentsAreTokens = val;
        return old;
    }

    QStringList* ReadCommentLines();

    static bool IsSymbol( int aTok );

    static bool IsNumber( int aTok );

    void Expecting( int aTok ) const;

    void Expecting( const char* aTokenList ) const;

    void Unexpected( int aTok ) const;

    void Unexpected( const char* aToken ) const;

    void Duplicate( int aTok );

    void NeedLEFT();

    void NeedRIGHT();

    void NeedBAR();

    const char* GetTokenText( int aTok ) const;

    QString GetTokenString( int aTok ) const;

    static const char* Syntax( int aTok );

    const char* CurText() const
    {
        return curText.c_str();
    }

    const std::string& CurStr() const
    {
        return curText;
    }

    QString FromUTF8() const
    {
        return QString::fromUtf8( curText.c_str() );
    }

    int CurLineNumber() const
    {
        return reader->LineNumber();
    }

    const char* CurLine() const
    {
        return (const char*)(*reader);
    }

    const QString& CurSource() const
    {
        return reader->GetSource();
    }

    int CurOffset() const
    {
        return curOffset + 1;
    }

#ifndef SWIG

protected:
    void init();

    inline bool isSep( char cc );

    int readLine()
    {
        if( reader )
        {
            reader->ReadLine();

            unsigned len = reader->Length();

            start = reader->Line();

            next  = start;
            limit = next + len;

            return len;
        }
        return 0;
    }

    int findToken( const std::string& aToken ) const;

    bool isStringTerminator( char cc ) const
    {
        if( !space_in_quoted_tokens && cc == ' ' )
            return true;

        if( cc == stringDelimiter )
            return true;

        return false;
    }

    double parseDouble();

    double parseDouble( const char* aExpected )
    {
        NeedNUMBER( aExpected );
        return parseDouble();
    }

    template <typename T>
    inline double parseDouble( T aToken )
    {
        return parseDouble( GetTokenText( aToken ) );
    }

protected:
    bool                iOwnReaders;
    const char*         start;
    const char*         next;
    const char*         limit;
    char                dummy[1];

    typedef std::vector<LINE_READER*>  READER_STACK;

    READER_STACK        readerStack;

    LINE_READER*        reader;

    bool                specctraMode;
    bool                m_knowsBar;

    char                stringDelimiter;
    bool                space_in_quoted_tokens;

    bool                commentsAreTokens;

    int                 prevTok;
    int                 curOffset;

    int                 curTok;
    std::string         curText;

    const KEYWORD*      keywords;
    unsigned            keywordCount;
    const KEYWORD_MAP*  keywordsLookup;
#endif // SWIG
};

#endif  // DSNLEXER_H_
