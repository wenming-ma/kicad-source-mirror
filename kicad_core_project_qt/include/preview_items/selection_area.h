
#ifndef PREVIEW_ITEMS_SELECTION_AREA_H
#define PREVIEW_ITEMS_SELECTION_AREA_H

#include <preview_items/simple_overlay_item.h>
#include <QString>


namespace KIGFX
{
class GAL;

namespace PREVIEW
{

/**
 * Represent a selection area (currently a rectangle) in a VIEW, drawn corner-to-corner between
 * two points.
 *
 * This is useful when selecting a rectangular area, for lasso-select or zooming, for example.
 */
class SELECTION_AREA : public SIMPLE_OVERLAY_ITEM
{
public:
    static const int SelectionLayer = LAYER_GP_OVERLAY;

    SELECTION_AREA();

    const BOX2I ViewBBox() const override;

    ///< Set the origin of the rectangle (the fixed corner)
    void SetOrigin( const VECTOR2I& aOrigin )
    {
        m_origin = aOrigin;
    }

    /**
     * Set the current end of the rectangle (the corner that moves with the cursor.
     */
    void SetEnd( const VECTOR2I& aEnd )
    {
        m_end = aEnd;
    }

    /**
     * @return  string "SELECTION_AREA"
     */
    QString GetClass() const override
    {
        return QStringLiteral( "SELECTION_AREA" );
    }

    VECTOR2I GetOrigin() const { return m_origin; }

    VECTOR2I GetEnd() const { return m_end; }

    void SetAdditive( bool aAdditive ) { m_additive = aAdditive; }
    void SetSubtractive( bool aSubtractive ) { m_subtractive = aSubtractive; }
    void SetExclusiveOr( bool aExclusiveOr ) { m_exclusiveOr = aExclusiveOr; }

    void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override final;

private:

    bool m_additive;
    bool m_subtractive;
    bool m_exclusiveOr;

    VECTOR2I m_origin, m_end;
};

} // PREVIEW
} // KIGFX

#endif // PREVIEW_ITEMS_SELECTION_AREA_H
