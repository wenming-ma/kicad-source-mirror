
#include <board.h>
#include <board_connected_item.h>
#include <board_design_settings.h>
#include <connectivity/connectivity_data.h>
#include <lset.h>
#include <properties/property_validators.h>
#include <string_utils.h>
#include <i18n_utility.h>
#include <netinfo.h>
// #include <api/board/board_types.pb.h> // DISABLED FOR MINIMAL BUILD

using namespace std::placeholders;

BOARD_CONNECTED_ITEM::BOARD_CONNECTED_ITEM( BOARD_ITEM* aParent, KICAD_T idtype ) :
    BOARD_ITEM( aParent, idtype ),
    m_netinfo( NETINFO_LIST::OrphanedItem() )
{
    m_localRatsnestVisible = true;
}


// DISABLED FOR MINIMAL BUILD - API functionality removed
/*
void BOARD_CONNECTED_ITEM::UnpackNet( const kiapi::board::types::Net& aProto )
{
    if( BOARD* board = GetBoard() )
    {
        QString name = QString::fromUtf8( aProto.name() );

        if( NETINFO_ITEM* net = board->FindNet( name ) )
        {
            m_netinfo = net;
        }
        else
        {
            NETINFO_ITEM* newnet = new NETINFO_ITEM( board, name, 0 );
            board->Add( newnet );
            m_netinfo = newnet;
        }
    }
}
*/


// DISABLED FOR MINIMAL BUILD - API functionality removed
/*
void BOARD_CONNECTED_ITEM::PackNet( kiapi::board::types::Net* aProto ) const
{
    aProto->set_name( GetNetname().ToUTF8() );
}
*/


bool BOARD_CONNECTED_ITEM::SetNetCode( int aNetCode, bool aNoAssert )
{
    if( !IsOnCopperLayer() )
        aNetCode = 0;

    // if aNetCode < 0 (typically NETINFO_LIST::FORCE_ORPHANED) or no parent board,
    // set the m_netinfo to the dummy NETINFO_LIST::ORPHANED

    BOARD* board = GetBoard();

    if( ( aNetCode >= 0 ) && board )
        m_netinfo = board->FindNet( aNetCode );
    else
        m_netinfo = NETINFO_LIST::OrphanedItem();

    if( !aNoAssert )
        Q_ASSERT( m_netinfo );

    return ( m_netinfo != nullptr );
}


int BOARD_CONNECTED_ITEM::GetOwnClearance( PCB_LAYER_ID aLayer, QString* aSource ) const
{
    DRC_CONSTRAINT constraint;

    if( GetBoard() && GetBoard()->GetDesignSettings().m_DRCEngine )
    {
        BOARD_DESIGN_SETTINGS& bds = GetBoard()->GetDesignSettings();

        constraint = bds.m_DRCEngine->EvalRules( CLEARANCE_CONSTRAINT, this, nullptr, aLayer );
    }

    if( constraint.Value().HasMin() )
    {
        if( aSource )
            *aSource = constraint.GetName();

        return constraint.Value().Min();
    }

    return 0;
}


int BOARD_CONNECTED_ITEM::GetNetCode() const
{
    return m_netinfo ? m_netinfo->GetNetCode() : -1;
}


// Note: do NOT return a std::shared_ptr from this.  It is used heavily in DRC, and the
// std::shared_ptr stuff shows up large in performance profiling.
NETCLASS* BOARD_CONNECTED_ITEM::GetEffectiveNetClass() const
{
    if( m_netinfo && m_netinfo->GetNetClass() )
        return m_netinfo->GetNetClass();
    else
        return GetBoard()->GetDesignSettings().m_NetSettings->GetDefaultNetclass().get();
}


QString BOARD_CONNECTED_ITEM::GetNetClassName() const
{
    return GetEffectiveNetClass()->GetName();
}


QString BOARD_CONNECTED_ITEM::GetNetname() const
{
    return m_netinfo ? m_netinfo->GetNetname() : QString();
}


QString BOARD_CONNECTED_ITEM::GetNetnameMsg() const
{
    if( !GetBoard() )
        return QStringLiteral( "[** NO BOARD DEFINED **]" );

    QString netname = GetNetname();

    if( !netname.length() )
        return QStringLiteral( "[<no net>]" );
    else if( GetNetCode() < 0 )
        return QStringLiteral( "[" ) + UnescapeString( netname ) + QStringLiteral( "](" ) + _( "Not Found" ) + QStringLiteral( ")" );
    else
        return QStringLiteral( "[" ) + UnescapeString( netname ) + QStringLiteral( "]" );
}


const QString& BOARD_CONNECTED_ITEM::GetShortNetname() const
{
    static QString emptyString;

    return m_netinfo ? m_netinfo->GetShortNetname() : emptyString;
}


const QString& BOARD_CONNECTED_ITEM::GetDisplayNetname() const
{
    static QString emptyString;

    if( !m_netinfo )
        return emptyString;

    if( const BOARD* board = GetBoard() )
    {
        if( board->GetNetInfo().m_DisplayNetnamesDirty )
            board->GetNetInfo().RebuildDisplayNetnames();
    }

    return m_netinfo->GetDisplayNetname();
}


static struct BOARD_CONNECTED_ITEM_DESC
{
    BOARD_CONNECTED_ITEM_DESC()
    {
        ENUM_MAP<PCB_LAYER_ID>& layerEnum = ENUM_MAP<PCB_LAYER_ID>::Instance();

        if( layerEnum.Choices().size() == 0 )
        {
            layerEnum.Undefined( UNDEFINED_LAYER );

            for( PCB_LAYER_ID layer : LSET::AllLayersMask().Seq() )
                layerEnum.Map( layer, LSET::Name( layer ) );
        }

        PROPERTY_MANAGER& propMgr = PROPERTY_MANAGER::Instance();
        REGISTER_TYPE( BOARD_CONNECTED_ITEM );
        propMgr.InheritsAfter( TYPE_HASH( BOARD_CONNECTED_ITEM ), TYPE_HASH( BOARD_ITEM ) );

        // Replace layer property as the properties panel will set a restriction for copper layers
        // only for BOARD_CONNECTED_ITEM that we don't want to apply to BOARD_ITEM
        auto layer = new PROPERTY_ENUM<BOARD_CONNECTED_ITEM, PCB_LAYER_ID, BOARD_ITEM>(
                _HKI( "Layer" ),
                &BOARD_CONNECTED_ITEM::SetLayer, &BOARD_CONNECTED_ITEM::GetLayer );
        layer->SetChoices( layerEnum.Choices() );
        propMgr.ReplaceProperty( TYPE_HASH( BOARD_ITEM ), _HKI( "Layer" ), layer );

        propMgr.AddProperty( new PROPERTY_ENUM<BOARD_CONNECTED_ITEM, int>( _HKI( "Net" ),
                             &BOARD_CONNECTED_ITEM::SetNetCode,
                             &BOARD_CONNECTED_ITEM::GetNetCode ) )
                .SetIsHiddenFromRulesEditor()
                .SetIsHiddenFromLibraryEditors();

        /**
         * This property should just be an alias for the one below, it only exists so that we
         * maintain compatibility with both `NetClass` and `Net_Class` in custom rules.
         * It has the name we would show in the GUI if we wanted to show this in the GUI, but we
         * don't at the moment because there is no way to edit the netclass of a net from a selected
         * connected item, and showing it makes users think they can change it.
         */
        propMgr.AddProperty( new PROPERTY<BOARD_CONNECTED_ITEM, QString>( _HKI( "Net Class" ),
                             NO_SETTER( BOARD_CONNECTED_ITEM, QString ),
                             &BOARD_CONNECTED_ITEM::GetNetClassName ) )
                .SetIsHiddenFromRulesEditor()
                .SetIsHiddenFromPropertiesManager()
                .SetIsHiddenFromLibraryEditors();

        // Compatibility alias for DRC engine
        propMgr.AddProperty( new PROPERTY<BOARD_CONNECTED_ITEM, QString>( _HKI( "NetClass" ),
                             NO_SETTER( BOARD_CONNECTED_ITEM, QString ),
                             &BOARD_CONNECTED_ITEM::GetNetClassName ) )
                .SetIsHiddenFromPropertiesManager()
                .SetIsHiddenFromLibraryEditors();

        // Used only in DRC engine
        propMgr.AddProperty( new PROPERTY<BOARD_CONNECTED_ITEM, QString>( _HKI( "NetName" ),
                             NO_SETTER( BOARD_CONNECTED_ITEM, QString ),
                             &BOARD_CONNECTED_ITEM::GetNetname ) )
                .SetIsHiddenFromPropertiesManager()
                .SetIsHiddenFromLibraryEditors();

        auto supportsTeardrops =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( BOARD_CONNECTED_ITEM* bci = dynamic_cast<BOARD_CONNECTED_ITEM*>( aItem ) )
                    {
                        if( !bci->GetBoard() || bci->GetBoard()->LegacyTeardrops() )
                            return false;

                        return bci->Type() == PCB_PAD_T || bci->Type() == PCB_VIA_T;
                    }

                    return false;
                };

        auto supportsTeardropPreferZoneSetting =
                []( INSPECTABLE* aItem ) -> bool
                {
                    if( BOARD_CONNECTED_ITEM* bci = dynamic_cast<BOARD_CONNECTED_ITEM*>( aItem ) )
                    {
                        if( !bci->GetBoard() || bci->GetBoard()->LegacyTeardrops() )
                            return false;

                        return bci->Type() == PCB_PAD_T;
                    }

                    return false;
                };

        const QString groupTeardrops = _HKI( "Teardrops" );

        auto enableTeardrops = new PROPERTY<BOARD_CONNECTED_ITEM, bool>( _HKI( "Enable Teardrops" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropsEnabled,
                         &BOARD_CONNECTED_ITEM::GetTeardropsEnabled );
        enableTeardrops->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( enableTeardrops, groupTeardrops );

        auto bestLength = new PROPERTY<BOARD_CONNECTED_ITEM, double>( _HKI( "Best Length Ratio" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropBestLengthRatio,
                         &BOARD_CONNECTED_ITEM::GetTeardropBestLengthRatio );
        bestLength->SetAvailableFunc( supportsTeardrops );
        bestLength->SetValidator( PROPERTY_VALIDATORS::PositiveRatioValidator );
        propMgr.AddProperty( bestLength, groupTeardrops );

        auto maxLength = new PROPERTY<BOARD_CONNECTED_ITEM, int>( _HKI( "Max Length" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropMaxLength,
                         &BOARD_CONNECTED_ITEM::GetTeardropMaxLength, PROPERTY_DISPLAY::PT_SIZE );
        maxLength->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( maxLength, groupTeardrops );

        auto bestWidth = new PROPERTY<BOARD_CONNECTED_ITEM, double>( _HKI( "Best Width Ratio" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropBestWidthRatio,
                         &BOARD_CONNECTED_ITEM::GetTeardropBestWidthRatio );
        bestWidth->SetAvailableFunc( supportsTeardrops );
        bestWidth->SetValidator( PROPERTY_VALIDATORS::PositiveRatioValidator );
        propMgr.AddProperty( bestWidth, groupTeardrops );

        auto maxWidth = new PROPERTY<BOARD_CONNECTED_ITEM, int>( _HKI( "Max Width" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropMaxWidth,
                         &BOARD_CONNECTED_ITEM::GetTeardropMaxWidth, PROPERTY_DISPLAY::PT_SIZE );
        maxWidth->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( maxWidth, groupTeardrops );

        auto curvePts = new PROPERTY<BOARD_CONNECTED_ITEM, bool>( _HKI( "Curved Teardrops" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropCurved,
                         &BOARD_CONNECTED_ITEM::GetTeardropCurved );
        curvePts->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( curvePts, groupTeardrops );

        auto preferZones = new PROPERTY<BOARD_CONNECTED_ITEM, bool>( _HKI( "Prefer Zone Connections" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropPreferZoneConnections,
                         &BOARD_CONNECTED_ITEM::GetTeardropPreferZoneConnections );
        preferZones->SetAvailableFunc( supportsTeardropPreferZoneSetting );
        propMgr.AddProperty( preferZones, groupTeardrops );

        auto twoTracks = new PROPERTY<BOARD_CONNECTED_ITEM, bool>( _HKI( "Allow Teardrops To Span Two Tracks" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropAllowSpanTwoTracks,
                         &BOARD_CONNECTED_ITEM::GetTeardropAllowSpanTwoTracks );
        twoTracks->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( twoTracks, groupTeardrops );

        auto maxTrackWidth = new PROPERTY<BOARD_CONNECTED_ITEM, double>( _HKI( "Max Width Ratio" ),
                         &BOARD_CONNECTED_ITEM::SetTeardropMaxTrackWidth,
                         &BOARD_CONNECTED_ITEM::GetTeardropMaxTrackWidth );
        maxTrackWidth->SetAvailableFunc( supportsTeardrops );
        propMgr.AddProperty( maxTrackWidth, groupTeardrops );
    }
} _BOARD_CONNECTED_ITEM_DESC;
