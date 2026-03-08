/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <qa_utils/wx_utils/unit_test_utils.h>
#include <cstdint>
#include <algorithm>
#include <cmath>
#include <geometry/shape_rect.h>
#include <settings/settings_manager.h>
#include <memory>
#include <optional>

#include <pcbnew/board.h>
#include <pcbnew/pad.h>
#include <pcbnew/pcb_track.h>

#include <router/pns_bundle_placer.h>
#include <router/pns_item.h>
#include <router/pns_kicad_iface.h>
#include <router/pns_node.h>
#include <router/pns_routing_settings.h>
#include <router/pns_router.h>
#include <router/pns_segment.h>
#include <router/pns_solid.h>
#include <router/pns_via.h>

static bool isCopper( const PNS::ITEM* aItem )
{
    if( !aItem )
        return false;

    BOARD_ITEM* parent = aItem->Parent();

    if( parent && parent->Type() == PCB_PAD_T )
    {
        PAD* pad = static_cast<PAD*>( parent );

        if( pad->IsAperturePad() || pad->IsNPTHWithNoCopper() )
            return false;
    }

    return true;
}


static bool isHole( const PNS::ITEM* aItem )
{
    if( !aItem )
        return false;

    return aItem->OfKind( PNS::ITEM::HOLE_T );
}


static bool isEdge( const PNS::ITEM* aItem )
{
    if( !aItem )
        return false;

    const BOARD_ITEM *parent = aItem->BoardItem();

    return parent && ( parent->IsOnLayer( Edge_Cuts ) || parent->IsOnLayer( Margin ) );
}


class MOCK_RULE_RESOLVER : public PNS::RULE_RESOLVER
{
public:
    MOCK_RULE_RESOLVER() : m_clearanceEpsilon( 10 )
    {
    }

    virtual ~MOCK_RULE_RESOLVER() {}

    virtual int Clearance( const PNS::ITEM* aA, const PNS::ITEM* aB,
                           bool aUseClearanceEpsilon = true ) override
    {
        PNS::CONSTRAINT constraint;
        int             rv = 0;
        PNS_LAYER_RANGE     layers;

        if( !aB )
            layers = aA->Layers();
        else if( isEdge( aA ) )
            layers = aB->Layers();
        else if( isEdge( aB ) )
            layers = aA->Layers();
        else
            layers = aA->Layers().Intersection( aB->Layers() );

        // Normalize layer range (no -1 magic numbers)
        layers = layers.Intersection( PNS_LAYER_RANGE( PCBNEW_LAYER_ID_START, PCB_LAYER_ID_COUNT - 1 ) );

        for( int layer = layers.Start(); layer <= layers.End(); ++layer )
        {
            if( isHole( aA ) && isHole( aB) )
            {
                if( QueryConstraint( PNS::CONSTRAINT_TYPE::CT_HOLE_TO_HOLE, aA, aB, layer, &constraint ) )
                {
                    if( constraint.m_Value.Min() > rv )
                        rv = constraint.m_Value.Min();
                }
            }
            else if( isHole( aA ) || isHole( aB ) )
            {
                if( QueryConstraint( PNS::CONSTRAINT_TYPE::CT_HOLE_CLEARANCE, aA, aB, layer, &constraint ) )
                {
                    if( constraint.m_Value.Min() > rv )
                        rv = constraint.m_Value.Min();
                }
            }
            else if( isCopper( aA ) && ( !aB || isCopper( aB ) ) )
            {
                if( QueryConstraint( PNS::CONSTRAINT_TYPE::CT_CLEARANCE, aA, aB, layer, &constraint ) )
                {
                    if( constraint.m_Value.Min() > rv )
                        rv = constraint.m_Value.Min();
                }
            }
            else if( isEdge( aA ) || ( aB && isEdge( aB ) ) )
            {
                if( QueryConstraint( PNS::CONSTRAINT_TYPE::CT_EDGE_CLEARANCE, aA, aB, layer, &constraint ) )
                {
                    if( constraint.m_Value.Min() > rv )
                        rv = constraint.m_Value.Min();
                }
            }
        }

        return rv;
    }

    virtual PNS::NET_HANDLE DpCoupledNet( PNS::NET_HANDLE aNet ) override { return nullptr; }
    virtual int DpNetPolarity( PNS::NET_HANDLE aNet ) override { return -1; }

    virtual bool DpNetPair( const PNS::ITEM* aItem, PNS::NET_HANDLE& aNetP,
                            PNS::NET_HANDLE& aNetN ) override
    {
        return false;
    }

    virtual int NetCode( PNS::NET_HANDLE aNet ) override
    {
        return -1;
    }

    virtual wxString NetName( PNS::NET_HANDLE aNet ) override
    {
        return wxEmptyString;
    }

    virtual bool QueryConstraint( PNS::CONSTRAINT_TYPE aType, const PNS::ITEM* aItemA,
                                  const PNS::ITEM* aItemB, int aLayer,
                                  PNS::CONSTRAINT* aConstraint ) override
    {
        ITEM_KEY key;

        key.a = aItemA;
        key.b = aItemB;
        key.type = aType;

        auto it = m_ruleMap.find( key );

        if( it == m_ruleMap.end() )
        {
            int cl;
            switch( aType )
            {
            case PNS::CONSTRAINT_TYPE::CT_CLEARANCE:      cl = m_defaultClearance;   break;
            case PNS::CONSTRAINT_TYPE::CT_HOLE_TO_HOLE:   cl = m_defaultHole2Hole;   break;
            case PNS::CONSTRAINT_TYPE::CT_HOLE_CLEARANCE: cl = m_defaultHole2Copper; break;
            default: return false;
            }

            //printf("GetDef %s %s %d cl %d\n", aItemA->KindStr().c_str(), aItemB->KindStr().c_str(), aType, cl );

            aConstraint->m_Type = aType;
            aConstraint->m_Value.SetMin( cl );

            return true;
        }
        else
        {
            *aConstraint = it->second;
        }

        return true;
    }

    int ClearanceEpsilon() const override { return m_clearanceEpsilon; }

    struct ITEM_KEY
    {
        const PNS::ITEM*     a = nullptr;
        const PNS::ITEM*     b = nullptr;
        PNS::CONSTRAINT_TYPE type;

        bool operator==( const ITEM_KEY& other ) const
        {
            return a == other.a && b == other.b && type == other.type;
        }

        bool operator<( const ITEM_KEY& other ) const
        {
            if( a < other.a )
            {
                return true;
            }
            else if ( a == other.a )
            {
                if( b < other.b )
                    return true;
                else if ( b == other.b )
                    return type < other.type;
            }

            return false;
        }
    };

    bool IsInNetTie( const PNS::ITEM* aA ) override { return false; }

    bool IsNetTieExclusion( const PNS::ITEM* aItem, const VECTOR2I& aCollisionPos,
                            const PNS::ITEM* aCollidingItem ) override
    {
        return false;
    }

    bool IsDrilledHole( const PNS::ITEM* aItem ) override { return false; }

    bool IsNonPlatedSlot( const PNS::ITEM* aItem ) override { return false; }

    bool IsKeepout( const PNS::ITEM* aObstacle, const PNS::ITEM* aItem, bool* aEnforce ) override
    {
        return false;
    }

    void AddMockRule( PNS::CONSTRAINT_TYPE aType, const PNS::ITEM* aItemA, const PNS::ITEM* aItemB,
                      PNS::CONSTRAINT& aConstraint )
    {
        ITEM_KEY key;

        key.a = aItemA;
        key.b = aItemB;
        key.type = aType;

        m_ruleMap[key] = aConstraint;
    }

    int m_defaultClearance = 200000;
    int m_defaultHole2Hole = 220000;
    int m_defaultHole2Copper = 210000;

private:
    std::map<ITEM_KEY, PNS::CONSTRAINT> m_ruleMap;
    int                                 m_clearanceEpsilon;
};

struct PNS_TEST_FIXTURE;

class MOCK_PNS_KICAD_IFACE : public PNS_KICAD_IFACE_BASE
{
public:
    MOCK_PNS_KICAD_IFACE( PNS_TEST_FIXTURE *aFixture ) :
        m_testFixture( aFixture )
    {}

    ~MOCK_PNS_KICAD_IFACE() override {}

    void HideItem( PNS::ITEM* aItem ) override {};
    void DisplayItem( const PNS::ITEM* aItem, int aClearance, bool aEdit = false,
                      int aFlags = 0 ) override {};
    PNS::RULE_RESOLVER* GetRuleResolver() override;

private:
    PNS_TEST_FIXTURE* m_testFixture;
};


struct PNS_TEST_FIXTURE
{
    PNS_TEST_FIXTURE() :
            m_routerSettings( nullptr, "" )
    {
        m_router = new PNS::ROUTER;
        m_iface = new MOCK_PNS_KICAD_IFACE( this );
        m_router->SetInterface( m_iface );
        m_iface->SetBoard( &m_board );
        m_router->LoadSettings( &m_routerSettings );
        ResetRouterWorld();
    }

    ~PNS_TEST_FIXTURE()
    {
        delete m_router;
        delete m_iface;
    }

    void ResetRouterWorld()
    {
        m_router->ClearWorld();
        m_router->SyncWorld();

        m_router->GetWorld()->SetRuleResolver( &m_ruleResolver );
        m_router->GetWorld()->SetMaxClearance( 2000000 );
    }

    SETTINGS_MANAGER      m_settingsManager;
    PNS::ROUTING_SETTINGS m_routerSettings;
    BOARD                 m_board;
    PNS::ROUTER*          m_router;
    MOCK_RULE_RESOLVER    m_ruleResolver;
    MOCK_PNS_KICAD_IFACE* m_iface;
};


PNS::RULE_RESOLVER* MOCK_PNS_KICAD_IFACE::GetRuleResolver()
{
    return &m_testFixture->m_ruleResolver;
}

static void dumpObstacles( const PNS::NODE::OBSTACLES &obstacles )
{
    for( const PNS::OBSTACLE& obs : obstacles )
    {
        BOOST_TEST_MESSAGE( wxString::Format( "%p [%s] - %p [%s], clearance %d",
                obs.m_head, obs.m_head->KindStr().c_str(),
                obs.m_item, obs.m_item->KindStr().c_str(),
                obs.m_clearance ) );
    }
}


struct BUNDLE_TEST_SCENE
{
    std::vector<PNS::ITEM*> startItems;
    VECTOR2I                startPoint;
    VECTOR2I                targetPoint;
    std::vector<VECTOR2I>   expectedStartAnchors;
};


static PNS::VIA* addTestVia( PNS::NODE* aWorld, const VECTOR2I& aPos, int aDiameter, int aDrill,
                             PNS::NET_HANDLE aNet )
{
    PNS::VIA* via = new PNS::VIA( aPos, PNS_LAYER_RANGE( F_Cu, B_Cu ), aDiameter, aDrill, aNet );
    aWorld->AddRaw( via );
    return via;
}


static PNS::SEGMENT* addTestSegment( PNS::NODE* aWorld, const VECTOR2I& aStart,
                                     const VECTOR2I& aEnd, int aWidth, PNS::NET_HANDLE aNet )
{
    PNS::SEGMENT* segment = new PNS::SEGMENT( SEG( aStart, aEnd ), aNet );
    segment->SetWidth( aWidth );
    segment->SetLayers( PNS_LAYER_RANGE( F_Cu, F_Cu ) );
    aWorld->AddRaw( segment );
    return segment;
}


static PNS::SOLID* addTestSolid( PNS::NODE* aWorld, const BOX2I& aBox, PNS::NET_HANDLE aNet )
{
    PNS::SOLID* solid = new PNS::SOLID();
    solid->SetShape( new SHAPE_RECT( aBox ) );
    solid->SetPos( aBox.Centre() );
    solid->SetLayers( PNS_LAYER_RANGE( F_Cu, F_Cu ) );
    solid->SetNet( aNet );
    aWorld->AddRaw( solid );
    return solid;
}


static PNS::SIZES_SETTINGS makeBundleTestSizes( int aTrackCount = 2 )
{
    PNS::SIZES_SETTINGS sizes;

    sizes.SetTrackWidth( 100000 );
    sizes.SetTrackWidthIsExplicit( true );
    sizes.SetBoardMinTrackWidth( 100000 );
    sizes.SetClearance( 100000 );
    sizes.SetMinClearance( 100000 );
    sizes.SetBundleGap( 200000 );
    sizes.SetBundleTrackCount( aTrackCount );
    sizes.SetViaDiameter( 350000 );
    sizes.SetViaDrill( 150000 );

    return sizes;
}


static bool bundleTraceHasCollision( PNS::BUNDLE_PLACER& aPlacer )
{
    PNS::NODE* node = aPlacer.CurrentNode();

    if( !node )
        return false;

    PNS::ITEM_SET traces = aPlacer.Traces();

    for( PNS::ITEM* item : traces.CItems() )
    {
        if( node->CheckColliding( static_cast<PNS::LINE*>( item ) ) )
            return true;
    }

    return false;
}


static BUNDLE_TEST_SCENE buildBlockedBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    const auto net1 = reinterpret_cast<PNS::NET_HANDLE>( 1 );
    const auto net2 = reinterpret_cast<PNS::NET_HANDLE>( 2 );
    const auto obstacleNet = reinterpret_cast<PNS::NET_HANDLE>( 99 );

    PNS::VIA* startA = addTestVia( world, VECTOR2I( 0, -250000 ), 300000, 120000, net1 );
    PNS::VIA* startB = addTestVia( world, VECTOR2I( 0, 250000 ), 300000, 120000, net2 );

    addTestSegment( world, VECTOR2I( 2500000, -450000 ), VECTOR2I( 2500000, 450000 ),
                    180000, obstacleNet );

    addTestSolid( world, BOX2I( VECTOR2I( 1900000, 500000 ), VECTOR2I( 1200000, 500000 ) ),
                  obstacleNet );
    addTestSolid( world, BOX2I( VECTOR2I( 1900000, -1000000 ), VECTOR2I( 1200000, 500000 ) ),
                  obstacleNet );

    return { { startA, startB }, VECTOR2I( 0, 0 ), VECTOR2I( 5000000, 0 ) };
}


static BUNDLE_TEST_SCENE buildOffsetAnchorBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    const auto net1 = reinterpret_cast<PNS::NET_HANDLE>( 1 );
    const auto net2 = reinterpret_cast<PNS::NET_HANDLE>( 2 );
    const auto obstacleNet = reinterpret_cast<PNS::NET_HANDLE>( 99 );

    PNS::VIA* startA = addTestVia( world, VECTOR2I( 0, -350000 ), 300000, 120000, net1 );
    PNS::VIA* startB = addTestVia( world, VECTOR2I( 450000, 250000 ), 300000, 120000, net2 );

    addTestSegment( world, VECTOR2I( 2500000, -450000 ), VECTOR2I( 2500000, 450000 ),
                    180000, obstacleNet );

    addTestSolid( world, BOX2I( VECTOR2I( 1900000, 500000 ), VECTOR2I( 1200000, 500000 ) ),
                  obstacleNet );
    addTestSolid( world, BOX2I( VECTOR2I( 1900000, -1000000 ), VECTOR2I( 1200000, 500000 ) ),
                  obstacleNet );

    VECTOR2I centroid( ( startA->Pos().x + startB->Pos().x ) / 2,
                       ( startA->Pos().y + startB->Pos().y ) / 2 );

    return { { startA, startB }, centroid + VECTOR2I( 600000, 0 ),
             centroid + VECTOR2I( 5000000, 0 ) };
}


static BUNDLE_TEST_SCENE buildTrackStubBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    std::vector<PNS::ITEM*> startItems;
    std::vector<VECTOR2I>   expectedStartAnchors;

    startItems.reserve( 4 );
    expectedStartAnchors.reserve( 4 );

    for( int i = 0; i < 4; ++i )
    {
        int y = ( -3 + 2 * i ) * 250000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        VECTOR2I freeEnd( -800000, y );
        VECTOR2I connectedEnd( 0, y );
        VECTOR2I continuationEnd( 800000, y );

        PNS::SEGMENT* selected = addTestSegment( world, connectedEnd, freeEnd, 100000, net );

        addTestVia( world, connectedEnd, 300000, 120000, net );
        addTestSegment( world, connectedEnd, continuationEnd, 100000, net );

        startItems.push_back( selected );
        expectedStartAnchors.push_back( freeEnd );
    }

    return { startItems, VECTOR2I( -600000, 0 ), VECTOR2I( 2200000, 0 ), expectedStartAnchors };
}


static BUNDLE_TEST_SCENE buildWalkaroundTrackStubBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    BUNDLE_TEST_SCENE scene = buildTrackStubBundleScene( aFixture );
    PNS::NODE*        world = aFixture.m_router->GetWorld();
    auto              obstacleNet = reinterpret_cast<PNS::NET_HANDLE>( 99 );

    addTestSegment( world, VECTOR2I( 1000000, -900000 ), VECTOR2I( 1000000, 900000 ),
                    180000, obstacleNet );
    addTestSolid( world, BOX2I( VECTOR2I( 900000, -1200000 ), VECTOR2I( 250000, 450000 ) ),
                  obstacleNet );
    addTestSolid( world, BOX2I( VECTOR2I( 900000, 750000 ), VECTOR2I( 250000, 450000 ) ),
                  obstacleNet );

    return scene;
}


static BUNDLE_TEST_SCENE buildThroughTrackBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    std::vector<PNS::ITEM*> startItems;
    std::vector<VECTOR2I>   expectedStartAnchors;

    startItems.reserve( 4 );
    expectedStartAnchors.reserve( 4 );

    for( int i = 0; i < 4; ++i )
    {
        int y = ( -3 + 2 * i ) * 250000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        VECTOR2I leftConnected( -600000, y );
        VECTOR2I rightConnected( 600000, y );
        VECTOR2I farLeft( -1200000, y );
        VECTOR2I farRight( 1200000, y );

        PNS::SEGMENT* selected = addTestSegment( world, rightConnected, leftConnected, 100000, net );

        addTestVia( world, leftConnected, 300000, 120000, net );
        addTestVia( world, rightConnected, 300000, 120000, net );
        addTestSegment( world, leftConnected, farLeft, 100000, net );
        addTestSegment( world, rightConnected, farRight, 100000, net );

        startItems.push_back( selected );
        expectedStartAnchors.push_back( leftConnected );
    }

    return { startItems, VECTOR2I( -900000, 0 ), VECTOR2I( 2200000, 0 ), expectedStartAnchors };
}


static BUNDLE_TEST_SCENE buildShortSpineLargeBundleScene( PNS_TEST_FIXTURE& aFixture,
                                                          bool aRouteLeft )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    std::vector<PNS::ITEM*> startItems;
    startItems.reserve( 8 );

    for( int i = 0; i < 8; ++i )
    {
        int y = ( -7 + 2 * i ) * 250000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        startItems.push_back( addTestVia( world, VECTOR2I( 0, y ), 300000, 120000, net ) );
    }

    int dir = aRouteLeft ? -1 : 1;

    return { startItems, VECTOR2I( dir * 200000, 0 ), VECTOR2I( dir * 900000, 0 ) };
}


static BUNDLE_TEST_SCENE buildVerticalDetourBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    std::vector<PNS::ITEM*> startItems;
    startItems.reserve( 8 );

    for( int i = 0; i < 8; ++i )
    {
        int y = ( -7 + 2 * i ) * 250000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        startItems.push_back( addTestVia( world, VECTOR2I( 0, y ), 300000, 120000, net ) );
    }

    return { startItems, VECTOR2I( 250000, 0 ), VECTOR2I( 2200000, 450000 ) };
}


static BUNDLE_TEST_SCENE buildLargeVerticalBundleScene( PNS_TEST_FIXTURE& aFixture, int aCount )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 4000000 );

    std::vector<PNS::ITEM*> startItems;
    startItems.reserve( aCount );

    int center = aCount - 1;

    for( int i = 0; i < aCount; ++i )
    {
        int y = ( -center + 2 * i ) * 200000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        startItems.push_back( addTestVia( world, VECTOR2I( 0, y ), 300000, 120000, net ) );
    }

    return { startItems, VECTOR2I( 250000, 0 ), VECTOR2I( 5200000, 300000 ) };
}


static BUNDLE_TEST_SCENE buildHorizontalLargeBundleScene( PNS_TEST_FIXTURE& aFixture )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 2000000 );

    std::vector<PNS::ITEM*> startItems;
    startItems.reserve( 8 );

    for( int i = 0; i < 8; ++i )
    {
        int x = ( -7 + 2 * i ) * 250000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        startItems.push_back( addTestVia( world, VECTOR2I( x, 0 ), 300000, 120000, net ) );
    }

    return { startItems, VECTOR2I( 0, -250000 ), VECTOR2I( 2200000, 900000 ) };
}


static BUNDLE_TEST_SCENE buildLargeHorizontalBundleScene( PNS_TEST_FIXTURE& aFixture, int aCount )
{
    aFixture.ResetRouterWorld();
    aFixture.m_ruleResolver.m_defaultClearance = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Hole = 100000;
    aFixture.m_ruleResolver.m_defaultHole2Copper = 100000;

    PNS::NODE* world = aFixture.m_router->GetWorld();
    world->SetRuleResolver( &aFixture.m_ruleResolver );
    world->SetMaxClearance( 4000000 );

    std::vector<PNS::ITEM*> startItems;
    startItems.reserve( aCount );

    int center = aCount - 1;

    for( int i = 0; i < aCount; ++i )
    {
        int x = ( -center + 2 * i ) * 200000;
        auto net = reinterpret_cast<PNS::NET_HANDLE>( static_cast<intptr_t>( i + 1 ) );
        startItems.push_back( addTestVia( world, VECTOR2I( x, 0 ), 300000, 120000, net ) );
    }

    return { startItems, VECTOR2I( 0, -250000 ), VECTOR2I( 5200000, 1200000 ) };
}


static std::vector<VECTOR2I> bundleSceneStartAnchors( const BUNDLE_TEST_SCENE& aScene )
{
    if( !aScene.expectedStartAnchors.empty() )
        return aScene.expectedStartAnchors;

    std::vector<VECTOR2I> anchors;
    anchors.reserve( aScene.startItems.size() );

    for( PNS::ITEM* item : aScene.startItems )
        anchors.push_back( item->Anchor( 0 ) );

    return anchors;
}


static std::vector<VECTOR2I> bundlePreviewStarts( PNS::BUNDLE_PLACER& aPlacer )
{
    PNS::ITEM_SET           traces = aPlacer.Traces();
    std::vector<VECTOR2I>   starts;

    starts.reserve( traces.Size() );

    for( PNS::ITEM* item : traces.Items() )
    {
        PNS::LINE* line = static_cast<PNS::LINE*>( item );
        starts.push_back( line->CPoint( 0 ) );
    }

    return starts;
}


static std::vector<SHAPE_LINE_CHAIN> bundlePreviewLanes( PNS::BUNDLE_PLACER& aPlacer )
{
    PNS::ITEM_SET                   traces = aPlacer.Traces();
    std::vector<SHAPE_LINE_CHAIN>   lanes;

    lanes.reserve( traces.Size() );

    for( PNS::ITEM* item : traces.Items() )
        lanes.push_back( static_cast<PNS::LINE*>( item )->CLine() );

    return lanes;
}


static std::vector<VECTOR2I> bundlePartialFixEnds( PNS::BUNDLE_PLACER& aPlacer )
{
    PNS::ITEM_SET         traces = aPlacer.Traces();
    std::vector<VECTOR2I> ends;

    ends.reserve( traces.Size() );

    for( PNS::ITEM* item : traces.Items() )
    {
        PNS::LINE*        line = static_cast<PNS::LINE*>( item );
        SHAPE_LINE_CHAIN  trimmed = line->CLine();

        if( trimmed.SegmentCount() > 1 )
            trimmed.Remove( -1, -1 );

        ends.push_back( trimmed.CLastPoint() );
    }

    return ends;
}


static void checkBundlePreviewStarts( PNS::BUNDLE_PLACER& aPlacer,
                                      const std::vector<VECTOR2I>& aExpectedStarts )
{
    std::vector<VECTOR2I> actualStarts = bundlePreviewStarts( aPlacer );

    BOOST_REQUIRE_EQUAL( actualStarts.size(), aExpectedStarts.size() );

    for( size_t i = 0; i < aExpectedStarts.size(); ++i )
        BOOST_CHECK_EQUAL( actualStarts[i], aExpectedStarts[i] );
}


static void bundlePreviewStartsEqual( PNS::BUNDLE_PLACER& aPlacer,
                                      const std::vector<VECTOR2I>& aExpectedStarts )
{
    checkBundlePreviewStarts( aPlacer, aExpectedStarts );
}


static bool bundlePreviewStartsDoNotEqualAnchorZero( const std::vector<PNS::ITEM*>& aStartItems,
                                                     const std::vector<VECTOR2I>& aActualStarts )
{
    if( aStartItems.size() != aActualStarts.size() )
        return false;

    for( size_t i = 0; i < aStartItems.size(); ++i )
    {
        if( !aStartItems[i] || aStartItems[i]->AnchorCount() < 1 )
            return false;

        if( aActualStarts[i] == aStartItems[i]->Anchor( 0 ) )
            return false;
    }

    return true;
}


static void sortBundleLanesByStart( std::vector<SHAPE_LINE_CHAIN>& aLanes )
{
    std::sort( aLanes.begin(), aLanes.end(), []( const SHAPE_LINE_CHAIN& aA,
                                                 const SHAPE_LINE_CHAIN& aB )
    {
        const VECTOR2I& startA = aA.CPoint( 0 );
        const VECTOR2I& startB = aB.CPoint( 0 );

        if( startA.y != startB.y )
            return startA.y < startB.y;

        if( startA.x != startB.x )
            return startA.x < startB.x;

        if( aA.PointCount() != aB.PointCount() )
            return aA.PointCount() < aB.PointCount();

        if( aA.CLastPoint().x != aB.CLastPoint().x )
            return aA.CLastPoint().x < aB.CLastPoint().x;

        return aA.CLastPoint().y < aB.CLastPoint().y;
    } );
}


static SHAPE_LINE_CHAIN mirrorLineChainAcrossVerticalAxis( const SHAPE_LINE_CHAIN& aLine, int aAxisX )
{
    SHAPE_LINE_CHAIN mirrored( aLine );

    for( int i = 0; i < mirrored.PointCount(); ++i )
    {
        VECTOR2I point = mirrored.CPoint( i );
        mirrored.SetPoint( i, VECTOR2I( 2 * aAxisX - point.x, point.y ) );
    }

    if( mirrored.PointCount() > 2 )
        mirrored.Simplify();

    return mirrored;
}


static void checkBundlePreviewMatches( std::vector<SHAPE_LINE_CHAIN> aExpected,
                                       std::vector<SHAPE_LINE_CHAIN> aActual )
{
    for( SHAPE_LINE_CHAIN& lane : aExpected )
    {
        if( lane.PointCount() > 2 )
            lane.Simplify();
    }

    for( SHAPE_LINE_CHAIN& lane : aActual )
    {
        if( lane.PointCount() > 2 )
            lane.Simplify();
    }

    sortBundleLanesByStart( aExpected );
    sortBundleLanesByStart( aActual );

    BOOST_REQUIRE_EQUAL( aExpected.size(), aActual.size() );

    for( size_t i = 0; i < aExpected.size(); ++i )
    {
        BOOST_REQUIRE_EQUAL( aExpected[i].PointCount(), aActual[i].PointCount() );

        for( int j = 0; j < aExpected[i].PointCount(); ++j )
            BOOST_CHECK_EQUAL( aExpected[i].CPoint( j ), aActual[i].CPoint( j ) );
    }
}


static void checkMirroredBundlePreviewMatches( const std::vector<SHAPE_LINE_CHAIN>& aRightLanes,
                                               const std::vector<SHAPE_LINE_CHAIN>& aLeftLanes,
                                               int aMirrorAxisX )
{
    std::vector<SHAPE_LINE_CHAIN> mirroredLeft;

    mirroredLeft.reserve( aLeftLanes.size() );

    for( const SHAPE_LINE_CHAIN& lane : aLeftLanes )
        mirroredLeft.push_back( mirrorLineChainAcrossVerticalAxis( lane, aMirrorAxisX ) );

    checkBundlePreviewMatches( aRightLanes, mirroredLeft );
}


static bool laneHasBackwardSegments( const SHAPE_LINE_CHAIN& aLane, const VECTOR2I& aRoutingDir,
                                     bool aFirstSegmentOnly = false )
{
    int segmentCount = aLane.SegmentCount();

    if( aFirstSegmentOnly )
        segmentCount = std::min( segmentCount, 1 );

    for( int i = 0; i < segmentCount; ++i )
    {
        SEG segment = aLane.CSegment( i );
        VECTOR2I delta = segment.B - segment.A;
        int64_t dot = (int64_t) delta.x * aRoutingDir.x + (int64_t) delta.y * aRoutingDir.y;

        if( dot < 0 )
            return true;
    }

    return false;
}


static bool bundlePreviewHasBackwardSegments( const std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                              const VECTOR2I& aRoutingDir )
{
    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( laneHasBackwardSegments( lane, aRoutingDir ) )
            return true;
    }

    return false;
}


static bool bundlePreviewHasBackwardFirstSegments( const std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                                   const VECTOR2I& aRoutingDir )
{
    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( laneHasBackwardSegments( lane, aRoutingDir, true ) )
            return true;
    }

    return false;
}


static bool bundlePreviewAllHaveSegments( const std::vector<SHAPE_LINE_CHAIN>& aLanes )
{
    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( lane.SegmentCount() < 1 )
            return false;
    }

    return true;
}


static bool bundlePreviewStartsMatchActualAnchors( const std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                                   const std::vector<VECTOR2I>& aAnchors )
{
    std::vector<VECTOR2I> remaining = aAnchors;

    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( lane.PointCount() < 1 )
            return false;

        auto it = std::find( remaining.begin(), remaining.end(), lane.CPoint( 0 ) );

        if( it == remaining.end() )
            return false;

        remaining.erase( it );
    }

    return remaining.empty();
}


static bool bundlePreviewHasSharedForeignOrigin( const std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                                 const std::vector<VECTOR2I>& aAnchors )
{
    if( aLanes.empty() )
        return false;

    const VECTOR2I& origin = aLanes.front().CPoint( 0 );

    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( lane.PointCount() < 1 || lane.CPoint( 0 ) != origin )
            return false;
    }

    return std::find( aAnchors.begin(), aAnchors.end(), origin ) == aAnchors.end();
}


static void bundlePreviewEqualsCachedGeometry( const std::vector<SHAPE_LINE_CHAIN>& aExpected,
                                               const std::vector<SHAPE_LINE_CHAIN>& aActual )
{
    checkBundlePreviewMatches( aExpected, aActual );
}


static bool bundlePreviewFirstSegmentsAllParallelToAxis( const std::vector<SHAPE_LINE_CHAIN>& aLanes,
                                                         const VECTOR2I& aAxis )
{
    for( const SHAPE_LINE_CHAIN& lane : aLanes )
    {
        if( lane.SegmentCount() < 1 )
            return false;

        VECTOR2I delta = lane.CSegment( 0 ).B - lane.CSegment( 0 ).A;
        int64_t  cross = (int64_t) delta.x * aAxis.y - (int64_t) delta.y * aAxis.x;

        if( cross != 0 )
            return false;
    }

    return true;
}

BOOST_FIXTURE_TEST_CASE( PNSHoleCollisions, PNS_TEST_FIXTURE )
{
    PNS::VIA* v1 = new PNS::VIA( VECTOR2I( 0, 1000000 ), PNS_LAYER_RANGE( F_Cu, B_Cu ), 50000, 10000 );
    PNS::VIA* v2 = new PNS::VIA( VECTOR2I( 0, 2000000 ), PNS_LAYER_RANGE( F_Cu, B_Cu ), 50000, 10000 );

    std::unique_ptr<PNS::NODE> world ( new PNS::NODE );

    v1->SetNet( (PNS::NET_HANDLE) 1 );
    v2->SetNet( (PNS::NET_HANDLE) 2 );

    world->SetMaxClearance( 10000000 );
    world->SetRuleResolver( &m_ruleResolver );

    world->AddRaw( v1 );
    world->AddRaw( v2 );

    BOOST_TEST_MESSAGE( "via to via, no violations" );
    {
        PNS::NODE::OBSTACLES obstacles;
        int count = world->QueryColliding( v1, obstacles );
        dumpObstacles( obstacles );
        BOOST_CHECK_EQUAL( obstacles.size(), 0 );
        BOOST_CHECK_EQUAL( count, 0 );
    }

    BOOST_TEST_MESSAGE( "via to via, forced copper to copper violation" );
    {
        PNS::NODE::OBSTACLES obstacles;
        m_ruleResolver.m_defaultClearance = 1000000;
        world->QueryColliding( v1, obstacles );
        dumpObstacles( obstacles );

        BOOST_CHECK_EQUAL( obstacles.size(), 1 );
        const auto& first = *obstacles.begin();

        BOOST_CHECK_EQUAL( first.m_head, v1 );
        BOOST_CHECK_EQUAL( first.m_item, v2 );
        BOOST_CHECK_EQUAL( first.m_clearance, m_ruleResolver.m_defaultClearance );
    }

    BOOST_TEST_MESSAGE( "via to via, forced hole to hole violation" );
    {
        PNS::NODE::OBSTACLES obstacles;
        m_ruleResolver.m_defaultClearance = 200000;
        m_ruleResolver.m_defaultHole2Hole = 1000000;

        world->QueryColliding( v1, obstacles );
        dumpObstacles( obstacles );

        BOOST_CHECK_EQUAL( obstacles.size(), 1 );
        auto iter = obstacles.begin();
        const auto& first = *iter++;

        BOOST_CHECK_EQUAL( first.m_head, v1->Hole() );
        BOOST_CHECK_EQUAL( first.m_item, v2->Hole() );
        BOOST_CHECK_EQUAL( first.m_clearance, m_ruleResolver.m_defaultHole2Hole );
    }

    BOOST_TEST_MESSAGE( "via to via, forced copper to hole violation" );
    {
        PNS::NODE::OBSTACLES obstacles;
        m_ruleResolver.m_defaultHole2Hole = 220000;
        m_ruleResolver.m_defaultHole2Copper = 1000000;

        world->QueryColliding( v1, obstacles );
        dumpObstacles( obstacles );

        BOOST_CHECK_EQUAL( obstacles.size(), 2 );
        auto iter = obstacles.begin();
        const auto& first = *iter++;

        // There is no guarantee on what order the two collisions will be in...
        BOOST_CHECK( ( first.m_head == v1 && first.m_item == v2->Hole() )
                  || ( first.m_head == v1->Hole() && first.m_item == v2 ) );

        BOOST_CHECK_EQUAL( first.m_clearance, m_ruleResolver.m_defaultHole2Copper );
    }
}


BOOST_FIXTURE_TEST_CASE( PNSViaBackdrillRetention, PNS_TEST_FIXTURE )
{
    PNS::VIA via( VECTOR2I( 1000, 2000 ), PNS_LAYER_RANGE( F_Cu, B_Cu ), 40000, 20000, nullptr,
                  VIATYPE::THROUGH );
    via.SetHoleLayers( PNS_LAYER_RANGE( F_Cu, In2_Cu ) );
    via.SetHolePostMachining( std::optional<PAD_DRILL_POST_MACHINING_MODE>( PAD_DRILL_POST_MACHINING_MODE::COUNTERSINK ) );
    via.SetSecondaryDrill( std::optional<int>( 12000 ) );
    via.SetSecondaryHoleLayers( std::optional<PNS_LAYER_RANGE>( PNS_LAYER_RANGE( F_Cu, In1_Cu ) ) );
    via.SetSecondaryHolePostMachining( std::optional<PAD_DRILL_POST_MACHINING_MODE>( PAD_DRILL_POST_MACHINING_MODE::NOT_POST_MACHINED ) );

    PNS::VIA viaCopy( via );
    std::unique_ptr<PNS::VIA> viaClone( via.Clone() );

    auto checkVia = [&]( const PNS::VIA& candidate )
    {
        BOOST_CHECK_EQUAL( candidate.HoleLayers().Start(), via.HoleLayers().Start() );
        BOOST_CHECK_EQUAL( candidate.HoleLayers().End(), via.HoleLayers().End() );
        BOOST_CHECK( candidate.HolePostMachining().has_value() );
        BOOST_CHECK( candidate.HolePostMachining().value() == PAD_DRILL_POST_MACHINING_MODE::COUNTERSINK );
        BOOST_CHECK( candidate.SecondaryDrill().has_value() );
        BOOST_CHECK_EQUAL( candidate.SecondaryDrill().value(), via.SecondaryDrill().value() );
        BOOST_CHECK( candidate.SecondaryHoleLayers().has_value() );
        BOOST_CHECK_EQUAL( candidate.SecondaryHoleLayers()->Start(),
                           via.SecondaryHoleLayers()->Start() );
        BOOST_CHECK_EQUAL( candidate.SecondaryHoleLayers()->End(),
                           via.SecondaryHoleLayers()->End() );
        BOOST_CHECK( candidate.SecondaryHolePostMachining().has_value() );

        // run this BOOST_CHECK only if possible to avoid crash
        if( candidate.SecondaryHolePostMachining().has_value() )
            BOOST_CHECK( candidate.SecondaryHolePostMachining().value() == via.SecondaryHolePostMachining().value() );
    };

    checkVia( viaCopy );
    checkVia( *viaClone );
}


BOOST_AUTO_TEST_CASE( PCBViaBackdrillCloneRetainsData )
{
    BOARD board;
    PCB_VIA via( &board );

    via.SetPrimaryDrillStartLayer( F_Cu );
    via.SetPrimaryDrillEndLayer( B_Cu );
    via.SetFrontPostMachining( std::optional<PAD_DRILL_POST_MACHINING_MODE>( PAD_DRILL_POST_MACHINING_MODE::COUNTERSINK ) );
    via.SetSecondaryDrillSize( std::optional<int>( 15000 ) );
    via.SetSecondaryDrillStartLayer( F_Cu );
    via.SetSecondaryDrillEndLayer( In2_Cu );

    via.SetBackPostMachining( std::optional<PAD_DRILL_POST_MACHINING_MODE>( PAD_DRILL_POST_MACHINING_MODE::COUNTERBORE ) );
    via.SetTertiaryDrillSize( std::optional<int>( 8000 ) );
    via.SetTertiaryDrillStartLayer( B_Cu );
    via.SetTertiaryDrillEndLayer( In4_Cu );

    PCB_VIA viaCopy( via );
    std::unique_ptr<PCB_VIA> viaClone( static_cast<PCB_VIA*>( via.Clone() ) );

    auto checkVia = [&]( const PCB_VIA& candidate )
    {
        BOOST_CHECK_EQUAL( candidate.GetPrimaryDrillStartLayer(), via.GetPrimaryDrillStartLayer() );
        BOOST_CHECK_EQUAL( candidate.GetPrimaryDrillEndLayer(), via.GetPrimaryDrillEndLayer() );
        BOOST_CHECK( candidate.GetFrontPostMachining().has_value() );
        BOOST_CHECK_EQUAL( static_cast<int>( candidate.GetFrontPostMachining().value() ),
                           static_cast<int>( via.GetFrontPostMachining().value() ) );
        BOOST_CHECK( candidate.GetSecondaryDrillSize().has_value() );
        BOOST_CHECK_EQUAL( candidate.GetSecondaryDrillSize().value(),
                           via.GetSecondaryDrillSize().value() );
        BOOST_CHECK_EQUAL( candidate.GetSecondaryDrillStartLayer(),
                           via.GetSecondaryDrillStartLayer() );
        BOOST_CHECK_EQUAL( candidate.GetSecondaryDrillEndLayer(),
                           via.GetSecondaryDrillEndLayer() );

        BOOST_CHECK( candidate.GetBackPostMachining().has_value() );
        BOOST_CHECK_EQUAL( static_cast<int>( candidate.GetBackPostMachining().value() ),
                           static_cast<int>( via.GetBackPostMachining().value() ) );
        BOOST_CHECK( candidate.GetTertiaryDrillSize().has_value() );
        BOOST_CHECK_EQUAL( candidate.GetTertiaryDrillSize().value(),
                           via.GetTertiaryDrillSize().value() );
        BOOST_CHECK_EQUAL( candidate.GetTertiaryDrillStartLayer(),
                           via.GetTertiaryDrillStartLayer() );
        BOOST_CHECK_EQUAL( candidate.GetTertiaryDrillEndLayer(),
                           via.GetTertiaryDrillEndLayer() );
    };

    checkVia( viaCopy );
    checkVia( *viaClone );
}


/**
 * Test that PNS_LAYER_RANGE(1, 0) is swapped to (0, 1).
 *
 * This is a minimal regression test for https://gitlab.com/kicad/code/kicad/-/issues/20355
 * The actual fix is in pns_kicad_iface.cpp syncPad() which skips creating an
 * INNER_LAYERS SOLID on 2-layer boards. This test verifies the layer range behavior
 * that motivated the fix.
 */
BOOST_AUTO_TEST_CASE( PNSLayerRangeSwapBehavior )
{
    // On a 2-layer board with FRONT_INNER_BACK mode, BoardCopperLayerCount() returns 2.
    // The code would calculate PNS_LAYER_RANGE(1, 2 - 2) = PNS_LAYER_RANGE(1, 0)
    // Since start > end, the constructor swaps them to (0, 1), which would span
    // both F_Cu and B_Cu incorrectly.

    PNS_LAYER_RANGE innerLayersRange2Layer( 1, 0 );  // What would happen on 2-layer board

    // Verify the swap behavior that causes the bug
    BOOST_CHECK_EQUAL( innerLayersRange2Layer.Start(), 0 );
    BOOST_CHECK_EQUAL( innerLayersRange2Layer.End(), 1 );
    BOOST_CHECK( innerLayersRange2Layer.Overlaps( 0 ) );  // F_Cu
    BOOST_CHECK( innerLayersRange2Layer.Overlaps( 1 ) );  // B_Cu

    // On a 4-layer board, inner layers are 1 and 2, so PNS_LAYER_RANGE(1, 4-2) = (1, 2)
    PNS_LAYER_RANGE innerLayersRange4Layer( 1, 2 );  // Correct for 4-layer board

    BOOST_CHECK_EQUAL( innerLayersRange4Layer.Start(), 1 );
    BOOST_CHECK_EQUAL( innerLayersRange4Layer.End(), 2 );
    BOOST_CHECK( !innerLayersRange4Layer.Overlaps( 0 ) ); // F_Cu - should not overlap
    BOOST_CHECK( innerLayersRange4Layer.Overlaps( 1 ) );  // In1_Cu
    BOOST_CHECK( innerLayersRange4Layer.Overlaps( 2 ) );  // In2_Cu
    BOOST_CHECK( !innerLayersRange4Layer.Overlaps( 3 ) ); // B_Cu - should not overlap
}


BOOST_FIXTURE_TEST_CASE( BundleShoveModeDoesNotFallBackToWalkaround, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes();

    m_router->UpdateSizes( sizes );

    {
        BUNDLE_TEST_SCENE walkScene = buildBlockedBundleScene( *this );
        PNS::BUNDLE_PLACER walkPlacer( m_router );

        walkPlacer.UpdateSizes( sizes );
        BOOST_REQUIRE( walkPlacer.SetLayer( F_Cu ) );
        walkPlacer.SetStartPads( walkScene.startItems );

        m_routerSettings.SetMode( PNS::RM_Walkaround );
        BOOST_REQUIRE( walkPlacer.Start( walkScene.startPoint, walkScene.startItems.front() ) );
        BOOST_CHECK( walkPlacer.Move( walkScene.targetPoint, nullptr ) );
        BOOST_CHECK( !bundleTraceHasCollision( walkPlacer ) );
    }

    {
        BUNDLE_TEST_SCENE shoveScene = buildBlockedBundleScene( *this );
        PNS::BUNDLE_PLACER shovePlacer( m_router );

        shovePlacer.UpdateSizes( sizes );
        BOOST_REQUIRE( shovePlacer.SetLayer( F_Cu ) );
        shovePlacer.SetStartPads( shoveScene.startItems );

        m_routerSettings.SetMode( PNS::RM_MarkObstacles );
        BOOST_REQUIRE( shovePlacer.Start( shoveScene.startPoint, shoveScene.startItems.front() ) );
        shovePlacer.Move( shoveScene.targetPoint, nullptr );

        m_routerSettings.SetMode( PNS::RM_Shove );
        BOOST_CHECK( !shovePlacer.Move( shoveScene.targetPoint, nullptr ) );
        BOOST_CHECK( bundleTraceHasCollision( shovePlacer ) );
    }
}


BOOST_FIXTURE_TEST_CASE( BundleWalkaroundPreservesOffsetStartAnchors, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes();
    BUNDLE_TEST_SCENE   scene = buildOffsetAnchorBundleScene( *this );
    std::vector<VECTOR2I> expectedStarts = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );
    checkBundlePreviewStarts( placer, expectedStarts );
    BOOST_CHECK( !bundleTraceHasCollision( placer ) );
}


BOOST_FIXTURE_TEST_CASE( BundleShovePreservesOffsetStartAnchors, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes();
    BUNDLE_TEST_SCENE   scene = buildOffsetAnchorBundleScene( *this );
    std::vector<VECTOR2I> expectedStarts = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );

    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    m_routerSettings.SetMode( PNS::RM_Shove );
    BOOST_CHECK( !placer.Move( scene.targetPoint, nullptr ) );
    checkBundlePreviewStarts( placer, expectedStarts );
    BOOST_CHECK( bundleTraceHasCollision( placer ) );
}


BOOST_FIXTURE_TEST_CASE( BundleWalkaroundChainedPlacementStartsFromCommittedEnds, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes();
    BUNDLE_TEST_SCENE   scene = buildOffsetAnchorBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_REQUIRE( placer.Move( scene.targetPoint, nullptr ) );

    std::vector<VECTOR2I> expectedNextStarts = bundlePartialFixEnds( placer );
    VECTOR2I secondTarget = scene.targetPoint + VECTOR2I( 1500000, 1200000 );

    BOOST_CHECK( !placer.FixRoute( scene.targetPoint, nullptr, false ) );
    BOOST_REQUIRE( placer.Move( secondTarget, nullptr ) );
    checkBundlePreviewStarts( placer, expectedNextStarts );
}


BOOST_FIXTURE_TEST_CASE( BundleTrackStartsAtDanglingEndsMarkObstacles, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 4 );
    BUNDLE_TEST_SCENE   scene = buildTrackStubBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    bundlePreviewStartsEqual( placer, bundleSceneStartAnchors( scene ) );
    BOOST_CHECK( bundlePreviewStartsDoNotEqualAnchorZero( scene.startItems,
                                                          bundlePreviewStarts( placer ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleTrackStartsAtDanglingEndsWalkaround, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 4 );
    BUNDLE_TEST_SCENE   scene = buildWalkaroundTrackStubBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    bundlePreviewStartsEqual( placer, bundleSceneStartAnchors( scene ) );
    BOOST_CHECK( bundlePreviewStartsDoNotEqualAnchorZero( scene.startItems,
                                                          bundlePreviewStarts( placer ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleTrackBothEndsConnectedUsesCursorSide, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 4 );
    BUNDLE_TEST_SCENE   scene = buildThroughTrackBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    bundlePreviewStartsEqual( placer, bundleSceneStartAnchors( scene ) );
    BOOST_CHECK( bundlePreviewStartsDoNotEqualAnchorZero( scene.startItems,
                                                          bundlePreviewStarts( placer ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleVerticalArrayMirrorSymmetryAcrossCursorSwingMarkObstacles,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildShortSpineLargeBundleScene( *this, false );
    int                 mirrorAxisX = PNS::BUNDLE_PRIMITIVE_GROUP( scene.startItems ).Centroid().x;
    VECTOR2I            mirroredLeftTarget( 2 * mirrorAxisX - scene.targetPoint.x, scene.targetPoint.y );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> rightLanes = bundlePreviewLanes( placer );

    BOOST_CHECK( placer.Move( mirroredLeftTarget, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> leftLanes = bundlePreviewLanes( placer );

    checkMirroredBundlePreviewMatches( rightLanes, leftLanes, mirrorAxisX );
}


BOOST_FIXTURE_TEST_CASE( BundleVerticalArrayNoBackwardSegmentsRightTargetMarkObstacles,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildVerticalDetourBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    BOOST_CHECK( !bundlePreviewHasBackwardSegments( bundlePreviewLanes( placer ), VECTOR2I( 1, 0 ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleHorizontalArrayDoesNotLockToStartAxisMarkObstacles,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildHorizontalLargeBundleScene( *this );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    std::vector<SHAPE_LINE_CHAIN> lanes = bundlePreviewLanes( placer );

    BOOST_CHECK( bundlePreviewAllHaveSegments( lanes ) );
    BOOST_CHECK( !bundlePreviewFirstSegmentsAllParallelToAxis( lanes, VECTOR2I( 1, 0 ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleHorizontalArrayWalkaroundMatchesBaseGeometry, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildHorizontalLargeBundleScene( *this );
    PNS::BUNDLE_PLACER  markPlacer( m_router );

    m_router->UpdateSizes( sizes );
    markPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( markPlacer.SetLayer( F_Cu ) );
    markPlacer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( markPlacer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( markPlacer.Move( scene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> markLanes = bundlePreviewLanes( markPlacer );

    scene = buildHorizontalLargeBundleScene( *this );
    PNS::BUNDLE_PLACER walkPlacer( m_router );

    m_router->UpdateSizes( sizes );
    walkPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( walkPlacer.SetLayer( F_Cu ) );
    walkPlacer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( walkPlacer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( walkPlacer.Move( scene.targetPoint, nullptr ) );

    checkBundlePreviewMatches( markLanes, bundlePreviewLanes( walkPlacer ) );
}


BOOST_FIXTURE_TEST_CASE( BundleLargeVerticalPreviewStartsAtAnchorsMarkObstacles,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 12 );
    BUNDLE_TEST_SCENE   scene = buildLargeVerticalBundleScene( *this, 12 );
    std::vector<VECTOR2I> anchors = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    std::vector<SHAPE_LINE_CHAIN> lanes = bundlePreviewLanes( placer );
    BOOST_CHECK( bundlePreviewStartsMatchActualAnchors( lanes, anchors ) );
    BOOST_CHECK( !bundlePreviewHasSharedForeignOrigin( lanes, anchors ) );
}


BOOST_FIXTURE_TEST_CASE( BundleLargeHorizontalPreviewStartsAtAnchorsMarkObstacles,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 12 );
    BUNDLE_TEST_SCENE   scene = buildLargeHorizontalBundleScene( *this, 12 );
    std::vector<VECTOR2I> anchors = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    std::vector<SHAPE_LINE_CHAIN> lanes = bundlePreviewLanes( placer );
    BOOST_CHECK( bundlePreviewStartsMatchActualAnchors( lanes, anchors ) );
    BOOST_CHECK( !bundlePreviewHasSharedForeignOrigin( lanes, anchors ) );
}


BOOST_FIXTURE_TEST_CASE( BundleLargePreviewDoesNotUseCentroidFallbackWalkaround,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 12 );
    BUNDLE_TEST_SCENE   scene = buildLargeVerticalBundleScene( *this, 12 );
    std::vector<VECTOR2I> anchors = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    std::vector<SHAPE_LINE_CHAIN> lanes = bundlePreviewLanes( placer );
    BOOST_CHECK( bundlePreviewStartsMatchActualAnchors( lanes, anchors ) );
    BOOST_CHECK( !bundlePreviewHasSharedForeignOrigin( lanes, anchors ) );
}


BOOST_FIXTURE_TEST_CASE( BundleLargePreviewFreezesLastValidAnchoredGeometry,
                         PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 12 );
    BUNDLE_TEST_SCENE   scene = buildLargeVerticalBundleScene( *this, 12 );
    PNS::BUNDLE_PLACER  placer( m_router );
    VECTOR2I            invalidTarget( 150000, 0 );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> validLanes = bundlePreviewLanes( placer );

    BOOST_CHECK( !placer.Move( invalidTarget, nullptr ) );
    bundlePreviewEqualsCachedGeometry( validLanes, bundlePreviewLanes( placer ) );
}


BOOST_FIXTURE_TEST_CASE( BundleLargeInvalidFirstMoveStillAnchored, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 12 );
    BUNDLE_TEST_SCENE   scene = buildLargeVerticalBundleScene( *this, 12 );
    std::vector<VECTOR2I> anchors = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER  placer( m_router );
    VECTOR2I            shortTarget( 150000, 0 );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    placer.Move( shortTarget, nullptr );

    std::vector<SHAPE_LINE_CHAIN> lanes = bundlePreviewLanes( placer );
    BOOST_CHECK( bundlePreviewStartsMatchActualAnchors( lanes, anchors ) );
    BOOST_CHECK( !bundlePreviewHasSharedForeignOrigin( lanes, anchors ) );
}


BOOST_FIXTURE_TEST_CASE( BundleShortSpineMirrorSymmetryMarkObstacles, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   rightScene = buildShortSpineLargeBundleScene( *this, false );
    int                 mirrorAxisX = PNS::BUNDLE_PRIMITIVE_GROUP( rightScene.startItems ).Centroid().x;
    PNS::BUNDLE_PLACER  rightPlacer( m_router );

    m_router->UpdateSizes( sizes );
    rightPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( rightPlacer.SetLayer( F_Cu ) );
    rightPlacer.SetStartPads( rightScene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( rightPlacer.Start( rightScene.startPoint, rightScene.startItems.front() ) );
    BOOST_CHECK( rightPlacer.Move( rightScene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> rightLanes = bundlePreviewLanes( rightPlacer );

    BUNDLE_TEST_SCENE  leftScene = buildShortSpineLargeBundleScene( *this, true );
    PNS::BUNDLE_PLACER leftPlacer( m_router );

    m_router->UpdateSizes( sizes );
    leftPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( leftPlacer.SetLayer( F_Cu ) );
    leftPlacer.SetStartPads( leftScene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( leftPlacer.Start( leftScene.startPoint, leftScene.startItems.front() ) );
    BOOST_CHECK( leftPlacer.Move( leftScene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> leftLanes = bundlePreviewLanes( leftPlacer );

    checkMirroredBundlePreviewMatches( rightLanes, leftLanes, mirrorAxisX );
}


BOOST_FIXTURE_TEST_CASE( BundleShortSpineNoBackwardSegmentsMarkObstacles, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildShortSpineLargeBundleScene( *this, true );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( placer.Move( scene.targetPoint, nullptr ) );

    BOOST_CHECK( !bundlePreviewHasBackwardSegments( bundlePreviewLanes( placer ), VECTOR2I( -1, 0 ) ) );
}


BOOST_FIXTURE_TEST_CASE( BundleShortSpineMirrorSymmetryWalkaround, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes( 8 );
    BUNDLE_TEST_SCENE   scene = buildShortSpineLargeBundleScene( *this, false );
    PNS::BUNDLE_PLACER  markPlacer( m_router );

    m_router->UpdateSizes( sizes );
    markPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( markPlacer.SetLayer( F_Cu ) );
    markPlacer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( markPlacer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( markPlacer.Move( scene.targetPoint, nullptr ) );
    std::vector<SHAPE_LINE_CHAIN> markLanes = bundlePreviewLanes( markPlacer );

    scene = buildShortSpineLargeBundleScene( *this, false );
    PNS::BUNDLE_PLACER walkPlacer( m_router );

    m_router->UpdateSizes( sizes );
    walkPlacer.UpdateSizes( sizes );
    BOOST_REQUIRE( walkPlacer.SetLayer( F_Cu ) );
    walkPlacer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_Walkaround );
    BOOST_REQUIRE( walkPlacer.Start( scene.startPoint, scene.startItems.front() ) );
    BOOST_CHECK( walkPlacer.Move( scene.targetPoint, nullptr ) );

    checkBundlePreviewMatches( markLanes, bundlePreviewLanes( walkPlacer ) );
}


BOOST_FIXTURE_TEST_CASE( BundleShoveRejectsDetachedModifiedHeads, PNS_TEST_FIXTURE )
{
    PNS::SIZES_SETTINGS sizes = makeBundleTestSizes();
    BUNDLE_TEST_SCENE   scene = buildBlockedBundleScene( *this );
    std::vector<VECTOR2I> expectedStarts = bundleSceneStartAnchors( scene );
    PNS::BUNDLE_PLACER  placer( m_router );

    m_router->UpdateSizes( sizes );
    placer.UpdateSizes( sizes );
    BOOST_REQUIRE( placer.SetLayer( F_Cu ) );
    placer.SetStartPads( scene.startItems );

    m_routerSettings.SetMode( PNS::RM_MarkObstacles );
    BOOST_REQUIRE( placer.Start( scene.startPoint, scene.startItems.front() ) );
    placer.Move( scene.targetPoint, nullptr );

    m_routerSettings.SetMode( PNS::RM_Shove );
    placer.Move( scene.targetPoint, nullptr );

    checkBundlePreviewStarts( placer, expectedStarts );
    BOOST_CHECK( !bundlePreviewHasBackwardFirstSegments( bundlePreviewLanes( placer ),
                                                         VECTOR2I( 1, 0 ) ) );
}

