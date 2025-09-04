#ifndef WXSTREAM_HELPER_H
#define WXSTREAM_HELPER_H

#include <QVector>
#include <QIODevice>
#include <QDebug>


static bool CopyStreamData( QIODevice& inputStream, QIODevice& outputStream,
                            qint64 size )
{
    constexpr size_t bufSize = 128 * 1024;
    QVector<char> buf( bufSize );

    qint64 copiedData = 0;
    qint64 readSize   = bufSize;

    for( ; ; )
    {
        if(size != -1 && copiedData + readSize > size )
            readSize = size - copiedData;

        qint64 actuallyRead = inputStream.read( buf.data(), readSize );

        if( actuallyRead == -1 )
            break;

        qint64 actuallyWritten = outputStream.write( buf.data(), actuallyRead );

        if( actuallyWritten != actuallyRead )
        {
            qDebug() << "Failed to output data";
            //return false;
        }

        if( size == -1 )
        {
            if( inputStream.atEnd() )
                break;
        }
        else
        {
            copiedData += actuallyRead;

            if( copiedData >= size )
                break;
        }
    }

    return true;
}


#endif // WXSTREAM_HELPER_H
