#include "pns_logger.h"
#include "pns_item.h"
#include "pns_via.h"

#include <QString>
#include <QStringList>
#include <QtGlobal>

namespace PNS {

LOGGER::LOGGER( )
{
}


LOGGER::~LOGGER()
{
}


void LOGGER::Clear()
{
    m_events.clear();
}


void LOGGER::LogM( LOGGER::EVENT_TYPE evt, const VECTOR2I& pos, std::vector<ITEM*> items,
                  const SIZES_SETTINGS* sizes, int aLayer )
{
    LOGGER::EVENT_ENTRY ent;

    ent.type = evt;
    ent.p = pos;
    ent.layer = aLayer;

    if( sizes )
    {
        ent.sizes = *sizes;
    }

    for( auto& item : items )
    {
        if( item && item->Parent() )
            ent.uuids.push_back( item->Parent()->m_Uuid );
    }

    m_events.push_back( ent );
}


void LOGGER::Log( LOGGER::EVENT_TYPE evt, const VECTOR2I& pos, const ITEM* item,
                  const SIZES_SETTINGS* sizes, int aLayer )
{
    std::vector<ITEM*> items;
    items.push_back( const_cast<ITEM*>( item ) );
    LogM( evt, pos, items, sizes, aLayer );
}


QString LOGGER::FormatLogFileAsString( int aMode,
                                        const std::vector<ITEM*>& aAddedItems,
                                        const std::set<KIID>&     aRemovedItems,
                                        const std::vector<ITEM*>& aHeads,
                                        const std::vector<LOGGER::EVENT_ENTRY>& aEvents )
{
    QString result = QString::asprintf( "mode %d\n", aMode );

    for( const EVENT_ENTRY& evt : aEvents )
        result += PNS::LOGGER::FormatEvent( evt );

    for( const KIID& uuid : aRemovedItems )
        result += QString::asprintf( "removed %s\n", uuid.AsString().toUtf8().constData() );

    for( ITEM* item : aAddedItems )
        result += QString::asprintf( "added %s\n", item->Format().c_str() );

    for( ITEM* item : aHeads )
        result += QString::asprintf( "head %s\n", item->Format().c_str() );

    return result;
}


QString LOGGER::FormatEvent( const LOGGER::EVENT_ENTRY& aEvent )
{
    QString str = QString::asprintf( "event %d %d %d %d %d ", aEvent.p.x, aEvent.p.y, aEvent.type, aEvent.layer, (int)aEvent.uuids.size() );

    for( int i = 0; i < aEvent.uuids.size(); i++ )
    {
        str.append( aEvent.uuids[i].AsString() );
        str.append( " " );
    }

    str.append( QString::asprintf( "%d %d %d %d %d %d %d",
            aEvent.sizes.TrackWidth(),
            aEvent.sizes.ViaDiameter(),
            aEvent.sizes.ViaDrill(),
            aEvent.sizes.TrackWidthIsExplicit() ? 1 : 0,
            aEvent.sizes.GetLayerBottom(),
            aEvent.sizes.GetLayerTop(),
            static_cast<int>( aEvent.sizes.ViaType() ) ) );

    str.append( "\n" );

    return str;
}


LOGGER::EVENT_ENTRY LOGGER::ParseEvent( const QString& aLine )
{
    QStringList tokens = aLine.split( ' ', Qt::SkipEmptyParts );
    int tokenIndex = 0;
    QString cmd = tokens.value( tokenIndex++ );

    int n_uuids = 0;

    Q_ASSERT_X( cmd == "event", "ParseEvent", "Line doesn't contain an event!" );

    EVENT_ENTRY evt;
    evt.p.x = tokens.value( tokenIndex++ ).toInt();
    evt.p.y = tokens.value( tokenIndex++ ).toInt();
    evt.type = (PNS::LOGGER::EVENT_TYPE) tokens.value( tokenIndex++ ).toInt();
    evt.layer = tokens.value( tokenIndex++ ).toInt();
    n_uuids = tokens.value( tokenIndex++ ).toInt();

    for( int i = 0; i < n_uuids; i++)
        evt.uuids.push_back( KIID( tokens.value( tokenIndex++ ) ) );

    return evt;
}

}
