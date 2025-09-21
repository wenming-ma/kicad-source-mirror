
#ifndef SYNC_SHEET_PIN_PREFERENCE_H
#define SYNC_SHEET_PIN_PREFERENCE_H

#include <bitmaps/bitmap_types.h>
#include <bitmaps/bitmaps_list.h>
#include <map>
#include <QColor>

class SYNC_SHEET_PIN_PREFERENCE
{
public:
    enum ICON_SIZE
    {
        NORMAL_WIDTH = 16,
        NORMAL_HEIGHT = 16

    };

    enum BOOKCTRL_ICON_INDEX
    {
        HAS_UNMATCHED,
        ALL_MATCHED
    };

    static const std::map<BOOKCTRL_ICON_INDEX, BITMAPS>& GetBookctrlPageIcon()
    {
        static std::map<BOOKCTRL_ICON_INDEX, BITMAPS> mapping = {
            { HAS_UNMATCHED, BITMAPS::erc_green }, { ALL_MATCHED, BITMAPS::ercwarn }
        };
        return mapping;
    }
};

#endif