// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef __ROUTER_PREVIEW_ITEM_H
#define __ROUTER_PREVIEW_ITEM_H

#include <cstdio>
#include <QString>

#include <view/view.h>
#include <view/view_item.h>
#include <view/view_group.h>

#include <math/vector2d.h>
#include <math/box2.h>

#include <geometry/shape_line_chain.h>
#include <geometry/shape_circle.h>

#include <gal/color4d.h>

#include <eda_item.h>

namespace PNS {

class ITEM;
class ROUTER;
class ROUTER_IFACE;

}

#define PNS_HEAD_TRACE 1
#define PNS_HOVER_ITEM 2
#define PNS_SEMI_SOLID 4
#define PNS_COLLISION  8


class ROUTER_PREVIEW_ITEM : public EDA_ITEM
{
public:
    enum ITEM_TYPE
    {
        PR_STUCK_MARKER = 0,
        PR_POINT,
        PR_SHAPE
    };

    /**
     * We draw this item on a single layer, but we stack up all the layers from
     * the various components that form this preview item.  In order to make this
     * work, we need to map that layer stack onto fractional depths that are less
     * than 1.0 so that this preview item doesn't draw on top of other overlays
     * that are in front of it.
     *
     * This factor is chosen to be fairly small so that we can fit an entire
     * GAL layer stack into the space of one view group sublayer (which is
     * currently hard-coded via GAL::AdvanceDepth take a depth of 0.1)
     */
    static constexpr double LayerDepthFactor = 0.001;
    static constexpr double PathOverlayDepth = LayerDepthFactor * static_cast<double>( LAYER_ZONE_END );

    ROUTER_PREVIEW_ITEM( const SHAPE& aShape, PNS::ROUTER_IFACE* aIface, KIGFX::VIEW* aView );
    ROUTER_PREVIEW_ITEM( const PNS::ITEM* aItem, PNS::ROUTER_IFACE* aIface, KIGFX::VIEW* aView,
                         int aFlags = 0 );
    ~ROUTER_PREVIEW_ITEM();

    void Update( const PNS::ITEM* aItem );

    void SetColor( const KIGFX::COLOR4D& aColor ) { m_color = aColor; }
    void SetDepth( double aDepth ) { m_depth = aDepth; }
    void SetWidth( int aWidth ) { m_width = aWidth; }

    void SetClearance( int aClearance ) { m_clearance = aClearance; }
    void ShowClearance( bool aEnabled ) { m_showClearance = aEnabled; }

    double GetOriginDepth() const { return m_originDepth; }

#if defined(DEBUG)
    void Show( int aA, std::ostream& aB ) const override {}
#endif

    virtual QString GetClass() const override
    {
        return "ROUTER_PREVIEW_ITEM";
    }

    const BOX2I ViewBBox() const override;

    virtual void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override;

    virtual std::vector<int> ViewGetLayers() const override
    {
        return { m_layer };
    }

    void drawLineChain( const SHAPE_LINE_CHAIN_BASE* aL, KIGFX::GAL* aGal ) const;

    void drawShape( const SHAPE* aShape, KIGFX::GAL* aGal ) const;

private:
    const KIGFX::COLOR4D getLayerColor( int aLayer ) const;

private:
    KIGFX::VIEW*   m_view;

    PNS::ROUTER_IFACE* m_iface;

    SHAPE*         m_shape;
    SHAPE*         m_hole;

    ITEM_TYPE      m_type;

    int            m_flags;
    int            m_width;
    int            m_layer;
    int            m_originLayer;
    int            m_clearance;
    bool           m_showClearance;

    double         m_originDepth;
    double         m_depth;

    KIGFX::COLOR4D m_color;
    VECTOR2I       m_pos;
};

#endif
