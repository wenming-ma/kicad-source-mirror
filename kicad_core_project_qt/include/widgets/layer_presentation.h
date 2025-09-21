
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21
#ifndef LAYER_PRESENTATION_H
#define LAYER_PRESENTATION_H

#include <gal/color4d.h>
#include <layer_ids.h>

class QPixmap;

using KIGFX::COLOR4D;

/**
 * Base class for an object that can provide information about
 * presenting layers (colours, etc).
 */
class LAYER_PRESENTATION
{
public:
    virtual ~LAYER_PRESENTATION() {}

    // Return a color index from the layer id
    virtual COLOR4D getLayerColor( int aLayer ) const = 0;

    // Return the name of the layer id
    virtual QString getLayerName( int aLayer ) const = 0;

    // Fill the layer bitmap aLayerbmp with the layer color
    static void DrawColorSwatch( QPixmap& aLayerbmp, const COLOR4D& aBackground,
                                 const COLOR4D& aColor );

    /**
     * Fill the layer bitmap aLayerbmp with the layer color
     * for the layer ID.
     */
    void DrawColorSwatch( QPixmap& aLayerbmp, int aLayer ) const;

    /**
     * Create a layer pair "side-by-side swatch" icon
     */
    static std::unique_ptr<QPixmap> CreateLayerPairIcon( const KIGFX::COLOR4D& aBgColor,
                                                          const KIGFX::COLOR4D& aTopColor,
                                                          const KIGFX::COLOR4D& aBottomColor,
                                                          int                   aScale );

    /**
     * Create a layer pair "side-by-side swatch" icon for the given
     * layer pair with the style of this presentation.
     */
    std::unique_ptr<QPixmap> CreateLayerPairIcon( int aLeftLayer, int aRightLayer,
                                                   int aScale ) const;
};

#endif // LAYER_PRESENTATION_H
