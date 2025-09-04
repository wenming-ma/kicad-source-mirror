
#include "bitmap_base.h"

#include <gr_basic.h>
#include <math/util.h>    // for KiROUND
#include <memory>         // for make_unique, unique_ptr
#include <plotters/plotter.h>
#include <richio.h>
#include <QImage>
#include <QPixmap>
#include <QBuffer>
#include <QIODevice>
#include <QString>
#include <QFile>
#include <QByteArray>



BITMAP_BASE::BITMAP_BASE( const VECTOR2I& pos )
{
    m_scale  = 1.0;                     // 1.0 = original bitmap size
    m_imageType = QImage::Format_Invalid;
    m_bitmap = nullptr;
    m_image  = nullptr;
    m_originalImage = nullptr;
    m_ppi    = 300;                     // the bitmap definition. the default is 300PPI
    m_pixelSizeIu = 254000.0 / m_ppi;   // a pixel size value OK for bitmaps using 300 PPI
                                        // for Eeschema which uses currently 254000PPI
    m_isMirroredX = false;
    m_isMirroredY = false;
    m_rotation   = ANGLE_0;
}


BITMAP_BASE::BITMAP_BASE( const BITMAP_BASE& aSchBitmap )
{
    m_scale = aSchBitmap.m_scale;
    m_ppi   = aSchBitmap.m_ppi;
    m_pixelSizeIu = aSchBitmap.m_pixelSizeIu;
    m_isMirroredX = aSchBitmap.m_isMirroredX;
    m_isMirroredY = aSchBitmap.m_isMirroredY;
    m_rotation = aSchBitmap.m_rotation;
    m_imageType = aSchBitmap.m_imageType;

    m_image = nullptr;
    m_bitmap = nullptr;
    m_originalImage = nullptr;

    if( aSchBitmap.m_image )
    {
        m_image   = new QImage( *aSchBitmap.m_image );
        m_bitmap  = new QPixmap( QPixmap::fromImage( *m_image ) );
        m_originalImage = new QImage( *aSchBitmap.m_originalImage );
        m_imageType = aSchBitmap.m_imageType;
        m_imageData = aSchBitmap.m_imageData;
        m_imageId = aSchBitmap.m_imageId;
    }
}


void BITMAP_BASE::rebuildBitmap( bool aResetID )
{
    if( m_bitmap )
        delete m_bitmap;

    m_bitmap  = new QPixmap( QPixmap::fromImage( *m_image ) );

    if( aResetID )
        m_imageId = KIID();

}


void BITMAP_BASE::updatePPI()
{
    // Todo: eventually we need to support dpi / scaling in both dimensions
    int dpiX = m_originalImage->dotsPerMeterX() / 39.37; // Convert to DPI

    if( dpiX > 1 )
    {
        m_ppi = dpiX;
    }
}


void BITMAP_BASE::ImportData( BITMAP_BASE& aItem )
{
    *m_image = *aItem.m_image;
    *m_bitmap = *aItem.m_bitmap;
    *m_originalImage = *aItem.m_originalImage;
    m_imageId = aItem.m_imageId;
    m_scale = aItem.m_scale;
    m_ppi = aItem.m_ppi;
    m_pixelSizeIu = aItem.m_pixelSizeIu;
    m_isMirroredX = aItem.m_isMirroredX;
    m_isMirroredY = aItem.m_isMirroredY;
    m_rotation = aItem.m_rotation;
    m_imageType = aItem.m_imageType;
    m_imageData = aItem.m_imageData;
}


bool BITMAP_BASE::ReadImageFile( QIODevice& aInStream )
{
    // Store the original image data in m_imageData
    m_imageData = aInStream.readAll();

    std::unique_ptr<QImage> new_image = std::make_unique<QImage>();

    // Load the image from the data into new_image
    if( !new_image->loadFromData( m_imageData ) )
        return false;

    return SetImage( *new_image );
}


bool BITMAP_BASE::ReadImageFile( QByteArray& aBuf )
{
    // Store the original image data in m_imageData
    m_imageData = aBuf;

    std::unique_ptr<QImage> new_image = std::make_unique<QImage>();

    // Load the image from the buffer into new_image
    if( !new_image->loadFromData( m_imageData ) )
        return false;

    return SetImage( *new_image );
}


bool BITMAP_BASE::ReadImageFile(const QString& aFullFilename)
{
    QFile file_stream(aFullFilename);

    // Check if the file could be opened successfully
    if (!file_stream.open(QIODevice::ReadOnly))
        return false;

    return ReadImageFile(file_stream);
}


bool BITMAP_BASE::SetImage( const QImage& aImage )
{
    if( aImage.isNull() || aImage.width() == 0 || aImage.height() == 0 )
        return false;

    delete m_image;
    m_image = new QImage( aImage );

    // Create a new QImage object from m_image
    delete m_originalImage;
    m_originalImage = new QImage( *m_image );

    rebuildBitmap();
    updatePPI();

    return true;
}


bool BITMAP_BASE::SaveImageData( QIODevice& aOutStream ) const
{
    if( m_imageData.isEmpty() )
    {
        // If m_imageData is empty, use QImage::save() method to write m_image contents to
        // the stream.
        const char* format = (m_imageType == QImage::Format_ARGB32) ? "JPEG" : "PNG";

        if( !m_image->save( &aOutStream, format ) )
        {
            return false;
        }
    }
    else
    {
        // Write the contents of m_imageData to the stream.
        aOutStream.write( m_imageData );
    }

    return true;
}


bool BITMAP_BASE::LoadLegacyData( LINE_READER& aLine, QString& aErrorMsg )
{
    QByteArray stream;
    char* line;

    while( true )
    {
        if( !aLine.ReadLine() )
        {
            aErrorMsg = "Unexpected end of data";
            return false;
        }

        line = aLine.Line();

        if( strncasecmp( line, "EndData", 4 ) == 0 )
        {
            // all the PNG date is read.
            // We expect here m_image and m_bitmap are void
            m_image = new QImage();
            m_image->loadFromData( stream );
            m_bitmap = new QPixmap( QPixmap::fromImage( *m_image ) );
            m_originalImage = new QImage( *m_image );
            updateImageDataBuffer();
            break;
        }

        // Read PNG data, stored in hexadecimal,
        // each byte = 2 hexadecimal digits and a space between 2 bytes
        // and put it in memory stream buffer
        int len = strlen( line );

        for( ; len > 0; len -= 3, line += 3 )
        {
            int value = 0;

            if( sscanf( line, "%X", &value ) == 1 )
                stream.append( (char) value );
            else
                break;
        }
    }

    return true;
}


const BOX2I BITMAP_BASE::GetBoundingBox() const
{
    BOX2I    bbox;
    VECTOR2I size = GetSize();

    bbox.Inflate( size.x / 2, size.y / 2 );

    return bbox;
}


void BITMAP_BASE::DrawBitmap( QPainter* aDC, const VECTOR2I& aPos,
                              const KIGFX::COLOR4D& aBackgroundColor ) const
{
    if( m_bitmap == nullptr )
        return;

    VECTOR2I pos = aPos;
    VECTOR2I size = GetSize();

    // This fixes a bug in OSX that should be fixed in the 3.0.3 version or later.
    if( ( size.x == 0 ) || ( size.y == 0 ) )
        return;

    // To draw the bitmap, pos is the upper left corner position
    pos.x -= size.x / 2;
    pos.y -= size.y / 2;

    QTransform init_matrix = aDC->transform();
    double scale = init_matrix.m11(); // Get current scale from transform
    
    bool useTransform = true; // QPainter always supports transforms

    QPoint clipAreaPos;

    QTransform matrix = aDC->transform();
    matrix.translate( pos.x, pos.y );
    matrix.scale( GetScalingFactor(), GetScalingFactor() );
    aDC->setTransform( matrix );

    clipAreaPos.setX( pos.x );
    clipAreaPos.setY( pos.y );

    pos.x = pos.y = 0;

#ifdef USE_CLIP_AREA
    aDC->setClipping( false );
    aDC->setClipRect( QRect( clipAreaPos, QSize( size.x, size.y ) ) );
#endif

    if( aBackgroundColor != COLOR4D::UNSPECIFIED && m_bitmap->hasAlpha() )
    {
        // Most printers don't support transparent images properly,
        // so blend the image with background color.

        int w = m_bitmap->width();
        int h = m_bitmap->height();

        QImage  image( w, h, QImage::Format_ARGB32 );
        QColor bgColor = aBackgroundColor.ToColor();

        image.fill( bgColor );
        
        QPainter painter( &image );
        painter.setCompositionMode( QPainter::CompositionMode_SourceOver );
        painter.drawPixmap( 0, 0, *m_bitmap );
        painter.end();

        if( GetGRForceBlackPenState() )
        {
            // Convert to grayscale
            for( int y = 0; y < h; ++y )
            {
                for( int x = 0; x < w; ++x )
                {
                    QRgb pixel = image.pixel( x, y );
                    int gray = qGray( pixel );
                    image.setPixel( x, y, qRgba( gray, gray, gray, qAlpha( pixel ) ) );
                }
            }
        }

        aDC->drawImage( pos.x, pos.y, image );
    }
    else if( GetGRForceBlackPenState() )
    {
        QImage result = m_bitmap->toImage();
        // Convert to grayscale
        for( int y = 0; y < result.height(); ++y )
        {
            for( int x = 0; x < result.width(); ++x )
            {
                QRgb pixel = result.pixel( x, y );
                int gray = qGray( pixel );
                result.setPixel( x, y, qRgba( gray, gray, gray, qAlpha( pixel ) ) );
            }
        }
        aDC->drawImage( pos.x, pos.y, result );
    }
    else
    {
        aDC->drawPixmap( pos.x, pos.y, *m_bitmap );
    }

    aDC->setTransform( init_matrix );

#ifdef USE_CLIP_AREA
    aDC->setClipping( false );
#endif
}


VECTOR2I BITMAP_BASE::GetSize() const
{
    VECTOR2I size;

    if( m_bitmap )
    {
        size.x = m_bitmap->width();
        size.y = m_bitmap->height();

        size.x = KiROUND( size.x * GetScalingFactor() );
        size.y = KiROUND( size.y * GetScalingFactor() );
    }

    return size;
}


void BITMAP_BASE::Mirror( FLIP_DIRECTION aFlipDirection )
{
    if( m_image )
    {
        // QImage::mirrored() preserves DPI information
        int resX = m_image->dotsPerMeterX();
        int resY = m_image->dotsPerMeterY();

        *m_image = m_image->mirrored( aFlipDirection == FLIP_DIRECTION::LEFT_RIGHT,
                                      aFlipDirection == FLIP_DIRECTION::TOP_BOTTOM );

        m_image->setDotsPerMeterX( resX );
        m_image->setDotsPerMeterY( resY );

        if( aFlipDirection == FLIP_DIRECTION::TOP_BOTTOM )
            m_isMirroredY = !m_isMirroredY;
        else
            m_isMirroredX = !m_isMirroredX;

        rebuildBitmap( false );
        updateImageDataBuffer();
    }
}


void BITMAP_BASE::Rotate( bool aRotateCCW )
{
    if( m_image )
    {
        // QImage transformations preserve DPI information
        int resX = m_image->dotsPerMeterX();
        int resY = m_image->dotsPerMeterY();

        QTransform transform;
        transform.rotate( aRotateCCW ? 90 : -90 );
        *m_image = m_image->transformed( transform );

        m_image->setDotsPerMeterX( resX );
        m_image->setDotsPerMeterY( resY );

        m_rotation += ( aRotateCCW ? ANGLE_90 : -ANGLE_90 );
        rebuildBitmap( false );
        updateImageDataBuffer();
    }
}


void BITMAP_BASE::ConvertToGreyscale()
{
    if( m_image )
    {
        // Convert to grayscale
        for( int y = 0; y < m_image->height(); ++y )
        {
            for( int x = 0; x < m_image->width(); ++x )
            {
                QRgb pixel = m_image->pixel( x, y );
                int gray = qGray( pixel );
                m_image->setPixel( x, y, qRgba( gray, gray, gray, qAlpha( pixel ) ) );
            }
        }
        
        // Convert original image to grayscale
        for( int y = 0; y < m_originalImage->height(); ++y )
        {
            for( int x = 0; x < m_originalImage->width(); ++x )
            {
                QRgb pixel = m_originalImage->pixel( x, y );
                int gray = qGray( pixel );
                m_originalImage->setPixel( x, y, qRgba( gray, gray, gray, qAlpha( pixel ) ) );
            }
        }
        
        rebuildBitmap();
        updateImageDataBuffer();
    }
}


void BITMAP_BASE::PlotImage( PLOTTER*       aPlotter, const VECTOR2I& aPos,
                             const COLOR4D& aDefaultColor,
                             int            aDefaultPensize ) const
{
    if( m_image == nullptr )
        return;

    // These 2 lines are useful only for plotters that cannot plot a bitmap
    // and plot a rectangle instead of.
    aPlotter->SetColor( aDefaultColor );
    aPlotter->SetCurrentLineWidth( aDefaultPensize );
    aPlotter->PlotImage( *m_image, aPos, GetScalingFactor() );
}


void BITMAP_BASE::updateImageDataBuffer()
{
    if( m_image )
    {
        QBuffer stream;
        stream.open( QIODevice::WriteOnly );
        const char* format = (m_imageType == QImage::Format_ARGB32) ? "JPEG" : "PNG";

        if( !m_image->save( &stream, format ) )
            return;

        m_imageData = stream.data();
    }
}
