#include <QString>
#include <QObject>
#include <ki_exception.h>


const QString IO_ERROR::What() const
{
#ifdef DEBUG
     return QString( "IO_ERROR: " ) + Problem() + QString("\n\n" ) + Where();
#else
     return Problem();
#endif
}


const QString IO_ERROR::Where() const
{
    return where;
}


const QString IO_ERROR::Problem() const
{
    return problem;
}



void IO_ERROR::init( const QString& aProblem, const char* aThrowersFile,
                     const char* aThrowersFunction, int aThrowersLineNumber )
{
    problem = aProblem;

    // The throwers filename is a full filename, depending on KiCad source location.
    // a short filename will be printed (it is better for user, the full filename has no meaning).
    QString srcname = aThrowersFile;

    // No need for translations of source code file/line messages.
    where = QString( "from %1 : %2() line %3" )
                  .arg( srcname.mid( srcname.lastIndexOf( '/' ) + 1 ) )
                  .arg( QString( aThrowersFunction ) )
                  .arg( aThrowersLineNumber );
}


void PARSE_ERROR::init( const QString& aProblem, const char* aThrowersFile,
                        const char* aThrowersFunction, int aThrowersLineNumber,
                        const QString& aSource, const char* aInputLine, int aLineNumber,
                        int aByteIndex )
{
    parseProblem = aProblem;

    problem = QObject::tr( "%1 in '%2', line %3, offset %4." )
                    .arg( aProblem )
                    .arg( aSource )
                    .arg( aLineNumber )
                    .arg( aByteIndex );

    inputLine  = aInputLine;
    lineNumber = aLineNumber;
    byteIndex  = aByteIndex;

    // The throwers filename is a full filename, depending on KiCad source location.
    // a short filename will be printed (it is better for user, the full filename has no meaning).
    QString srcname = aThrowersFile;

    // No need for translations of source code file/line messages.
    where = QString( "from %1 : %2() line %3" )
                  .arg( srcname.mid( srcname.lastIndexOf( '/' ) + 1 ) )
                  .arg( QString( aThrowersFunction ) )
                  .arg( aThrowersLineNumber );
}


void FUTURE_FORMAT_ERROR::init( const QString& aRequiredVersion,
                                const QString& aRequiredGenerator )
{
    requiredVersion = aRequiredVersion;
    requiredGenerator = aRequiredGenerator;

    if( requiredGenerator.isEmpty() )
    {
        problem = QObject::tr( "KiCad was unable to open this file because it was created with a more "
                           "recent version than the one you are running.\n\n"
                           "To open it you will need to upgrade KiCad to a version dated %1 or "
                           "later." )
                        .arg( aRequiredVersion );
    }
    else
    {
        problem = QObject::tr( "KiCad was unable to open this file because it was created with a more "
                           "recent version than the one you are running.\n\n"
                           "To open it you will need to upgrade KiCad to version %1 or "
                           "later (file format dated %2 or later)." )
                        .arg( aRequiredGenerator )
                        .arg( aRequiredVersion );
    }
}


FUTURE_FORMAT_ERROR::FUTURE_FORMAT_ERROR( const QString& aRequiredVersion,
                                          const QString& aRequiredGenerator ) :
        PARSE_ERROR()
{
    init( aRequiredVersion, aRequiredGenerator );

    lineNumber = 0;
    byteIndex = 0;
}


FUTURE_FORMAT_ERROR::FUTURE_FORMAT_ERROR( const PARSE_ERROR& aParseError,
                                          const QString& aRequiredVersion,
                                          const QString& aRequiredGenerator ) :
        PARSE_ERROR()
{
    if( const FUTURE_FORMAT_ERROR* ffe = dynamic_cast<const FUTURE_FORMAT_ERROR*>( &aParseError ) )
    {
        requiredVersion = ffe->requiredVersion;
        requiredGenerator = ffe->requiredGenerator;
        problem = ffe->Problem();
    }
    else
    {
        init( aRequiredVersion, aRequiredGenerator );

        if( !aParseError.Problem().isEmpty() )
            problem += QString( "\n\n" ) + QObject::tr( "Full error text:" ) + QString( "\n" ) +
                       aParseError.Problem();
    }

    lineNumber = aParseError.lineNumber;
    byteIndex = aParseError.byteIndex;
    inputLine = aParseError.inputLine;
}