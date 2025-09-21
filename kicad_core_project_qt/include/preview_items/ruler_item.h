
#ifndef PREVIEW_ITEMS_RULER_ITEM_H
#define PREVIEW_ITEMS_RULER_ITEM_H

#include <optional>

#include <QStringList>

#include <eda_item.h>
#include <gal/color4d.h>
#include <preview_items/two_point_geom_manager.h>

namespace KIGFX
{
class GAL;

namespace PREVIEW
{
class TWO_POINT_GEOMETRY_MANAGER;

/**
 * A drawn ruler item for showing the distance between two points.
 */
class RULER_ITEM : public EDA_ITEM
{
public:
    RULER_ITEM( const TWO_POINT_GEOMETRY_MANAGER& m_geomMgr, const EDA_IU_SCALE& aIuScale,
                EDA_UNITS userUnits, bool aFlipX, bool aFlipY );

    ///< @copydoc EDA_ITEM::ViewBBox()
    const BOX2I ViewBBox() const override;

    ///< @copydoc EDA_ITEM::ViewGetLayers()
    std::vector<int> ViewGetLayers() const override;

    ///< @copydoc EDA_ITEM::ViewDraw();
    void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override final;

    void SetColor( const COLOR4D& aColor ) { m_color = aColor; }

    void SetShowTicks( bool aShow ) { m_showTicks = aShow; }

    void SetShowEndArrowHead( bool aShow ) { m_showEndArrowHead = aShow; }

    /**
     * Get the strings for the dimensions of the ruler.
     */
    QStringList GetDimensionStrings() const;

#if defined(DEBUG)
    void Show( int x, std::ostream& st ) const override
    {
    }
#endif

    /**
     * Get class name.
     *
     * @return  string "RULER_ITEM".
     */
    QString GetClass() const override
    {
        return "RULER_ITEM";
    }

    /**
     * Switch the ruler units.
     *
     * @param aUnits is the new unit system the ruler should use.
     */
    void SwitchUnits( EDA_UNITS aUnits ) { m_userUnits = aUnits; }

    void UpdateDir( bool aFlipX, bool aFlipY )
    {
        m_flipX = aFlipX;
        m_flipY = aFlipY;
    }

private:
    const TWO_POINT_GEOMETRY_MANAGER& m_geomMgr;
    EDA_UNITS                         m_userUnits;
    const EDA_IU_SCALE&               m_iuScale;
    bool                              m_flipX;
    bool                              m_flipY;
    std::optional<COLOR4D>            m_color;
    bool                              m_showTicks = true;
    bool                              m_showEndArrowHead = false;
};

} // PREVIEW
} // KIGFX

#endif // PREVIEW_ITEMS_RULER_ITEM_H
