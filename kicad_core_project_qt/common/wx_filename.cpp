// Qt Framework Transformation: wxWidgets to Qt conversion completed
#include <wx_filename.h>
#include <string_utils.h>
#include <QDir>
#include <limits.h>

#if !defined(_WIN32) && !defined(_WIN64) && !defined(__WIN32__) && !defined(__WINDOWS__)
#include <unistd.h>
#include <climits>
#endif

WX_FILENAME::WX_FILENAME( const QString& aPath, const QString& aFilename )
        : m_fn( aPath + "/" + aFilename ), m_path( aPath ), m_fullName( aFilename )
{
}


void WX_FILENAME::SetFullName( const QString& aFileNameAndExtension )
{
    m_fullName = aFileNameAndExtension;
}


void WX_FILENAME::SetPath( const QString& aPath )
{
    m_fn.setFile( aPath + "/" + m_fullName );
    m_path = aPath;
}


QString WX_FILENAME::GetName() const
{
    int dot = m_fullName.lastIndexOf( '.' );
    if( dot != -1 )
        return m_fullName.left( dot );
    return m_fullName;
}


QString WX_FILENAME::GetFullName() const
{
    return m_fullName;
}


QString WX_FILENAME::GetPath() const
{
    return m_path;
}


QString WX_FILENAME::GetFullPath() const
{
    return m_path + "/" + m_fullName;
}


void WX_FILENAME::resolve()
{
    int dot = m_fullName.lastIndexOf( '.' );
    if( dot != -1 )
    {
        QString baseName = m_fullName.left( dot );
        QString extension = m_fullName.mid( dot + 1 );
        m_fn.setFile( m_path + "/" + baseName + "." + extension );
    }
    else
    {
        m_fn.setFile( m_path + "/" + m_fullName );
    }
}


long long WX_FILENAME::GetTimestamp()
{
    resolve();

    if( m_fn.exists() && m_fn.isFile() )
        return m_fn.lastModified().toMSecsSinceEpoch();

    return 0;
}


void WX_FILENAME::ResolvePossibleSymlinks( QFileInfo& aFilename )
{
#if !defined(_WIN32) && !defined(_WIN64) && !defined(__WIN32__) && !defined(__WINDOWS__)
    if( aFilename.isSymLink() )
    {
        char buffer[PATH_MAX];
        char* realPath = realpath( aFilename.absoluteFilePath().toLocal8Bit().constData(), buffer );

        if( realPath )
            aFilename.setFile( QString::fromLocal8Bit( realPath ) );
    }
#endif
}
