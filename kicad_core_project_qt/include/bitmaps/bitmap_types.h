#ifndef BITMAP_TYPES_H_
#define BITMAP_TYPES_H_

#include <kicommon.h>

class QPixmap;
class QIcon;
class QWidget;
class QString;
class BITMAP_STORE;

enum class BITMAPS : unsigned int;

enum class BITMAP_TYPE
{
    PNG,
    JPG,
    BMP
};

KICOMMON_API BITMAP_STORE* GetBitmapStore();

KICOMMON_API QPixmap KiBitmap( BITMAPS aBitmap, int aHeightTag = -1 );

KICOMMON_API QIcon KiBitmapBundle( BITMAPS aBitmap, int aMinHeight = -1 );

KICOMMON_API QIcon KiDisabledBitmapBundle( BITMAPS aBitmap );

KICOMMON_API void ClearScaledBitmapCache();

KICOMMON_API QPixmap KiScaledBitmap( BITMAPS aBitmap, QWidget* aWidget, int aHeight = -1,
                                      bool aQuantized = false );

KICOMMON_API QPixmap KiScaledBitmap( const QPixmap& aBitmap, QWidget* aWidget );

KICOMMON_API int KiIconScale( QWidget* aWidget );

KICOMMON_API QPixmap* KiBitmapNew( BITMAPS aBitmap );

#endif  // BITMAP_TYPES_H_
