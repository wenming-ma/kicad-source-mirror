
#include <env_paths.h>
#include <project.h>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QStringList>

static bool normalizeAbsolutePaths( const QFileInfo& aPathA, const QFileInfo& aPathB,
                                    QString* aResultPath )
{
    if( !aPathA.isAbsolute() )
        return false;
    if( !aPathB.isAbsolute() )
        return false;

    if( aPathA.absolutePath() == aPathB.absolutePath() )
        return true;

    QStringList aPathParts = aPathA.absolutePath().split( '/', Qt::SkipEmptyParts );
    QStringList bPathParts = aPathB.absolutePath().split( '/', Qt::SkipEmptyParts );
    
    if( aPathParts.size() > bPathParts.size() )
        return false;

    QStringList aDirs = aPathParts;
    QStringList bDirs = bPathParts;

    size_t i = 0;

    while( i < aDirs.size() )
    {
        if( aDirs[i] != bDirs[i] )
            return false;

        i++;
    }

    if( aResultPath )
    {
        while( i < bDirs.size() )
        {
            *aResultPath += bDirs[i] + "/";
            i++;
        }
    }

    return true;
}


QString NormalizePath( const QFileInfo& aFilePath, const ENV_VAR_MAP* aEnvVars,
                        const QString& aProjectPath )
{
    QFileInfo envPath;
    QString   varName;
    QString   remainingPath;
    QString   normalizedFullPath;
    int       pathDepth = 0;

    if( aEnvVars )
    {
        for( const std::pair<const QString, ENV_VAR_ITEM>& entry : *aEnvVars )
        {
            // Don't bother normalizing paths that don't exist or the user cannot read.
            QDir dir( entry.second.GetValue() );
            if( !dir.exists() || !dir.isReadable() )
            {
                continue;
            }

            envPath.setFile( entry.second.GetValue() );

            QString tmp;

            if( normalizeAbsolutePaths( envPath, aFilePath, &tmp ) )
            {
                int newDepth = envPath.absolutePath().split( '/', Qt::SkipEmptyParts ).size();

                // Only use the variable if it removes more directories than the previous ones
                if( newDepth > pathDepth )
                {
                    pathDepth     = newDepth;
                    varName       = entry.first;
                    remainingPath = tmp;
                }

            }
        }
    }

    if( varName.isEmpty() && !aProjectPath.isEmpty()
        && QFileInfo( aProjectPath ).isAbsolute() && aFilePath.isAbsolute() )
    {
        envPath.setFile( aProjectPath );

        if( normalizeAbsolutePaths( envPath, aFilePath, &remainingPath ) )
            varName = PROJECT_VAR_NAME;
    }

    if( varName.isEmpty() )
    {
        normalizedFullPath = aFilePath.absoluteFilePath();
    }
    else
    {
        normalizedFullPath = QString( "${%1}/" ).arg( varName );

        if( !remainingPath.isEmpty() )
            normalizedFullPath += remainingPath;

        normalizedFullPath += aFilePath.fileName();
    }

    return normalizedFullPath;
}


QString NormalizePath( const QFileInfo& aFilePath, const ENV_VAR_MAP* aEnvVars,
                        const PROJECT* aProject )
{
    if( aProject )
        return NormalizePath( aFilePath, aEnvVars, aProject->GetProjectPath() );
    else
        return NormalizePath( aFilePath, aEnvVars, QString() );
}


// Create file path by appending path and file name. This approach allows the filename
// to contain a relative path, whereas QFileInfo::setFile() would replace the
// relative path
static QString createFilePath( const QString& aPath, const QString& aFileName )
{
    QString path( aPath );

    if( !path.endsWith( QDir::separator() ) )
        path.append( QDir::separator() );

    return path + aFileName;
}


QString ResolveFile( const QString& aFileName, const ENV_VAR_MAP* aEnvVars,
                      const PROJECT* aProject )
{
    QFileInfo full( aFileName );

    if( full.isAbsolute() )
        return full.absoluteFilePath();

    if( aProject )
    {
        QFileInfo fn( createFilePath( aProject->GetProjectPath(), aFileName ) );

        if( fn.exists() )
            return fn.absoluteFilePath();
    }

    if( aEnvVars )
    {
        for( const std::pair<const QString, ENV_VAR_ITEM>& entry : *aEnvVars )
        {
            QFileInfo fn( createFilePath( entry.second.GetValue(), aFileName ) );

            if( fn.exists() )
                return fn.absoluteFilePath();
        }
    }

    return QString();
}


bool PathIsInsideProject( const QString& aFileName, const PROJECT* aProject, QFileInfo* aSubPath )
{
    QFileInfo fn( aFileName );
    QFileInfo prj( aProject->GetProjectPath() );

    QStringList pdirs = prj.absolutePath().split( '/', Qt::SkipEmptyParts );
    QStringList fdirs = fn.absolutePath().split( '/', Qt::SkipEmptyParts );

    if( fdirs.size() < pdirs.size() )
        return false;

    for( size_t i = 0; i < pdirs.size(); i++ )
    {
        if( fdirs[i] != pdirs[i] )
            return false;
    }

    // Now we know that fn is inside prj
    if( aSubPath )
    {
        QString subPath;
        for( size_t i = pdirs.size(); i < fdirs.size(); i++ )
        {
            if( !subPath.isEmpty() )
                subPath += QDir::separator();
            subPath += fdirs[i];
        }
        aSubPath->setFile( subPath );
    }

    return true;
}
