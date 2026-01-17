// QT_TRANSFORMATION_COMPLETED

#ifndef RENDER_SETTINGS_H
#define RENDER_SETTINGS_H

#include <map>
#include <set>

#include <gal/color4d.h>
#include <layer_ids.h>
#include <lset.h>
#include <memory>

#include <QPainter>
#include <QString>

class COLOR_SETTINGS;

namespace KIGFX
{
class VIEW_ITEM;

class RENDER_SETTINGS
{
public:
    RENDER_SETTINGS();
    virtual ~RENDER_SETTINGS();

    virtual void LoadColors( const COLOR_SETTINGS* aSettings ) { }

    inline void SetLayerIsHighContrast( int aLayerId, bool aEnabled = true )
    {
        if( aEnabled )
            m_highContrastLayers.insert( aLayerId );
        else
            m_highContrastLayers.erase( aLayerId );
    }

    inline bool GetLayerIsHighContrast( int aLayerId ) const
    {
        return ( m_highContrastLayers.count( aLayerId ) > 0 );
    }

    const std::set<int> GetHighContrastLayers() const
    {
        return m_highContrastLayers;
    }

    PCB_LAYER_ID GetPrimaryHighContrastLayer() const
    {
        for( int layer : m_highContrastLayers )
        {
            if( layer >= PCBNEW_LAYER_ID_START && layer < PCB_LAYER_ID_COUNT )
                return (PCB_LAYER_ID) layer;
        }

        return UNDEFINED_LAYER;
    }

    PCB_LAYER_ID GetActiveLayer() const { return m_activeLayer; }
    void SetActiveLayer( PCB_LAYER_ID aLayer ) { m_activeLayer = aLayer; }

    const QString& GetLayerName() const { return m_layerName; }
    void SetLayerName( const QString& aLayerName ) { m_layerName = aLayerName; }

    LSET GetPrintLayers() const { return m_printLayers; }
    void SetPrintLayers( const LSET& aLayerSet ) { m_printLayers = aLayerSet; }

    inline void ClearHighContrastLayers()
    {
        m_highContrastLayers.clear();
    }

    inline bool IsHighlightEnabled() const
    {
        return m_highlightEnabled;
    }

    inline const std::set<int>& GetHighlightNetCodes() const
    {
        return m_highlightNetcodes;
    }

    inline void SetHighlight( bool aEnabled, int aNetcode = -1, bool aMulti = false )
    {
        m_highlightEnabled = aEnabled;

        if( aEnabled )
        {
            if( !aMulti )
                m_highlightNetcodes.clear();

            m_highlightNetcodes.insert( aNetcode );
        }
        else
            m_highlightNetcodes.clear();
    }

    inline void SetHighlight( std::set<int>& aHighlight, bool aEnabled = true )
    {
        m_highlightEnabled  = aEnabled;

        if( aEnabled )
            m_highlightNetcodes = aHighlight;
        else
            m_highlightNetcodes.clear();
    }

    void SetHighContrast( bool aEnabled ) { m_hiContrastEnabled = aEnabled; }
    bool GetHighContrast() const { return m_hiContrastEnabled; }

    void SetDrawBoundingBoxes( bool aEnabled ) { m_drawBoundingBoxes = aEnabled; }
    bool GetDrawBoundingBoxes() const { return m_drawBoundingBoxes; }

    virtual COLOR4D GetColor( const VIEW_ITEM* aItem, int aLayer ) const = 0;

    float GetDrawingSheetLineWidth() const { return m_drawingSheetLineWidth; }

    int GetDefaultPenWidth() const { return m_defaultPenWidth; }
    void SetDefaultPenWidth( int aWidth ) { m_defaultPenWidth = aWidth; }

    int GetMinPenWidth() const { return m_minPenWidth; }
    void SetMinPenWidth( int aWidth ) { m_minPenWidth = aWidth; }

    double GetDashLengthRatio() const { return m_dashLengthRatio; }
    void SetDashLengthRatio( double aRatio ) { m_dashLengthRatio = aRatio; }
    double GetDashLength( int aLineWidth ) const;
    double GetDotLength( int aLineWidth ) const;

    double GetGapLengthRatio() const { return m_gapLengthRatio; }
    void SetGapLengthRatio( double aRatio ) { m_gapLengthRatio = aRatio; }
    double GetGapLength( int aLineWidth ) const;

    virtual bool GetShowPageLimits() const { return true; }

    bool IsPrinting() const { return m_isPrinting; }
    void SetIsPrinting( bool isPrinting ) { m_isPrinting = isPrinting; }

    bool IsPrintBlackAndWhite() const { return m_printBlackAndWite; }
    void SetPrintBlackAndWhite( bool aPrintBlackAndWhite )
    {
        m_printBlackAndWite = aPrintBlackAndWhite;
    }

    bool PrintBlackAndWhiteReq() const
    {
        return m_printBlackAndWite && m_isPrinting;
    }

    virtual const COLOR4D& GetBackgroundColor() const = 0;

    virtual void SetBackgroundColor( const COLOR4D& aColor ) = 0;

    virtual const COLOR4D& GetGridColor() = 0;

    virtual const COLOR4D& GetCursorColor() = 0;

    inline const COLOR4D& GetLayerColor( int aLayer ) const
    {
        // We don't (yet?) have a separate color for intersheet refs
        if( aLayer == LAYER_INTERSHEET_REFS )
            aLayer = LAYER_GLOBLABEL;

        return m_layerColors.count( aLayer ) ? m_layerColors.at( aLayer ) : COLOR4D::BLACK;
    }

    inline void SetLayerColor( int aLayer, const COLOR4D& aColor )
    {
        m_layerColors[aLayer] = aColor;

        update();       // recompute other shades of the color
    }

    virtual bool IsBackgroundDark() const
    {
        return false;
    }

    void SetOutlineWidth( float aWidth ) { m_outlineWidth = aWidth; }
    float GetOutlineWidth() const { return m_outlineWidth; }

    void SetHighlightFactor( float aFactor ) { m_highlightFactor = aFactor; }
    void SetSelectFactor( float aFactor ) { m_selectFactor = aFactor; }

    void SetDefaultFont( const QString& aFont ) { m_defaultFont = aFont; }
    const QString& GetDefaultFont() const { return m_defaultFont; }

    QPainter* GetPrintPainter() const { return m_printPainter; }
    void SetPrintPainter( QPainter* aPainter ) { m_printPainter = aPainter; }

    // Qt compatibility method - alias for GetPrintPainter()
    QPainter* GetPrintDC() const { return GetPrintPainter(); }

protected:
    virtual void update();

    PCB_LAYER_ID           m_activeLayer;        // The active layer (as shown by appearance mgr)
    QString                m_layerName;
    std::set<int>          m_highContrastLayers; // High-contrast layers (both board layers and
                                                 //   synthetic GAL layers)
    std::map<int, COLOR4D> m_layerColors;        // Layer colors
    std::map<int, COLOR4D> m_layerColorsHi;      // Layer colors for highlighted objects
    std::map<int, COLOR4D> m_layerColorsSel;     // Layer colors for selected objects
    std::map<int, COLOR4D> m_hiContrastColor;    // High-contrast mode layer colors
    std::map<int, COLOR4D> m_layerColorsDark;    // Darkened layer colors (for high-contrast mode)

    COLOR4D m_backgroundColor;                   // The background color

    /// Parameters for display modes
    bool          m_hiContrastEnabled;    // High contrast display mode on/off
    float         m_hiContrastFactor;     // Factor used for computing high contrast color

    bool          m_highlightEnabled;     // Highlight display mode on/off
    std::set<int> m_highlightNetcodes;    // Set of net cods to be highlighted
    float         m_highlightFactor;      // Factor used for computing highlight color

    bool          m_drawBoundingBoxes;    // Visual aid for debugging

    float         m_selectFactor;         // Specifies how color of selected items is changed
    float         m_outlineWidth;         // Line width used when drawing outlines
    float         m_drawingSheetLineWidth;// Line width used for borders and titleblock

    int           m_defaultPenWidth;
    int           m_minPenWidth;          // Some clients (such as PDF) don't like ultra-thin
                                          // lines.  This sets an absolute minimum.
    double        m_dashLengthRatio;
    double        m_gapLengthRatio;

    QString       m_defaultFont;

    bool          m_isPrinting;           // true when draw to a printer
    bool          m_printBlackAndWite;    // true if black and white printing is requested: some
                                          // backgrounds are not printed to avoid not visible items
    LSET          m_printLayers;

    QPainter*     m_printPainter;
};

}

#endif /* RENDER_SETTINGS_H */
