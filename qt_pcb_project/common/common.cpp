
#include <eda_base_frame.h>
#include <kiplatform/app.h>
#include <project.h>
#include <common.h>
#include <env_vars.h>
#include <reporter.h>
#include <macros.h>
#include <mutex>
#include <QSettings>
#include <QMessageBox>
#include <QStandardPaths>
#include <QUrl>
#include <QRegularExpression>
#include <QDir>
#include <QDateTime>
#include <QDirIterator>
#include <QFileInfo>

#ifdef _WIN32
#include <Windows.h>
#endif


enum Bracket
{
    Bracket_None,
    Bracket_Normal  = ')',
    Bracket_Curly   = '}',
#ifdef  __WINDOWS__
    Bracket_Windows = '%',    // yeah, Windows people are a bit strange ;-)
#endif
    Bracket_Max
};


QString ExpandTextVars( const QString& aSource, const PROJECT* aProject, int aFlags )
{
    std::function<bool( QString* )> projectResolver =
            [&]( QString* token ) -> bool
            {
                return aProject->TextVarResolver( token );
            };

    return ExpandTextVars( aSource, &projectResolver, aFlags );
}


QString ExpandTextVars( const QString& aSource,
                         const std::function<bool( QString* )>* aResolver, int aFlags )
{
    QString newbuf;
    size_t   sourceLen = aSource.length();

    newbuf.reserve( sourceLen );  // best guess (improves performance)

    for( size_t i = 0; i < sourceLen; ++i )
    {
        if( aSource[i] == '$' && i + 1 < sourceLen && aSource[i+1] == '{' )
        {
            QString token;

            for( i = i + 2; i < sourceLen; ++i )
            {
                if( aSource[i] == '}' )
                    break;
                else
                    token.append( aSource[i] );
            }

            if( token.isEmpty() )
                continue;

            if( ( aFlags & FOR_ERC_DRC ) == 0 && (   token.startsWith( "ERC_WARNING" )
                                                  || token.startsWith( "ERC_ERROR" )
                                                  || token.startsWith( "DRC_WARNING" )
                                                  || token.startsWith( "DRC_ERROR" ) ) )
            {
                // Only show user-defined warnings/errors during ERC/DRC
            }
            else if( aResolver && (*aResolver)( &token ) )
            {
                newbuf.append( token );
            }
            else
            {
                // Token not resolved: leave the reference unchanged
                newbuf.append( "${" + token + "}" );
            }
        }
        else
        {
            newbuf.append( aSource[i] );
        }
    }

    return newbuf;
}


QString GetGeneratedFieldDisplayName( const QString& aSource )
{
    std::function<bool( QString* )> tokenExtractor =
            [&]( QString* token ) -> bool
            {
                *token = *token;    // token value is the token name
                return true;
            };

    return ExpandTextVars( aSource, &tokenExtractor );
}


bool IsGeneratedField( const QString& aSource )
{
    static QRegularExpression expr( "^\\$\\{\\w*\\}$" );
    return expr.match( aSource ).hasMatch();
}


// Environment variable expansion function
QString KIExpandEnvVars( const QString& str, const PROJECT* aProject,
                            std::set<QString>* aSet = nullptr )
{
    // If the same string is inserted twice, we have a loop
    if( aSet )
    {
        if( auto [ _, result ] = aSet->insert( str ); !result )
            return str;
    }

    size_t strlen = str.length();

    QString strResult;
    strResult.reserve( strlen );  // best guess (improves performance)

    auto getVersionedEnvVar =
            []( const QString& aMatch, QString& aResult ) -> bool
            {
                for ( const QString& var : ENV_VAR::GetPredefinedEnvVars() )
                {
                    if( var.contains( aMatch ) )
                    {
                        const auto value = ENV_VAR::GetEnvVar<QString>( var );

                        if( !value )
                            continue;

                        aResult += *value;
                        return true;
                    }
                }

                return false;
            };

    for( size_t n = 0; n < strlen; n++ )
    {
        QChar str_n = str[n];

        switch( str_n.unicode() )
        {
#ifdef __WINDOWS__
        case '%':
#endif // __WINDOWS__
        case '$':
        {
            Bracket bracket;
#ifdef __WINDOWS__
            if( str_n == '%' )
            {
                bracket = Bracket_Windows;
            }
            else
#endif // __WINDOWS__
            if( n == strlen - 1 )
            {
                bracket = Bracket_None;
            }
            else
            {
                switch( str[n + 1].unicode() )
                {
                case '(':
                    bracket = Bracket_Normal;
                    str_n = str[++n];                   // skip the bracket
                    break;

                case '{':
                    bracket = Bracket_Curly;
                    str_n = str[++n];                   // skip the bracket
                    break;

                default:
                    bracket = Bracket_None;
                }
            }

            size_t m = n + 1;

            if( m >= strlen )
                break;

            QChar str_m = str[m];

            while( str_m.isLetterOrNumber() || str_m == '_' || str_m == ':' )
            {
                if( ++m == strlen )
                {
                    str_m = QChar(0);
                    break;
                }

                str_m = str[m];
            }

            QString strVarName = str.mid( n + 1, m - n - 1 );

            bool expanded = false;
            QString tmp = strVarName;

            if( aProject && aProject->TextVarResolver( &tmp ) )
            {
                strResult += tmp;
                expanded = true;
            }
            else if( !(tmp = qgetenv( strVarName.toLatin1() )).isEmpty() )
            {
                strResult += tmp;
                expanded = true;
            }
            // Replace unmatched older variables with current locations
            // If the user has the older location defined, that will be matched
            // first above.  But if they do not, this will ensure that their board still
            // displays correctly
            else if( strVarName.contains( "KISYS3DMOD")
                   || strVarName.contains( "KICAD*_3DMODEL_DIR" ) )
            {
                if( getVersionedEnvVar( "KICAD*_3DMODEL_DIR", strResult ) )
                    expanded = true;
            }
            else if( strVarName.contains( "KICAD*_SYMBOL_DIR" ) )
            {
                if( getVersionedEnvVar( "KICAD*_SYMBOL_DIR", strResult ) )
                    expanded = true;
            }
            else if( strVarName.contains( "KICAD*_FOOTPRINT_DIR" ) )
            {
                if( getVersionedEnvVar( "KICAD*_FOOTPRINT_DIR", strResult ) )
                    expanded = true;
            }
            else if( strVarName.contains( "KICAD*_3RD_PARTY" ) )
            {
                if( getVersionedEnvVar( "KICAD*_3RD_PARTY", strResult ) )
                    expanded = true;
            }
            else
            {
                // variable doesn't exist => don't change anything
#ifdef  __WINDOWS__
                if ( bracket != Bracket_Windows )
#endif
                if ( bracket != Bracket_None )
                    strResult += str[n - 1];

                strResult += str_n;
                strResult += strVarName;
            }

            // check the closing bracket
            if( bracket != Bracket_None )
            {
                QChar expectedBracket = (bracket == Bracket_Normal) ? QChar(')') :
                                       (bracket == Bracket_Curly) ? QChar('}') :
#ifdef __WINDOWS__
                                       (bracket == Bracket_Windows) ? QChar('%') :
#endif
                                       QChar();
                if( m == strlen || str_m != expectedBracket )
                {
                    // under MSW it's common to have '%' characters in the registry
                    // and it's annoying to have warnings about them each time, so
                    // ignore them silently if they are not used for env vars
                    //
                    // under Unix, OTOH, this warning could be useful for the user to
                    // understand why isn't the variable expanded as intended
#ifndef __WINDOWS__
                    char bracketChar = (bracket == Bracket_Normal) ? ')' :
                                      (bracket == Bracket_Curly) ? '}' :
#ifdef __WINDOWS__
                                      (bracket == Bracket_Windows) ? '%' :
#endif
                                      '?';
                    qWarning( "Environment variables expansion failed: missing '%c' at position %u in '%s'.",
                                  bracketChar, (unsigned int) (m + 1), str.toLatin1().data() );
#endif // __WINDOWS__
                }
                else
                {
                    // skip closing bracket unless the variables wasn't expanded
                    if( !expanded )
                    {
                        QChar bracketChar = (bracket == Bracket_Normal) ? QChar(')') :
                                           (bracket == Bracket_Curly) ? QChar('}') :
#ifdef __WINDOWS__
                                           (bracket == Bracket_Windows) ? QChar('%') :
#endif
                                           QChar();
                        strResult += bracketChar;
                    }

                    m++;
                }
            }

            n = m - 1;  // skip variable name
            str_n = str[n];
        }
            break;

        case '\\':
            // backslash can be used to suppress special meaning of % and $
            if( n < strlen - 1 && (str[n + 1] == '%' || str[n + 1] == '$' ) )
            {
                str_n = str[++n];
                strResult += str_n;

                break;
            }

            KI_FALLTHROUGH;

        default:
            strResult += str_n;
        }
    }

    std::set<QString> loop_check;
    auto first_pos = strResult.indexOf( QRegularExpression( "[{(%]" ) );
    auto last_pos = strResult.lastIndexOf( QRegularExpression( "[})%]" ) );

    if( first_pos != -1 && last_pos != -1 && first_pos != last_pos )
        strResult = KIExpandEnvVars( strResult, aProject, aSet ? aSet : &loop_check );

    return strResult;
}


const QString ExpandEnvVarSubstitutions( const QString& aString, const PROJECT* aProject )
{
    // qgetenv is thread-safe, but maintain lock for consistency
    static std::mutex getenv_mutex;

    std::lock_guard<std::mutex> lock( getenv_mutex );

    // We reserve the right to do this another way, by providing our own member function.
    return KIExpandEnvVars( aString, aProject );
}


const QString ResolveUriByEnvVars( const QString& aUri, const PROJECT* aProject )
{
    QString uri = ExpandTextVars( aUri, aProject );

    return ExpandEnvVarSubstitutions( uri, aProject );
}


bool EnsureFileDirectoryExists( QFileInfo*      aTargetFullFileName,
                                const QString&  aBaseFilename,
                                REPORTER*       aReporter )
{
    QString msg;
    QString baseFilePath = QFileInfo( aBaseFilename ).absolutePath();

    // make aTargetFullFileName path absolute if it's not already
    if( !aTargetFullFileName->isAbsolute() )
    {
        if( aReporter )
        {
            msg = QString( "Cannot make path '%1' absolute with respect to '%2'." )
                        .arg( aTargetFullFileName->absolutePath() )
                        .arg( baseFilePath );
            aReporter->Report( msg, RPT_SEVERITY_ERROR );
        }

        return false;
    }

    // Ensure the path of aTargetFullFileName exists, and create it if needed:
    QString outputPath( aTargetFullFileName->absolutePath() );

    if( !QDir( outputPath ).exists() )
    {
        // Make every directory provided when the provided path doesn't exist
        if( QDir().mkpath( outputPath ) )
        {
            if( aReporter )
            {
                msg = QString( "Output directory '%1' created." ).arg( outputPath );
                aReporter->Report( msg, RPT_SEVERITY_INFO );
                return true;
            }
        }
        else
        {
            if( aReporter )
            {
                msg = QString( "Cannot create output directory '%1'." ).arg( outputPath );
                aReporter->Report( msg, RPT_SEVERITY_ERROR );
            }

            return false;
        }
    }

    return true;
}


QString EnsureFileExtension( const QString& aFilename, const QString& aExtension )
{
    QString newFilename( aFilename );

    // It's annoying to throw up nag dialogs when the extension isn't right.  Just fix it,
    // but be careful not to destroy existing after-dot-text that isn't actually a bad
    // extension, such as "Schematic_1.1".
    QString currentExt = newFilename.section( '.', -1 ).toLower();
    if( currentExt != aExtension )
    {
        if( !newFilename.endsWith( '.' ) )
            newFilename.append( '.' );

        newFilename.append( aExtension );
    }

    return newFilename;
}


bool matchWild( const char* pat, const char* text, bool dot_special )
{
    if( !*text )
    {
        // Match if both are empty.
        return !*pat;
    }

    const char *m = pat,
    *n = text,
    *ma = nullptr,
    *na = nullptr;
    int just = 0,
    acount = 0,
    count = 0;

    if( dot_special && (*n == '.') )
    {
        // Never match so that hidden Unix files are never found.
        return false;
    }

    for( ;; )
    {
        if( *m == '*' )
        {
            ma = ++m;
            na = n;
            just = 1;
            acount = count;
        }
        else if( *m == '?' )
        {
            m++;

            if( !*n++ )
                return false;
        }
        else
        {
            if( *m == '\\' )
            {
                m++;

                // Quoting "nothing" is a bad thing
                if( !*m )
                    return false;
            }

            if( !*m )
            {
                // If we are out of both strings or we just
                // saw a wildcard, then we can say we have a
                // match
                if( !*n )
                    return true;

                if( just )
                    return true;

                just = 0;
                goto not_matched;
            }

            // We could check for *n == NULL at this point, but
            // since it's more common to have a character there,
            // check to see if they match first (m and n) and
            // then if they don't match, THEN we can check for
            // the NULL of n
            just = 0;

            if( *m == *n )
            {
                m++;
                count++;
                n++;
            }
            else
            {
                not_matched:

                // If there are no more characters in the
                // string, but we still need to find another
                // character (*m != NULL), then it will be
                // impossible to match it
                if( !*n )
                    return false;

                if( ma )
                {
                    m = ma;
                    n = ++na;
                    count = acount;
                }
                else
                    return false;
            }
        }
    }
}


#if defined( __WIN32__ )

// Convert between QDateTime and FILETIME which is a 64-bit value representing
// the number of 100-nanosecond intervals since January 1, 1601 UTC.
//
// This is the offset between FILETIME epoch and the Unix/Qt Epoch.
static qint64 EPOCH_OFFSET_IN_MSEC = 11644473600000LL;


static void ConvertFileTimeToQt( QDateTime* dt, const FILETIME& ft )
{
    qint64 t = ((qint64)ft.dwHighDateTime << 32) | ft.dwLowDateTime;
    t /= 10000; // Convert hundreds of nanoseconds to milliseconds.
    t -= EPOCH_OFFSET_IN_MSEC;

    *dt = QDateTime::fromMSecsSinceEpoch( t );
}

#endif // __WIN32__


long long TimestampDir( const QString& aDirPath, const QString& aFilespec )
{
    long long timestamp = 0;

#if defined( __WIN32__ )
    // Win32 version.
    // Save time by not searching for each path twice
    std::wstring filespec( aDirPath.toStdWString() );
    filespec += L'\\';
    filespec += aFilespec.toStdWString();

    WIN32_FIND_DATA findData;
    QDateTime       lastModDate;

    HANDLE fileHandle = ::FindFirstFile( filespec.data(), &findData );

    if( fileHandle != INVALID_HANDLE_VALUE )
    {
        do
        {
            ConvertFileTimeToQt( &lastModDate, findData.ftLastWriteTime );
            timestamp += lastModDate.toMSecsSinceEpoch();

            // Get the file size (partial) as well to check for sneaky changes.
            timestamp += findData.nFileSizeLow;
        }
        while ( FindNextFile( fileHandle, &findData ) != 0 );
    }

    FindClose( fileHandle );
#else
    // Cross-platform Qt version using QDirIterator.
    QDir directory( aDirPath );
    
    if( directory.exists() )
    {
        QDirIterator dirIterator( aDirPath, QStringList() << aFilespec, QDir::Files | QDir::NoDotAndDotDot );
        
        while( dirIterator.hasNext() )
        {
            QString filePath = dirIterator.next();
            QFileInfo fileInfo( filePath );
            
            // Handle symbolic links by resolving to the target file
            if( fileInfo.isSymLink() )
            {
                QString targetPath = fileInfo.symLinkTarget();
                if( !targetPath.isEmpty() )
                {
                    QFileInfo targetInfo( targetPath );
                    if( targetInfo.exists() && targetInfo.isFile() )
                    {
                        fileInfo = targetInfo;
                    }
                    // If we can't resolve the symlink, use the original fileInfo
                }
            }
            
            // Only process regular files (Qt handles this automatically with QDir::Files)
            if( fileInfo.isFile() )
            {
                timestamp += fileInfo.lastModified().toMSecsSinceEpoch();
                
                // Get the file size as well to check for sneaky changes.
                timestamp += fileInfo.size();
            }
            else
            {
                // if we couldn't get file info, use the name hash as fallback
                timestamp += (signed) std::hash<std::string>{}( fileInfo.fileName().toStdString() );
            }
        }
    }
#endif

    return timestamp;
}


bool WarnUserIfOperatingSystemUnsupported()
{
    if( !KIPLATFORM::APP::IsOperatingSystemUnsupported() )
        return false;

    QMessageBox::warning( nullptr, "Unsupported Operating System",
                         "This operating system is not supported by KiCad and its dependencies.\n\n"
                         "Any issues with KiCad on this system cannot be reported to the official bugtracker." );

    return true;
}
