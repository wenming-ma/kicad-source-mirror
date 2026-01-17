
#include <kiplatform/io.h>

#include <QString>
#include <QFileInfo>
#include <QDir>

#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

FILE* KIPLATFORM::IO::SeqFOpen( const QString& aPath, const QString& aMode )
{
    FILE* fp = fopen( aPath.toStdString().c_str(), aMode.toStdString().c_str() );

    if( fp )
    {
        if( posix_fadvise( fileno( fp ), 0, 0, POSIX_FADV_SEQUENTIAL ) != 0 )
        {
            fclose( fp );
            fp = nullptr;
        }
    }

    return fp;
}

bool KIPLATFORM::IO::DuplicatePermissions( const QString &aSrc, const QString &aDest )
{
    struct stat sourceStat;
    if( stat( aSrc.toStdString().c_str(), &sourceStat ) == 0 )
    {
        mode_t permissions = sourceStat.st_mode & ( S_IRWXU | S_IRWXG | S_IRWXO );
        if( chmod( aDest.toStdString().c_str(), permissions ) == 0 )
        {
            return true;
        }
        else
        {
            // Handle error
            return false;
        }
    }
    else
    {
        // Handle error
        return false;
    }
}

bool KIPLATFORM::IO::IsFileHidden( const QString& aFileName )
{
    QFileInfo fn( aFileName );

    return fn.baseName().startsWith( "." );
}


void KIPLATFORM::IO::LongPathAdjustment( QFileInfo& aFilename )
{
    // no-op
}