// QT_TRANSFORMATION_COMPLETED

#ifndef RICHIO_H_
#define RICHIO_H_


#include <QVector>
#include <QString>
#include <QTextStream>
#include <core/utf8.h>
#include <cstdio>

#include <ki_exception.h>
#include <kicommon.h>

KICOMMON_API int
#if defined(__GNUG__)
    __attribute__ ((format (printf, 2, 3)))
#endif
    StrPrintf( std::string* aResult, const char* aFormat, ... );


KICOMMON_API std::string
#if defined(__GNUG__)
    __attribute__ ((format (printf, 1, 2)))
#endif
    StrPrintf( const char* format, ... );


KICOMMON_API QString SafeReadFile( const QString& aFilePath, const QString& aReadType );


#define LINE_READER_LINE_DEFAULT_MAX        1000000
#define LINE_READER_LINE_INITIAL_SIZE       5000

class KICOMMON_API LINE_READER
{
public:

    LINE_READER( unsigned aMaxLineLength = LINE_READER_LINE_DEFAULT_MAX );

    virtual ~LINE_READER();

    virtual char* ReadLine() = 0;

    virtual const QString& GetSource() const
    {
        return m_source;
    }

    char* Line() const
    {
        return m_line;
    }

    operator char* () const
    {
        return Line();
    }

    virtual unsigned LineNumber() const
    {
        return m_lineNum;
    }

    unsigned Length() const
    {
        return m_length;
    }

protected:
    void        expandCapacity( unsigned aNewsize );

    unsigned    m_length;
    unsigned    m_lineNum;

    char*       m_line;
    unsigned    m_capacity;

    unsigned    m_maxLineLength;

    QString     m_source;
};


class KICOMMON_API FILE_LINE_READER : public LINE_READER
{
public:
    FILE_LINE_READER( const QString& aFileName, unsigned aStartingLineNumber = 0,
                      unsigned aMaxLineLength = LINE_READER_LINE_DEFAULT_MAX );

    FILE_LINE_READER( FILE* aFile, const QString& aFileName, bool doOwn = true,
                      unsigned aStartingLineNumber = 0,
                      unsigned aMaxLineLength = LINE_READER_LINE_DEFAULT_MAX );

    ~FILE_LINE_READER();

    char* ReadLine() override;

    void Rewind()
    {
        rewind( m_fp );
        m_lineNum = 0;
    }

    long int FileLength();
    long int CurPos();

protected:
    bool    m_iOwn;
    FILE*   m_fp;
};


class KICOMMON_API STRING_LINE_READER : public LINE_READER
{
protected:
    std::string     m_lines;
    size_t          m_ndx;

public:

    STRING_LINE_READER( const std::string& aString, const QString& aSource );

    STRING_LINE_READER( const STRING_LINE_READER& aStartingPoint );

    char* ReadLine() override;
};


class KICOMMON_API INPUTSTREAM_LINE_READER : public LINE_READER
{
public:
    INPUTSTREAM_LINE_READER( QTextStream* aStream, const QString& aSource );

    char* ReadLine() override;

protected:
    QTextStream* m_stream;
};


#define OUTPUTFMTBUFZ    500

class KICOMMON_API OUTPUTFORMATTER
{
protected:
    OUTPUTFORMATTER( int aReserve = OUTPUTFMTBUFZ, char aQuoteChar = '"' ) :
            m_buffer( aReserve, '\0' )
    {
        quoteChar[0] = aQuoteChar;
        quoteChar[1] = '\0';
    }

    static const char* GetQuoteChar( const char* wrapee, const char* quote_char );

    virtual void write( const char* aOutBuf, int aCount ) = 0;

#if defined(__GNUG__)   // The GNU C++ compiler defines this

#define PRINTF_FUNC_N __attribute__( ( format( printf, 3, 4 ) ) )
#define PRINTF_FUNC __attribute__( ( format( printf, 2, 3 ) ) )
#else
#define PRINTF_FUNC_N     // nothing
#define PRINTF_FUNC       // nothing
#endif

public:
    virtual ~OUTPUTFORMATTER() {}

    int PRINTF_FUNC_N Print( int nestLevel, const char* fmt, ... );

    int PRINTF_FUNC Print( const char* fmt, ... );

    virtual const char* GetQuoteChar( const char* wrapee ) const;

    virtual std::string Quotes( const std::string& aWrapee ) const;

    std::string Quotew( const QString& aWrapee ) const;

    virtual bool Finish() { return true; }

private:
    QVector<char>   m_buffer;
    char            quoteChar[2];

    int sprint( const char* fmt, ... );
    int vprint( const char* fmt, va_list ap );

};


class KICOMMON_API STRING_FORMATTER : public OUTPUTFORMATTER
{
public:
    STRING_FORMATTER( int aReserve = OUTPUTFMTBUFZ, char aQuoteChar = '"' ) :
        OUTPUTFORMATTER( aReserve, aQuoteChar )
    {
    }

    void Clear()
    {
        m_mystring.clear();
    }

    void StripUseless();

    const std::string& GetString()
    {
        return m_mystring;
    }

protected:
    void write( const char* aOutBuf, int aCount ) override;

private:
    std::string m_mystring;
};


class KICOMMON_API FILE_OUTPUTFORMATTER : public OUTPUTFORMATTER
{
public:

    FILE_OUTPUTFORMATTER( const QString& aFileName, const char* aMode = "wt",
                          char aQuoteChar = '"' );

    ~FILE_OUTPUTFORMATTER();

protected:
    void write( const char* aOutBuf, int aCount ) override;

    FILE*       m_fp;
    QString     m_filename;
};


class KICOMMON_API PRETTIFIED_FILE_OUTPUTFORMATTER : public OUTPUTFORMATTER
{
public:
    PRETTIFIED_FILE_OUTPUTFORMATTER( const QString& aFileName, const char* aMode = "wt",
                                     char aQuoteChar = '"' );

    ~PRETTIFIED_FILE_OUTPUTFORMATTER();

    bool Finish() override;

protected:
    void write( const char* aOutBuf, int aCount ) override;

private:
    FILE* m_fp;
    std::string m_buf;
};


#endif // RICHIO_H_
