
#ifndef __ORIGIN_VIEWITEM_H
#define __ORIGIN_VIEWITEM_H

#include <math/box2.h>
#include <view/view.h>
#include <layer_ids.h>
#include <gal/color4d.h>
#include <eda_item.h>
#include <QString>

/**
 * View item to draw an origin marker.
 */
namespace KIGFX {

class ORIGIN_VIEWITEM : public EDA_ITEM
{
public:
    /// Marker symbol styles.
    enum MARKER_STYLE
    {
        NO_GRAPHIC, CROSS, X, DOT, CIRCLE_CROSS, CIRCLE_X, CIRCLE_DOT, DASH_LINE
    };

    ORIGIN_VIEWITEM( const COLOR4D& aColor = COLOR4D( 1.0, 1.0, 1.0, 1.0 ),
                     MARKER_STYLE aStyle = CIRCLE_X, int aSize = 16,
                     const VECTOR2D& aPosition = VECTOR2D( 0, 0 ) );

    ORIGIN_VIEWITEM( const VECTOR2D& aPosition, EDA_ITEM_FLAGS flags );

    ORIGIN_VIEWITEM* Clone() const override;

    const BOX2I ViewBBox() const override;

    void ViewDraw( int aLayer, VIEW* aView ) const override;

    std::vector<int> ViewGetLayers() const override
    {
        return { LAYER_GP_OVERLAY };
    }

#if defined(DEBUG)
    void Show( int x, std::ostream& st ) const override
    {
    }
#endif

    /**
     * Get class name.
     *
     * @return string "ORIGIN_VIEWITEM"
     */
    QString GetClass() const override
    {
        return "ORIGIN_VIEWITEM";
    }

    /**
     * Set the draw at zero flag.
     *
     * When set the marker will be drawn when its position is 0,0.  Otherwise it will not
     * be drawn when its position is 0,0.
     *
     * @param aDrawFlag The value to set the draw at zero flag.
     */
    inline void SetDrawAtZero( bool aDrawFlag )
    {
        m_drawAtZero = aDrawFlag;
    }

    void SetPosition( const VECTOR2I& aPosition ) override
    {
        m_position = VECTOR2D( aPosition );
    }

    VECTOR2I GetPosition() const override
    {
        return VECTOR2I( m_position.x, m_position.y );
    }

    inline void SetEndPosition( const VECTOR2D& aPosition )
    {
        m_end = aPosition;
    }

    inline const VECTOR2I GetEndPosition() const
    {
        return VECTOR2I( m_end.x, m_end.y );
    }

    inline void SetSize( int aSize )
    {
        m_size = aSize;
    }

    inline int GetSize() const
    {
        return m_size;
    }

    inline void SetColor( const KIGFX::COLOR4D& aColor )
    {
        m_color = aColor;
    }

    inline const KIGFX::COLOR4D& GetColor() const
    {
        return m_color;
    }

    inline void SetStyle( MARKER_STYLE aStyle )
    {
        m_style = aStyle;
    }

    inline MARKER_STYLE GetStyle() const
    {
        return m_style;
    }

protected:
    /// Marker coordinates.
    VECTOR2D        m_position;

    /// Marker end position for markers that stretch between points.
    VECTOR2D        m_end;

    /// Marker size (in pixels).
    int             m_size;

    /// Marker color.
    COLOR4D         m_color;

    /// Marker symbol.
    MARKER_STYLE    m_style;

    /// If set, the marker will be drawn even if its position is 0,0.
    bool            m_drawAtZero;
};

} // namespace KIGFX

#endif
