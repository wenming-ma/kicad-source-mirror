#ifndef KICAD_BITMAP_STORE_H
#define KICAD_BITMAP_STORE_H

#include <memory>
#include <unordered_map>

#include <QPixmap>
#include <QImage>
#include <QString>
#include <hashtables.h>

#include <bitmaps/bitmap_info.h>
#include <kicommon.h>
#include <asset_archive.h>


namespace std
{
    template<> struct KICOMMON_API hash<std::pair<BITMAPS, int>>
    {
        size_t operator()( const std::pair<BITMAPS, int>& aPair ) const;
    };
}

// Helper to retrieve bitmaps while handling icon themes and scaling
class KICOMMON_API BITMAP_STORE
{
public:
    BITMAP_STORE();

    ~BITMAP_STORE() = default;

    // Retrieves a bitmap from the given bitmap id
    QPixmap GetBitmap( BITMAPS aBitmapId, int aHeight = -1 );

    // Constructs and returns a bitmap bundle containing all available sizes of the given ID
    QPixmap GetBitmapBundle( BITMAPS aBitmapId, int aMinHeight = -1 );
     
    // Constructs and returns a bitmap bundle for the given icon ID, with the bitmaps
    // converted to disabled state according to the current UI theme
    QPixmap GetDisabledBitmapBundle( BITMAPS aBitmapId );

    // Retrieves a bitmap from the given bitmap id, scaled to a given factor
    // This factor is for legacy reasons divided by 4, so a scale factor of 4 will return the original image
    QPixmap GetBitmapScaled( BITMAPS aBitmapId, int aScaleFactor, int aHeight = -1 );

    // Notifies the store that the icon theme has been changed by the user, so caches must be invalidated
    void ThemeChanged();

    bool IsDarkTheme() const { return m_theme == QStringLiteral( "dark" ); }

private:

    QImage getImage( BITMAPS aBitmapId, int aHeight = -1 );

    const QString& bitmapName( BITMAPS aBitmapId, int aHeight = -1 );

    QString computeBitmapName( BITMAPS aBitmapId, int aHeight = -1 );

    void buildBitmapInfoCache();

    std::unique_ptr<ASSET_ARCHIVE> m_archive;

    std::unordered_map<std::pair<BITMAPS, int>, QString> m_bitmapNameCache;

    std::unordered_map<BITMAPS, std::vector<BITMAP_INFO>> m_bitmapInfoCache;

    QString m_theme;
};

#endif // KICAD_BITMAP_STORE_H
