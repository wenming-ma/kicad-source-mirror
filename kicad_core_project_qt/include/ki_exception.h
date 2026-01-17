// QT_TRANSFORMATION_COMPLETED

#ifndef KI_EXCEPTION_H_
#define KI_EXCEPTION_H_

#include <kicommon.h>
#include <QString>


/// macro which captures the "call site" values of __FILE_, __FUNCTION__ & __LINE__
#define THROW_IO_ERROR( msg )   throw IO_ERROR( msg, __FILE__, __FUNCTION__, __LINE__ )

class KICOMMON_API KI_PARAM_ERROR
{
public:
    KI_PARAM_ERROR( const QString& aMessage )
    {
        m_message = aMessage;
    }

    KI_PARAM_ERROR() {}

    const QString What() const
    {
        return m_message;
    }

    virtual ~KI_PARAM_ERROR() throw () {}

private:
    QString    m_message;
};


class KICOMMON_API IO_ERROR
{
public:
    IO_ERROR( const QString& aProblem, const char* aThrowersFile,
              const char* aThrowersFunction, int aThrowersLineNumber )
    {
        init( aProblem, aThrowersFile, aThrowersFunction, aThrowersLineNumber );
    }

    IO_ERROR() {}

    void init( const QString& aProblem, const char* aThrowersFile,
               const char* aThrowersFunction, int aThrowersLineNumber );

    virtual const QString Problem() const;
    virtual const QString Where() const;

    virtual const QString What() const;

    virtual ~IO_ERROR() throw () {}

protected:
    QString    problem;
    QString    where;
};


struct KICOMMON_API PARSE_ERROR : public IO_ERROR
{
    int         lineNumber;
    int         byteIndex;

    std::string inputLine;

    PARSE_ERROR( const QString& aProblem, const char* aThrowersFile,
                 const char* aThrowersFunction, int aThrowersLineNumber,
                 const QString& aSource, const char* aInputLine,
                 int aLineNumber, int aByteIndex ) :
        IO_ERROR()
    {
        init( aProblem, aThrowersFile, aThrowersFunction, aThrowersLineNumber,
              aSource, aInputLine, aLineNumber, aByteIndex );
    }

    void init( const QString& aProblem, const char* aThrowersFile,
               const char* aThrowersFunction, int aThrowersLineNumber,
               const QString& aSource, const char* aInputLine,
               int aLineNumber, int aByteIndex );

    ~PARSE_ERROR() throw () {}

    const QString ParseProblem() { return parseProblem; }

protected:
    PARSE_ERROR() :
        IO_ERROR(),
        lineNumber( 0 ),
        byteIndex( 0 )
    {}

protected:
    QString parseProblem;
};


#define THROW_PARSE_ERROR( aProblem, aSource, aInputLine, aLineNumber, aByteIndex )      \
    throw PARSE_ERROR( aProblem, __FILE__, __FUNCTION__, __LINE__, aSource, aInputLine,  \
                       aLineNumber, aByteIndex )


struct KICOMMON_API FUTURE_FORMAT_ERROR : public PARSE_ERROR
{
    QString requiredVersion;
    QString requiredGenerator;

    FUTURE_FORMAT_ERROR( const QString& aRequiredVersion,
                         const QString& aRequiredGenerator = QString() );
    FUTURE_FORMAT_ERROR( const PARSE_ERROR& aParseError, const QString& aRequiredVersion,
                         const QString& aRequiredGenerator = QString() );
    ~FUTURE_FORMAT_ERROR() throw () {}

    void init( const QString& aRequiredVersion,
               const QString& aRequiredGenerator = QString() );
};


#endif // KI_EXCEPTION_H_
