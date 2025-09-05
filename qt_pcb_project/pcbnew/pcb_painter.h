
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_PAINTER_H
#define PCB_PAINTER_H

#include <frame_type.h>
#include <gal/painter.h>
#include <padstack.h>   // PAD_DRILL_SHAPE
#include <pcb_display_options.h>
#include <math/vector2d.h>
#include <memory>
#include <geometry/shape_segment.h>
#include <QString>


class EDA_ITEM;
class PCB_DISPLAY_OPTIONS;
class PCB_VIEWERS_SETTINGS_BASE;
class BOARD_ITEM;
class PCB_ARC;
class BOARD;
class PCB_VIA;
class PCB_TRACK;
class PAD;
class PCB_SHAPE;
class PCB_GROUP;
class FOOTPRINT;
class ZONE;
class PCB_REFERENCE_IMAGE;
class PCB_TEXT;
class PCB_FIELD;
class PCB_TEXTBOX;
class PCB_TABLE;
class PCB_DIMENSION_BASE;
class PCB_TARGET;
class PCB_MARKER;
class NET_SETTINGS;
class NETINFO_LIST;
class TEXT_ATTRIBUTES;

namespace KIFONT
{
class FONT;
class METRICS;
}

namespace KIGFX
{
class GAL;

/**
 * PCB specific render settings.
 */
class PCB_RENDER_SETTINGS : public RENDER_SETTINGS
{
public:
    friend class PCB_PAINTER;

    PCB_RENDER_SETTINGS();

    /**
     * Load settings related to display options (high-contrast mode, full or outline modes
     * for vias/pads/tracks and so on).
     *
     * @param aOptions are settings that you want to use for displaying items.
     */
    void LoadDisplayOptions( const PCB_DISPLAY_OPTIONS& aOptions );

    void LoadColors( const COLOR_SETTINGS* aSettings ) override;

    /// @copydoc RENDER_SETTINGS::GetColor()
    COLOR4D GetColor( const VIEW_ITEM* aItem, int aLayer ) const override;

    ///< Board-specific version
    COLOR4D GetColor( const BOARD_ITEM* aItem, int aLayer ) const;

    ///< nullptr version
    COLOR4D GetColor( std::nullptr_t, int aLayer ) const
    {
        return GetColor( static_cast<const BOARD_ITEM*>( nullptr ), aLayer );
    }

    bool GetShowPageLimits() const override;

    inline bool IsBackgroundDark() const override
    {
        auto it = m_layerColors.find( LAYER_PCB_BACKGROUND );

        if( it == m_layerColors.end() )
            return false;

        return it->second.GetBrightness() < 0.5;
    }

    const COLOR4D& GetBackgroundColor() const override
    {
        auto it = m_layerColors.find( LAYER_PCB_BACKGROUND );
        return it == m_layerColors.end() ? COLOR4D::BLACK : it->second;
    }

    void SetBackgroundColor( const COLOR4D& aColor ) override
    {
        m_layerColors[ LAYER_PCB_BACKGROUND ] = aColor;
    }

    const COLOR4D& GetGridColor() override { return m_layerColors[ LAYER_GRID ]; }

    const COLOR4D& GetCursorColor() override { return m_layerColors[ LAYER_CURSOR ]; }

    NET_COLOR_MODE GetNetColorMode() const { return m_netColorMode; }
    void SetNetColorMode( NET_COLOR_MODE aMode ) { m_netColorMode = aMode; }

    std::map<int, KIGFX::COLOR4D>& GetNetColorMap() { return m_netColors; }

    std::set<int>& GetHiddenNets() { return m_hiddenNets; }
    const std::set<int>& GetHiddenNets() const { return m_hiddenNets; }

public:
    bool               m_ForcePadSketchModeOn;
    bool               m_ForceShowFieldsWhenFPSelected;

    ZONE_DISPLAY_MODE  m_ZoneDisplayMode;
    HIGH_CONTRAST_MODE m_ContrastModeDisplay;

    PAD*               m_PadEditModePad;       // Pad currently in Pad Edit Mode (if any)

protected:
    ///< Maximum font size for netnames (and other dynamically shown strings)
    static const double MAX_FONT_SIZE;

    ///< How to display nets and netclasses with color overrides
    NET_COLOR_MODE     m_netColorMode;

    ///< Overrides for specific netclass colors
    std::map<QString, KIGFX::COLOR4D> m_netclassColors;

    ///< Overrides for specific net colors, stored as netcodes for the ratsnest to access easily
    std::map<int, KIGFX::COLOR4D> m_netColors;

    ///< Set of net codes that should not have their ratsnest displayed
    std::set<int> m_hiddenNets;

    // These opacity overrides multiply with any opacity in the base layer color
    double m_trackOpacity;     ///< Opacity override for all tracks
    double m_viaOpacity;       ///< Opacity override for all types of via
    double m_padOpacity;       ///< Opacity override for SMD pads and PTHs
    double m_zoneOpacity;      ///< Opacity override for filled zones
    double m_imageOpacity;     ///< Opacity override for user images
    double m_filledShapeOpacity;     ///< Opacity override for graphic shapes
};


/**
 * Contains methods for drawing PCB-specific items.
 */
class PCB_PAINTER : public PAINTER
{
public:
    PCB_PAINTER( GAL* aGal, FRAME_T aFrameType );

    /// @copydoc PAINTER::GetSettings()
    virtual PCB_RENDER_SETTINGS* GetSettings() override
    {
        return &m_pcbSettings;
    }

    /// @copydoc PAINTER::Draw()
    virtual bool Draw( const VIEW_ITEM* aItem, int aLayer ) override;

protected:
    PCB_VIEWERS_SETTINGS_BASE* viewer_settings();

    // Drawing functions for various types of PCB-specific items
    void draw( const PCB_TRACK* aTrack, int aLayer );
    void draw( const PCB_ARC* aArc, int aLayer );
    void draw( const PCB_VIA* aVia, int aLayer );
    void draw( const PAD* aPad, int aLayer );
    void draw( const PCB_SHAPE* aSegment, int aLayer );
    void draw( const PCB_REFERENCE_IMAGE* aBitmap, int aLayer );
    void draw( const PCB_FIELD* aField, int aLayer );
    void draw( const PCB_TEXT* aText, int aLayer );
    void draw( const PCB_TEXTBOX* aText, int aLayer );
    void draw( const PCB_TABLE* aTable, int aLayer );
    void draw( const FOOTPRINT* aFootprint, int aLayer );
    void draw( const PCB_GROUP* aGroup, int aLayer );
    void draw( const ZONE* aZone, int aLayer );
    void draw( const PCB_DIMENSION_BASE* aDimension, int aLayer );
    void draw( const PCB_TARGET* aTarget );
    void draw( const PCB_MARKER* aMarker, int aLayer );

    /**
     * Get the thickness to draw for a line (e.g. 0 thickness lines get a minimum value).
     *
     * @param aActualThickness line own thickness
     * @return the thickness to draw
     */
    int getLineThickness( int aActualThickness ) const;

    /**
     * Return drill shape of a pad.
     */
    virtual PAD_DRILL_SHAPE getDrillShape( const PAD* aPad ) const;

    /**
     * Return hole shape for a pad (internal units).
     */
    virtual SHAPE_SEGMENT getPadHoleShape( const PAD* aPad ) const;

    /**
     * Return drill diameter for a via (internal units).
     */
    virtual int getViaDrillSize( const PCB_VIA* aVia ) const;

    void strokeText( const QString& aText, const VECTOR2I& aPosition,
                     const TEXT_ATTRIBUTES& aAttrs, const KIFONT::METRICS& aFontMetrics );

    void renderNetNameForSegment( const SHAPE_SEGMENT& aSeg, const COLOR4D& aColor, const QString& aNetName ) const;

protected:
    PCB_RENDER_SETTINGS m_pcbSettings;
    FRAME_T             m_frameType;

    int                 m_maxError;
    int                 m_holePlatingThickness;
    int                 m_lockedShadowMargin;
};
} // namespace KIGFX

#endif /* PCB_PAINTER_H */
