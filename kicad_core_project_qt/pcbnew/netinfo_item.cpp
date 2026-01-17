// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05
#include <QtCore/QString>
#include <QtCore/QDebug>

#include <pcb_base_frame.h>
#include <string_utils.h>
#include <widgets/msgpanel.h>
#include <base_units.h>
#include <board.h>
#include <board_design_settings.h>
#include <connectivity/connectivity_data.h>
#include <footprint.h>
#include <pcb_track.h>
#include <pad.h>


NETINFO_ITEM::NETINFO_ITEM( BOARD* aParent, const QString& aNetName, int aNetCode ) :
        BOARD_ITEM( aParent, PCB_NETINFO_T ),
        m_netCode( aNetCode ),
        m_netname( aNetName ),
        m_shortNetname( m_netname.section( '/', -1 ) ),
        m_displayNetname( UnescapeString( m_shortNetname ) ),
        m_isCurrent( true )
{
    m_parent = aParent;

    if( aParent )
        m_netClass = aParent->GetDesignSettings().m_NetSettings->GetDefaultNetclass();
    else
        m_netClass = std::make_shared<NETCLASS>( QStringLiteral( "Default" ) );
}


NETINFO_ITEM::~NETINFO_ITEM()
{
    // m_NetClass is not owned by me.
}


void NETINFO_ITEM::Clear()
{
    Q_ASSERT( m_parent );
    m_netClass = m_parent->GetDesignSettings().m_NetSettings->GetDefaultNetclass();
}


void NETINFO_ITEM::SetNetClass( const std::shared_ptr<NETCLASS>& aNetClass )
{
    Q_ASSERT( m_parent );

    if( aNetClass )
        m_netClass = aNetClass;
    else
        m_netClass = m_parent->GetDesignSettings().m_NetSettings->GetDefaultNetclass();
}


void NETINFO_ITEM::GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList )
{
    QString msg;

    aList.emplace_back( _( "Net Name" ), UnescapeString( GetNetname() ) );

    aList.emplace_back( _( "Net Code" ), QString::asprintf( "%d", GetNetCode() ) );

    // Warning: for netcode == NETINFO_LIST::ORPHANED, the parent or the board can be NULL
    BOARD * board = m_parent ? m_parent->GetBoard() : nullptr;

    if( board )
    {
        int        count      = 0;
        PCB_TRACK* startTrack = nullptr;

        for( FOOTPRINT* footprint : board->Footprints() )
        {
            for( PAD* pad : footprint->Pads() )
            {
                if( pad->GetNetCode() == GetNetCode() )
                    count++;
            }
        }

        aList.emplace_back( _( "Pads" ), QString::asprintf( "%d", count ) );

        count = 0;

        for( PCB_TRACK* track : board->Tracks() )
        {
            if( track->GetNetCode() == GetNetCode() )
            {
                if( track->Type() == PCB_VIA_T )
                    count++;
                else if( !startTrack )
                    startTrack = track;
            }
        }

        aList.emplace_back( _( "Vias" ), QString::asprintf( "%d", count ) );

        if( startTrack )
        {
            double lengthNet      = 0.0; // This  is the length of tracks on pcb
            double lengthPadToDie = 0.0; // this is the length of internal ICs connections

            std::tie( count, lengthNet, lengthPadToDie ) = board->GetTrackLength( *startTrack );

            // Displays the full net length (tracks on pcb + internal ICs connections ):
            aList.emplace_back( _( "Net Length" ),
                                aFrame->MessageTextFromValue( lengthNet + lengthPadToDie ) );

            // Displays the net length of tracks only:
            aList.emplace_back( _( "On Board" ), aFrame->MessageTextFromValue( lengthNet ) );

            // Displays the net length of internal ICs connections (wires inside ICs):
            aList.emplace_back( _( "In Package" ), aFrame->MessageTextFromValue( lengthPadToDie ) );
        }
    }
}


bool NETINFO_ITEM::Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const
{
    return BOARD_ITEM::Matches( GetNetname(), aSearchData );
}


const BOX2I NETINFO_ITEM::GetBoundingBox() const
{
    static const std::vector<KICAD_T> netItemTypes = { PCB_TRACE_T,
                                                       PCB_ARC_T,
                                                       PCB_VIA_T,
                                                       PCB_ZONE_T,
                                                       PCB_PAD_T,
                                                       PCB_SHAPE_T };

    std::shared_ptr<CONNECTIVITY_DATA> conn = GetBoard()->GetConnectivity();
    BOX2I                              bbox;

    for( BOARD_ITEM* item : conn->GetNetItems( m_netCode, netItemTypes ) )
        bbox.Merge( item->GetBoundingBox() );

    return bbox;
}
