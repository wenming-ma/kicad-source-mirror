#pragma once

#include <gal/gal.h>
#include <view/view_item.h>
#include <vector>
#include <deque>
#include <QString>

class SEG;
class SHAPE_LINE_CHAIN;
class SHAPE_POLY_SET;

namespace KIGFX
{
class VIEW;

class GAL_API VIEW_OVERLAY : public VIEW_ITEM
{
public:

    VIEW_OVERLAY();
    virtual ~VIEW_OVERLAY();

    // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
    // will only land us in trouble.
    VIEW_OVERLAY( const VIEW_OVERLAY& ) = delete;
    VIEW_OVERLAY& operator=( const VIEW_OVERLAY& ) = delete;

    QString GetClass() const override;

    struct COMMAND;
    struct COMMAND_ARC;
    struct COMMAND_LINE;
    struct COMMAND_CIRCLE;
    struct COMMAND_RECTANGLE;

    struct COMMAND_SET_STROKE;
    struct COMMAND_SET_FILL;
    struct COMMAND_SET_COLOR;
    struct COMMAND_SET_WIDTH;

    struct COMMAND_POLYGON;
    struct COMMAND_POINT_POLYGON;
    struct COMMAND_POLY_POLYGON;

    struct COMMAND_POLYLINE;
    struct COMMAND_POINT_POLYLINE;

    struct COMMAND_GLYPH_SIZE;
    struct COMMAND_BITMAP_TEXT;

    void Clear();

    virtual const BOX2I ViewBBox() const override;
    virtual void ViewDraw( int aLayer, VIEW *aView ) const override;
    virtual std::vector<int> ViewGetLayers() const override;

    // Basic shape primitives
    void Line( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint );
    void Line( const SEG& aSeg );
    void Segment( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint, double aWidth );
    void Circle( const VECTOR2D& aCenterPoint, double aRadius );
    void Arc( const VECTOR2D& aCenterPoint, double aRadius, const EDA_ANGLE& aStartAngle,
              const EDA_ANGLE& aEndAngle );
    void Rectangle( const VECTOR2D& aStartPoint, const VECTOR2D& aEndPoint );
    void Cross( const VECTOR2D& aP, int aSize );

    // polygon primitives
    void Polygon( const std::deque<VECTOR2D>& aPointList );
    void Polygon( const SHAPE_POLY_SET& aPolySet );
    void Polyline( const SHAPE_LINE_CHAIN& aPolyLine );
    void Polygon( const VECTOR2D aPointList[], int aListSize );

    void BitmapText( const QString& aText, const VECTOR2I& aPosition, const EDA_ANGLE& aAngle );

    // Draw settings
    void SetIsFill( bool aIsFillEnabled );
    void SetIsStroke( bool aIsStrokeEnabled );
    void SetFillColor( const COLOR4D& aColor );
    void SetStrokeColor( const COLOR4D& aColor );
    void SetGlyphSize( const VECTOR2I& aSize );
    void SetLineWidth( double aLineWidth );

    const COLOR4D& GetStrokeColor() const { return m_strokeColor; }
    const COLOR4D& GetFillColor() const { return m_fillColor; }

private:
    void releaseCommands();

private:
    COLOR4D               m_strokeColor;
    COLOR4D               m_fillColor;
    std::vector<COMMAND*> m_commands;
};

} // namespace KIGFX

