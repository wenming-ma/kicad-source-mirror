
#include <search_stack.h>
#include <string_utils.h>
#include <trace_helpers.h>
#include <QStringList>
#include <QDebug>
#include <QDir>
#include <QFileInfo>


#if defined(_WIN32)
 #define PATH_SEPS          ";\r\n"
#else
 #define PATH_SEPS          ":;\r\n"       // unix == linux | mac
#endif


int SEARCH_STACK::Split( QStringList* aResult, const QString& aPathString )
{
    QStringList tokens = aPathString.split(QRegExp(QString("[") + PATH_SEPS + QString("]")), Qt::SkipEmptyParts);

    for( const QString& path : tokens )
    {
        aResult->append( path );
    }

    return aResult->count();
}


// Convert aRelativePath to an absolute path based on aBaseDir
static QString base_dir( const QString& aRelativePath, const QString& aBaseDir )
{
    QFileInfo fn( aRelativePath );

    if( !fn.isAbsolute() && !aBaseDir.isEmpty() )
    {
        Q_ASSERT_X( QFileInfo( aBaseDir ).isAbsolute(), "base_dir", "Must pass absolute path in aBaseDir" );
        QDir baseDir( aBaseDir );
        return baseDir.relativeFilePath( aRelativePath );
    }

    return fn.absoluteFilePath();
}


QString SEARCH_STACK::FilenameWithRelativePathInSearchList(
        const QString& aFullFilename, const QString& aBaseDir )
{
    QFileInfo fn( aFullFilename );
    QString   filename = aFullFilename;

    unsigned   pathlen  = fn.path().length();   // path len, used to find the better (shortest)
                                                // subpath within defaults paths

    for( unsigned kk = 0; kk < GetCount(); kk++ )
    {
        QString baseDirPath = base_dir( (*this)[kk], aBaseDir );
        QDir baseDir( baseDirPath );
        QString relativePath = baseDir.relativeFilePath( aFullFilename );
        
        if( !relativePath.isEmpty() && relativePath != aFullFilename )
        {
            if( relativePath.startsWith( ".." ) )  // Path outside KiCad libs paths
                continue;

            QFileInfo relFn( relativePath );
            if( pathlen > relFn.path().length() )    // A better (shortest) subpath is found
            {
                filename = relativePath;
                pathlen  = relFn.path().length();
            }
        }
    }

    return filename;
}


void SEARCH_STACK::RemovePaths( const QString& aPaths )
{
    bool            isCS = true;  // Qt file operations are case sensitive by default on Unix
    QStringList     paths;

    Split( &paths, aPaths );

    for( int i = 0; i < paths.count(); ++i )
    {
        QString path = paths[i];

        int index = indexOf( path );
        if( index != -1 )
        {
            removeAt( index );
        }
    }
}


void SEARCH_STACK::AddPaths( const QString& aPaths, int aIndex )
{
    bool            isCS = true;  // Qt file operations are case sensitive by default on Unix
    QStringList     paths;

    Split( &paths, aPaths );

    // appending all of them, on large or negative aIndex
    if( unsigned( aIndex ) >= GetCount() )
    {
        for( int i = 0; i < paths.count(); ++i )
        {
            QString path = paths[i];

            QDir dir( path );
            if( dir.exists() && dir.isReadable()
                && indexOf( path ) == -1 )
            {
                append( path );
            }
        }
    }

    // inserting all of them:
    else
    {
        for( int i = 0; i < paths.count(); ++i )
        {
            QString path = paths[i];

            QDir dir( path );
            if( dir.exists() && dir.isReadable()
                && indexOf( path ) == -1 )
            {
                insert( aIndex, path );
                aIndex++;
            }
        }
    }
}


#if 1       // this function is too convoluted for words.

const QString SEARCH_STACK::LastVisitedPath( const QString& aSubPathToSearch )
{
    QString path;

    // Initialize default path to the main default lib path
    // this is the second path in list (the first is the project path).
    unsigned pcount = GetCount();

    if( pcount )
    {
        unsigned ipath = 0;

        if( (*this)[0] == QDir::currentPath() )
            ipath = 1;

        // First choice of path:
        if( ipath < pcount )
            path = (*this)[ipath];

        // Search a sub path matching this SEARCH_PATH
        if( !isEmpty() )
        {
            for( ; ipath < pcount; ipath++ )
            {
                if( (*this)[ipath].contains( aSubPathToSearch ) )
                {
                    path = (*this)[ipath];
                    break;
                }
            }
        }
    }

    if( path.isEmpty() )
        path = QDir::currentPath();

    return path;
}
#endif


#if defined( DEBUG )
void SEARCH_STACK::Show( const QString& aPrefix ) const
{
    qDebug() << QString("%1 SEARCH_STACK:").arg(aPrefix);

    for( unsigned i = 0; i < GetCount(); ++i )
    {
        qDebug() << QString("  [%1]:%2").arg(i, 2).arg((*this)[i]);
    }
}
#endif
