
#include "clipboard.h"

#include <QApplication>
#include <QClipboard>
#include <QMimeData>
#include <QPixmap>
#include <QImageReader>
#include <QUrl>


bool SaveClipboard( const std::string& aTextUTF8 )
{
    QClipboard* clipboard = QApplication::clipboard();

    if( clipboard )
    {
        // Store the UTF8 string as Unicode string in clipboard:
        QString text = QString::fromUtf8( aTextUTF8.c_str() );
        clipboard->setText( text );

        return true;
    }

    return false;
}


std::string GetClipboardUTF8()
{
    std::string result;

    QClipboard* clipboard = QApplication::clipboard();

    if( clipboard )
    {
        const QMimeData* mimeData = clipboard->mimeData();

        if( mimeData && mimeData->hasText() )
        {
            QString text = clipboard->text();

            // The clipboard is expected containing a Unicode string, so return it
            // as UTF8 string
            result = text.toUtf8().toStdString();
        }
    }

    return result;
}


std::unique_ptr<QPixmap> GetImageFromClipboard()
{
    std::unique_ptr<QPixmap> image;

    QClipboard* clipboard = QApplication::clipboard();

    // First try for an image
    if( clipboard )
    {
        const QMimeData* mimeData = clipboard->mimeData();

        if( mimeData && mimeData->hasImage() )
        {
            QPixmap pixmap = clipboard->pixmap();
            if( !pixmap.isNull() )
            {
                image = std::make_unique<QPixmap>( pixmap );
            }
        }
        else if( mimeData && mimeData->hasUrls() )
        {
            QList<QUrl> urls = mimeData->urls();
            if( urls.size() == 1 )
            {
                QString filePath = urls.first().toLocalFile();
                QPixmap pixmap( filePath );

                if( !pixmap.isNull() )
                {
                    image = std::make_unique<QPixmap>( pixmap );
                }
            }
        }
    }

    return image;
}
