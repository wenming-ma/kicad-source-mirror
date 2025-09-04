#include <QPixmap>
#include <QBitmap>
#include <QImage>
#include <QBuffer>
#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QPainter>
#include <QHash>

#include <cstdint>
#include <mutex>

#include <asset_archive.h>
#include <bitmaps.h>
#include <bitmap_store.h>
#include <pgm_base.h>
#include <paths.h>
#include <kiplatform/ui.h>
#include <math/util.h>
#include <settings/common_settings.h>

static std::unique_ptr<BITMAP_STORE> s_BitmapStore;


struct SCALED_BITMAP_ID {
    BITMAPS bitmap;
    int scale;

    bool operator==( SCALED_BITMAP_ID const& other ) const noexcept
    {
        return bitmap == other.bitmap && scale == other.scale;
    }
};


inline uint qHash( const SCALED_BITMAP_ID& id, uint seed = 0 )
{
    static const bool sz64 = sizeof( uintptr_t ) == 8;
    static const size_t mask = sz64 ? 0xF000000000000000uLL : 0xF0000000uL;
    static const size_t offset = sz64 ? 60 : 28;

    // The hash only needs to be fast and simple, not necessarily accurate - a collision
    // only makes things slower, not broken. BITMAPS is a pointer, so the most
    // significant several bits are generally going to be the same for all. Just convert
    // it to an integer and stuff the scale factor into those bits.
    uint result = ( (uintptr_t)( id.bitmap ) & ~mask ) |
                  ( ( (uintptr_t)( id.scale ) & 0xF ) << offset );
    return qHashBits( &result, sizeof(result), seed );
}


static QHash<SCALED_BITMAP_ID, QPixmap> s_ScaledBitmapCache;

static std::mutex s_BitmapCacheMutex;


BITMAP_STORE* GetBitmapStore()
{
    if( !s_BitmapStore )
    {
        QString stockPath = PATHS::GetStockDataPath() + QString( "/resources" );
        s_BitmapStore = std::make_unique<BITMAP_STORE>();
    }

    return s_BitmapStore.get();
}


QPixmap KiBitmap( BITMAPS aBitmap, int aHeightTag )
{
    return GetBitmapStore()->GetBitmap( aBitmap, aHeightTag );
}


QIcon KiBitmapBundle( BITMAPS aBitmap, int aMinHeight )
{
    return GetBitmapStore()->GetBitmapBundle( aBitmap, aMinHeight );
}


QIcon KiDisabledBitmapBundle( BITMAPS aBitmap )
{
    return GetBitmapStore()->GetDisabledBitmapBundle( aBitmap );
}


int KiIconScale( QWidget* aWidget )
{
    // For historical reasons, "4" here means unity (no scaling)

#if defined( Q_OS_WIN )
    // Basically don't try and scale within KiCad and let Qt do its thing
    // Qt will auto scale automatically with dpi scaling
    return 4;
#else
    const int vert_size = aWidget ? (aWidget->logicalDpiY() * 8) / 96 : 8;

    // Autoscale won't exceed unity until the system has quite high resolution,
    // because we don't want the icons to look obviously scaled on a system
    // where it's easy to see it.

    if( vert_size > 34 )        return 8;
    else if( vert_size > 29 )   return 7;
    else if( vert_size > 24 )   return 6;
    else                        return 4;
#endif
}


QPixmap KiScaledBitmap( BITMAPS aBitmap, QWidget* aWidget, int aHeight, bool aQuantized )
{
    // Bitmap conversions are cached because they can be slow.
    int scale = KiIconScale( aWidget );

    if( aQuantized )
        scale = KiROUND( (double) scale / 4.0 ) * 4;

    SCALED_BITMAP_ID id = { static_cast<BITMAPS>( aBitmap ), scale };

    std::lock_guard<std::mutex> guard( s_BitmapCacheMutex );
    auto it = s_ScaledBitmapCache.find( id );

    if( it != s_ScaledBitmapCache.end() )
    {
        return it->second;
    }
    else
    {
        QPixmap bitmap = GetBitmapStore()->GetBitmapScaled( aBitmap, scale, aHeight );
        s_ScaledBitmapCache.insert( id, bitmap );
        return bitmap;
    }
}


void ClearScaledBitmapCache()
{
    std::lock_guard<std::mutex> guard( s_BitmapCacheMutex );
    s_ScaledBitmapCache.clear();
}


QPixmap KiScaledBitmap( const QPixmap& aBitmap, QWidget* aWidget )
{
    const int scale = KiIconScale( aWidget );

    if( scale == 4 )
    {
        return QPixmap( aBitmap );
    }
    else
    {
        QImage image = aBitmap.toImage();
        image = image.scaled( scale * image.width() / 4, scale * image.height() / 4,
                             Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

        return QPixmap::fromImage( image );
    }
}


QPixmap* KiBitmapNew( BITMAPS aBitmap )
{
    QPixmap* bitmap = new QPixmap( GetBitmapStore()->GetBitmap( aBitmap ) );

    return bitmap;
}
