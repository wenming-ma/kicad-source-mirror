
#pragma once

#include <QImage>
#include <QPixmap>
#include <QByteArray>
#include <QString>

#include <core/mirror.h>
#include <kiid.h>
#include <math/box2.h>
#include <gal/color4d.h>

class LINE_READER;
class PLOTTER;
class QPainter;
class QIODevice;


class BITMAP_BASE
{
public:
    BITMAP_BASE( const VECTOR2I& pos = VECTOR2I( 0, 0 ) );

    BITMAP_BASE( const BITMAP_BASE& aSchBitmap );

    ~BITMAP_BASE()
    {
        delete m_bitmap;
        delete m_image;
        delete m_originalImage;
    }

    double GetPixelSizeIu() const { return m_pixelSizeIu; }
    void SetPixelSizeIu( double aPixSize ) { m_pixelSizeIu = aPixSize; }

    QImage* GetImageData() { return m_image; }
    const QImage* GetImageData() const { return m_image; }

    const QImage* GetOriginalImageData() const { return m_originalImage; }

    double GetScale() const { return m_scale; }
    void SetScale( double aScale ) { m_scale = aScale; }

    KIID GetImageID() const { return m_imageId; }

    void ImportData( BITMAP_BASE& aItem );

    double GetScalingFactor() const
    {
        return m_pixelSizeIu * m_scale;
    }

    VECTOR2I GetSize() const;

    VECTOR2I GetSizePixels() const
    {
        if( m_image )
            return VECTOR2I( m_image->width(), m_image->height() );
        else
            return VECTOR2I( 0, 0 );
    }

    int GetPPI() const
    {
        return m_ppi;
    }

    const BOX2I GetBoundingBox() const;

    void DrawBitmap( QPainter* aDC, const VECTOR2I& aPos,
                     const KIGFX::COLOR4D& aBackgroundColor = KIGFX::COLOR4D::UNSPECIFIED ) const;

    bool ReadImageFile( const QString& aFullFilename );

    bool ReadImageFile( QIODevice& aInStream );

    bool ReadImageFile( QByteArray& aBuf );

    bool SetImage( const QImage& aImage );

    bool SaveImageData( QIODevice& aOutStream ) const;

    bool LoadLegacyData( LINE_READER& aLine, QString& aErrorMsg );

    void Mirror( FLIP_DIRECTION aFlipDirection );

    void Rotate( bool aRotateCCW );

    void ConvertToGreyscale();

    bool IsMirroredX() const { return m_isMirroredX; }
    bool IsMirroredY() const { return m_isMirroredY; }
    EDA_ANGLE Rotation() const { return m_rotation; }

    void PlotImage( PLOTTER* aPlotter, const VECTOR2I& aPos,
                    const KIGFX::COLOR4D& aDefaultColor, int aDefaultPensize ) const;

    QImage::Format GetImageType() const { return m_imageType; }

    void SetImageType( QImage::Format aType ) { m_imageType = aType; }

private:
    void updateImageDataBuffer();

    void rebuildBitmap( bool aResetID = true );

    void updatePPI();

    double         m_scale;
    QByteArray     m_imageData;
    QImage::Format m_imageType;

    QImage*  m_image;
    QImage*  m_originalImage;
    QPixmap* m_bitmap;
    double   m_pixelSizeIu;
    int      m_ppi;
    KIID     m_imageId;
    bool     m_isMirroredX;
    bool     m_isMirroredY;
    EDA_ANGLE m_rotation;
};
