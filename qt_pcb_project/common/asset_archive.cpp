#include <QFile>
#include <QByteArray>
#include <QBuffer>

#include <asset_archive.h>


ASSET_ARCHIVE::ASSET_ARCHIVE( const QString& aFilePath, bool aLoadNow ) :
        m_filePath( aFilePath )
{
    if( aLoadNow )
        Load();
}


bool ASSET_ARCHIVE::Load()
{
    if( !m_fileInfoCache.empty() )
        return false;

    QFile zipFile( m_filePath );

    if( !zipFile.open( QIODevice::ReadOnly ) )
        return false;

    QByteArray compressedData = zipFile.readAll();
    zipFile.close();

    QByteArray uncompressedData = qUncompress( compressedData );
    
    if( uncompressedData.isEmpty() )
        return false;

    m_cache.resize( 2 * zipFile.size() );

    size_t offset = 0;
    
    QBuffer buffer( &uncompressedData );
    buffer.open( QIODevice::ReadOnly );
    
    while( !buffer.atEnd() )
    {
        QByteArray header = buffer.read( 512 );
        if( header.size() < 512 )
            break;
            
        QString filename = QString::fromLatin1( header.left( 100 ) ).trimmed();
        if( filename.isEmpty() )
            break;
            
        if( header.at( 156 ) == '5' )
        {
            continue;
        }
        
        QString sizeStr = QString::fromLatin1( header.mid( 124, 12 ) ).trimmed();
        bool ok;
        size_t length = sizeStr.toLongLong( &ok, 8 );
        if( !ok )
            break;
            
        FILE_INFO fi;
        fi.offset = offset;
        fi.length = length;

        if( offset + length > m_cache.size() )
            m_cache.resize( m_cache.size() * 2 );

        QByteArray fileData = buffer.read( length );
        std::memcpy( &m_cache[offset], fileData.constData(), fileData.size() );

        m_fileInfoCache[filename] = fi;

        offset += length;
        
        size_t padding = ( 512 - ( length % 512 ) ) % 512;
        if( padding > 0 )
            buffer.read( padding );
    }

    m_cache.resize( offset );

    return true;
}


long ASSET_ARCHIVE::GetFileContents( const QString& aFilePath, const unsigned char* aDest,
                                     size_t aMaxLen )
{
    return 0;
}


long ASSET_ARCHIVE::GetFilePointer( const QString& aFilePath, const unsigned char** aDest )
{
    if( aFilePath.isEmpty() )
        return -1;

    Q_ASSERT( aDest );

    if( !m_fileInfoCache.count( aFilePath ) )
        return -1;

    const FILE_INFO& fi = m_fileInfoCache.at( aFilePath );

    *aDest = &m_cache[fi.offset];

    return fi.length;
}