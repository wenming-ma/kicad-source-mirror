

#ifndef KICAD_BITMAP_INFO_H
#define KICAD_BITMAP_INFO_H

#include <kicommon.h>
#include <vector>
#include <unordered_map>
#include <bitmaps/bitmaps_list.h>

#include <QString>

struct KICOMMON_API BITMAP_INFO
{
    BITMAPS  id;
    QString filename;
    int      height;
    QString theme;

    BITMAP_INFO( BITMAPS aId, const QString& aFilename, int aHeight, const QString& aTheme ) :
        id( aId ),
        filename( aFilename ),
        height( aHeight ),
        theme( aTheme )
    {};

};


extern KICOMMON_API void BuildBitmapInfo( std::unordered_map<BITMAPS,
                             std::vector<BITMAP_INFO>>& aBitmapInfoCache );

#endif // KICAD_BITMAP_INFO_H
