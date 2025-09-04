#pragma once

#include <bitset>
#include <QVector>
#include <limits>
#include <QString>

#include <gal/gal.h>
#include <math/box2.h>
#include <inspectable.h>

#if defined( _MSC_VER )
#pragma warning( push )
#pragma warning( disable : 4275 )
#endif

namespace KIGFX
{
// Forward declarations
class VIEW;
class VIEW_ITEM_DATA;

enum VIEW_UPDATE_FLAGS {
    NONE        = 0x00,     // No updates are required.
    APPEARANCE  = 0x01,     // Visibility flag has changed.
    COLOR       = 0x02,     // Color has changed.
    GEOMETRY    = 0x04,     // Position or shape has changed.
    LAYERS      = 0x08,     // Layers have changed.
    INITIAL_ADD = 0x10,     // Item is being added to the view.
    REPAINT     = 0x20,     // Item needs to be redrawn.
    ALL         = 0xef      // All except INITIAL_ADD.
};

enum VIEW_VISIBILITY_FLAGS {
    VISIBLE        = 0x01,  // Item is visible (in general)
    // Item is temporarily hidden (usually in favor of a being drawn from an overlay, such as a SELECTION).  Overrides VISIBLE flag.
    HIDDEN         = 0x02,
    OVERLAY_HIDDEN = 0x04   // Item is temporarily hidden from being drawn on an overlay.
};

// An abstract base class for deriving all objects that can be added to a VIEW.
// Its role is to:
// - communicate geometry, appearance and visibility updates to the associated dynamic VIEW,
// - provide a bounding box for redraw area calculation,
// - (optional) draw the object using the GAL API functions for PAINTER-less implementations.
// VIEW_ITEM objects are never owned by a VIEW. A single VIEW_ITEM can belong to any number of
// static VIEWs, but only one dynamic VIEW due to storage of only one VIEW reference.
class GAL_API VIEW_ITEM : public INSPECTABLE
{
public:
    VIEW_ITEM( bool isSCH_ITEM = false, bool isBOARD_ITEM = false ) :
            m_isSCH_ITEM( isSCH_ITEM ),
            m_isBOARD_ITEM( isBOARD_ITEM ),
            m_viewPrivData( nullptr ),
            m_forcedTransparency( 0.0 )
    {
    }

    virtual ~VIEW_ITEM();

    VIEW_ITEM( const VIEW_ITEM& aOther ) = delete;
    VIEW_ITEM& operator=( const VIEW_ITEM& aOther ) = delete;

    bool IsSCH_ITEM() const { return m_isSCH_ITEM; }
    bool IsBOARD_ITEM() const { return m_isBOARD_ITEM; }

    virtual QString GetClass() const = 0;

    virtual const BOX2I ViewBBox() const = 0;

    virtual void ViewDraw( int aLayer, VIEW* aView ) const;

    virtual QVector<int> ViewGetLayers() const = 0;

    virtual double ViewGetLOD( int aLayer, const VIEW* aView ) const
    {
        // By default always show the item
        return LOD_SHOW;
    }

    VIEW_ITEM_DATA* viewPrivData() const
    {
        return m_viewPrivData;
    }

    void SetForcedTransparency( double aForcedTransparency )
    {
        m_forcedTransparency = aForcedTransparency;
    }

    double GetForcedTransparency() const
    {
        return m_forcedTransparency;
    }

protected:
    static constexpr double LOD_HIDE = std::numeric_limits<double>::max();
    static constexpr double LOD_SHOW = 0.0;

    static double lodScaleForThreshold( const KIGFX::VIEW* aView, int aWhatIu, int aThresholdIu );

private:
    friend class VIEW;

    bool            m_isSCH_ITEM;
    bool            m_isBOARD_ITEM;
    VIEW_ITEM_DATA* m_viewPrivData;
    double          m_forcedTransparency;  // Additional transparency for diff'ing items.
};

} // namespace KIGFX

#if defined( _MSC_VER )
#pragma warning( pop )
#endif
