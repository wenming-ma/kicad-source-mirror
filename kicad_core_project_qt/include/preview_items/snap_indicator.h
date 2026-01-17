
#pragma once

#include <origin_viewitem.h>
#include <geometry/point_types.h>
#include <QString>


namespace KIGFX
{

/**
 * View item to draw an origin marker with an optional
 * snap type indicator.
 */
class SNAP_INDICATOR : public ORIGIN_VIEWITEM
{
public:
    SNAP_INDICATOR( const COLOR4D& aColor = COLOR4D::WHITE, int aSize = 16,
                    const VECTOR2D& aPosition = VECTOR2D( 0, 0 ) );

    SNAP_INDICATOR( const VECTOR2D& aPosition, EDA_ITEM_FLAGS flags );

    SNAP_INDICATOR* Clone() const override;

    const BOX2I ViewBBox() const override;

    void ViewDraw( int aLayer, VIEW* aView ) const override;

#if defined( DEBUG )
    void Show( int x, std::ostream& st ) const override {}
#endif

    /**
     * Get class name.
     *
     * @return string "SNAP_INDICATOR"
     */
    QString GetClass() const override { return "SNAP_INDICATOR"; }

    /**
     * Set a mask of point types that this snap item represents.
     *
     * This is a mask of POINT_TYPE.
     */
    void SetSnapTypes( int aSnapTypes ) { m_snapTypes = aSnapTypes; }

private:
    int m_snapTypes = POINT_TYPE::PT_NONE;
};

} // namespace KIGFX
