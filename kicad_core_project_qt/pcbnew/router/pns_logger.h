
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#ifndef __PNS_LOGGER_H
#define __PNS_LOGGER_H

#include <cstdio>
#include <vector>
#include <string>
#include <sstream>

#include <QString>
#include <math/vector2d.h>
#include <kiid.h>

#include "pns_sizes_settings.h"

class SHAPE_LINE_CHAIN;
class SHAPE;

namespace PNS {

class ITEM;

class LOGGER
{
public:

    enum EVENT_TYPE {
        EVT_START_ROUTE = 0,
        EVT_START_DRAG,
        EVT_FIX,
        EVT_MOVE,
        EVT_ABORT,
        EVT_TOGGLE_VIA,
        EVT_UNFIX,
        EVT_START_MULTIDRAG
    };

    struct EVENT_ENTRY {
        VECTOR2I p;
        EVENT_TYPE type;
        std::vector<KIID> uuids;
        SIZES_SETTINGS sizes;
        int layer;

        EVENT_ENTRY() :
                layer( 0 ),
                type( EVT_START_ROUTE )
        {
        }

        EVENT_ENTRY( const EVENT_ENTRY& aE ) :
                p( aE.p ),
                type( aE.type ),
                uuids( aE.uuids ),
                sizes( aE.sizes ),
                layer( aE.layer )
        {
        }
    };

    LOGGER();
    ~LOGGER();

    void Clear();

    void LogM( EVENT_TYPE evt, const VECTOR2I& pos = VECTOR2I(), std::vector<ITEM*> items = {},
              const SIZES_SETTINGS* sizes = nullptr, int aLayer = 0 );

    void Log( EVENT_TYPE evt, const VECTOR2I& pos = VECTOR2I(), const ITEM* item = nullptr,
              const SIZES_SETTINGS* sizes = nullptr, int aLayer = 0 );

    const std::vector<EVENT_ENTRY>& GetEvents()
    {
        return m_events;
    }

    static QString FormatLogFileAsString( int aMode,
                                          const std::vector<ITEM*>& aAddedItems,
                                          const std::set<KIID>&     aRemovedItems,
                                          const std::vector<ITEM*>& aHeads,
                                          const std::vector<EVENT_ENTRY>& aEvents );

    static QString FormatEvent( const EVENT_ENTRY& aEvent );

    static EVENT_ENTRY ParseEvent( const QString& aLine );

private:
    std::vector<EVENT_ENTRY> m_events;
};

}

#endif
