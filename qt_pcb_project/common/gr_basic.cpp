#include <gr_basic.h>
#include <trigo.h>
#include <eda_item.h>
#include <QtGui/QPainter>
#include <QtGui/QPen>
#include <QtGui/QBrush>
#include <QtCore/QPoint>
#include <QtCore/QRect>

#include <algorithm>
#include <cmath>

static const bool FILLED = true;
static const bool NOT_FILLED = false;

GR_DRAWMODE g_XorMode = GR_NXOR;

static void GRSRect( QPainter* aPainter, int x1, int y1, int x2, int y2, int aWidth, const QColor& aColor );

static int     GRLastMoveToX, GRLastMoveToY;
static bool    s_ForceBlackPen;
static QColor  s_DC_lastbrushcolor( 0, 0, 0, 0 );
static bool    s_DC_lastbrushfill  = false;
static QPainter* s_Painter_lastPainter = nullptr;


static void vector2IQtDrawPolygon( QPainter* aPainter, const VECTOR2I* Points, int n )
{
    QPoint* points = new QPoint[n];

    for( int i = 0; i < n; i++ )
        points[i] = QPoint( Points[i].x, Points[i].y );

    aPainter->drawPolygon( points, n );
    delete[] points;
}


static void qtDrawLine( QPainter* painter, int x1, int y1, int x2, int y2, int width )
{
    GRLastMoveToX = x2;
    GRLastMoveToY = y2;
    painter->drawLine( x1, y1, x2, y2 );
}


void GRResetPenAndBrush( QPainter* painter )
{
    GRSetBrush( painter, QColor(Qt::black) );
    s_DC_lastbrushcolor = QColor( 0, 0, 0, 0 );
    s_Painter_lastPainter = nullptr;
}


void GRSetColorPen( QPainter* painter, const QColor& color, int width, Qt::PenStyle style )
{
    QColor penColor = color;

    QVector<qreal> dots;
    dots << 1 << 3;

    if( width <= 1 && painter->brush().style() != Qt::SolidPattern )
        width = 1;

    if( s_ForceBlackPen )
        penColor = Qt::black;

    if( width == 0 )
    {
        penColor = Qt::transparent;
        style = Qt::NoPen;
    }

    const QPen& curr_pen = painter->pen();

    if( curr_pen.color() != penColor || curr_pen.width() != width || curr_pen.style() != style )
    {
        QPen pen;
        pen.setColor( penColor );

        if( style == Qt::DotLine )
        {
            style = Qt::CustomDashLine;
            pen.setDashPattern( dots );
        }

        pen.setWidth( width );
        pen.setStyle( style );
        painter->setPen( pen );
    }
    else
    {
        painter->setPen( curr_pen );
    }
}


void GRSetBrush( QPainter* painter, const QColor& color, bool fill )
{
    QColor brushColor = color;

    if( s_ForceBlackPen )
        brushColor = Qt::black;

    if( s_DC_lastbrushcolor != brushColor || s_DC_lastbrushfill != fill || s_Painter_lastPainter != painter )
    {
        QBrush brush;

        brush.setColor( brushColor );

        if( fill )
            brush.setStyle( Qt::SolidPattern );
        else
            brush.setStyle( Qt::NoBrush );

        painter->setBrush( brush );

        s_DC_lastbrushcolor = brushColor;
        s_DC_lastbrushfill = fill;
        s_Painter_lastPainter = painter;
    }
}


void GRForceBlackPen( bool flagforce )
{
    s_ForceBlackPen = flagforce;
}


bool GetGRForceBlackPenState( void )
{
    return s_ForceBlackPen;
}


void GRLine( QPainter* painter, int x1, int y1, int x2, int y2, int width, const QColor& color,
             Qt::PenStyle aStyle)
{
    GRSetColorPen( painter, color, width, aStyle );
    qtDrawLine( painter, x1, y1, x2, y2, width );
    GRLastMoveToX = x2;
    GRLastMoveToY = y2;
}


void GRLine( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
             const QColor& aColor, Qt::PenStyle aStyle )
{
    GRLine( aPainter, aStart.x, aStart.y, aEnd.x, aEnd.y, aWidth, aColor, aStyle );
}


void GRMoveTo( int x, int y )
{
    GRLastMoveToX = x;
    GRLastMoveToY = y;
}


void GRLineTo( QPainter* painter, int x, int y, int width, const QColor& color )
{
    GRLine( painter, GRLastMoveToX, GRLastMoveToY, x, y, width, color );
}


void GRCSegm( QPainter* painter, const VECTOR2I& A, const VECTOR2I& B, int width, const QColor& color )
{
    GRLastMoveToX = B.x;
    GRLastMoveToY = B.y;

    if( width <= 2 )
    {
        GRSetColorPen( painter, color, width );
        painter->drawLine( A.x, A.y, B.x, B.y );
        return;
    }

    GRSetBrush( painter, color, NOT_FILLED );
    GRSetColorPen( painter, color, 0 );

    int radius = ( width + 1 ) >> 1;
    int dx = B.x - A.x;
    int dy = B.y - A.y;
    EDA_ANGLE angle( VECTOR2I( dx, dy ) );

    angle = -angle;

    VECTOR2I start;
    VECTOR2I end;
    VECTOR2I org( A.x, A.y );
    int len = (int) hypot( dx, dy );

    QTransform transform = painter->transform();
    bool mirrored = (transform.m11() > 0 && transform.m22() < 0) || (transform.m11() < 0 && transform.m22() > 0);

    start.x = 0;
    start.y = radius;
    end.x = len;
    end.y = radius;
    RotatePoint( start, angle );
    RotatePoint( end, angle );

    start += org;
    end += org;

    painter->drawLine( QPoint( start.x, start.y ), QPoint( end.x, end.y ) );

    end.x = 0;
    end.y = -radius;
    RotatePoint( end, angle );
    end += org;

    QRect rect( org.x - radius, org.y - radius, 2 * radius, 2 * radius );
    int startAngle = (int)( atan2( start.y - org.y, start.x - org.x ) * 180 / M_PI * 16 );
    int endAngle = (int)( atan2( end.y - org.y, end.x - org.x ) * 180 / M_PI * 16 );
    int spanAngle = endAngle - startAngle;
    if( spanAngle < 0 ) spanAngle += 360 * 16;
    
    if( !mirrored )
        painter->drawArc( rect, startAngle, spanAngle );
    else
        painter->drawArc( rect, endAngle, -spanAngle );

    start.x = len;
    start.y = -radius;
    RotatePoint( start, angle );
    start += org;

    painter->drawLine( QPoint( start.x, start.y ), QPoint( end.x, end.y ) );

    end.x = len;
    end.y = radius;
    RotatePoint( end, angle);
    end += org;

    rect = QRect( B.x - radius, B.y - radius, 2 * radius, 2 * radius );
    startAngle = (int)( atan2( end.y - B.y, end.x - B.x ) * 180 / M_PI * 16 );
    endAngle = (int)( atan2( start.y - B.y, start.x - B.x ) * 180 / M_PI * 16 );
    spanAngle = endAngle - startAngle;
    if( spanAngle < 0 ) spanAngle += 360 * 16;
    
    if( !mirrored )
        painter->drawArc( rect, startAngle, spanAngle );
    else
        painter->drawArc( rect, endAngle, -spanAngle );
}


void GRFilledSegment( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
                      const QColor& aColor )
{
    GRSetColorPen( aPainter, aColor, aWidth );
    qtDrawLine( aPainter, aStart.x, aStart.y, aEnd.x, aEnd.y, aWidth );
}


static void GRSPoly( QPainter* painter, int n, const VECTOR2I* Points, bool Fill, int width,
                     const QColor& Color, const QColor& BgColor )
{
    if( Fill && ( n > 2 ) )
    {
        GRSetBrush( painter, BgColor, FILLED );
        GRSetColorPen( painter, Color, width );

        vector2IQtDrawPolygon( painter, Points, n );
    }
    else
    {
        GRMoveTo( Points[0].x, Points[0].y );

        for( int i = 1; i < n; ++i )
            GRLineTo( painter, Points[i].x, Points[i].y, width, Color );
    }
}


static void GRSClosedPoly( QPainter* aPainter, int aPointCount, const VECTOR2I* aPoints, bool aFill,
                           int aWidth, const QColor& aColor, const QColor& aBgColor )
{
    if( aFill && ( aPointCount > 2 ) )
    {
        GRLastMoveToX = aPoints[aPointCount - 1].x;
        GRLastMoveToY = aPoints[aPointCount - 1].y;
        GRSetBrush( aPainter, aBgColor, FILLED );
        GRSetColorPen( aPainter, aColor, aWidth );
        vector2IQtDrawPolygon( aPainter, aPoints, aPointCount );
    }
    else
    {
        GRMoveTo( aPoints[0].x, aPoints[0].y );

        for( int i = 1; i < aPointCount; ++i )
            GRLineTo( aPainter, aPoints[i].x, aPoints[i].y, aWidth, aColor );

        int lastpt = aPointCount - 1;

        if( aPoints[lastpt] != aPoints[0] )
            GRLineTo( aPainter, aPoints[0].x, aPoints[0].y, aWidth, aColor );
    }
}


void GRPoly( QPainter* painter, int n, const VECTOR2I* Points, bool Fill, int width, const QColor& Color,
             const QColor& BgColor )
{
    GRSPoly( painter, n, Points, Fill, width, Color, BgColor );
}


void GRClosedPoly( QPainter* painter, int n, const VECTOR2I* Points, bool Fill, const QColor& Color )
{
    GRSClosedPoly( painter, n, Points, Fill, 0, Color, Color );
}


void GRCircle( QPainter* aPainter, const VECTOR2I& aPos, int aRadius, int aWidth, const QColor& aColor )
{
    GRSetBrush( aPainter, aColor, NOT_FILLED );
    GRSetColorPen( aPainter, aColor, aWidth );

    QRect rect( aPos.x - aRadius, aPos.y - aRadius, 2 * aRadius, 2 * aRadius );
    aPainter->drawArc( rect, 0, 180 * 16 );
    aPainter->drawArc( rect, 180 * 16, 180 * 16 );
}


void GRFilledCircle( QPainter* aPainter, const VECTOR2I& aPos, int aRadius, int aWidth,
                     const QColor& aStrokeColor, const QColor& aFillColor )
{
    GRSetBrush( aPainter, aFillColor, FILLED );
    GRSetColorPen( aPainter, aStrokeColor, aWidth );
    aPainter->drawEllipse( aPos.x - aRadius, aPos.y - aRadius, 2 * aRadius, 2 * aRadius );
}


void GRArc( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, const VECTOR2I& aCenter,
            int aWidth, const QColor& aColor )
{
    GRSetBrush( aPainter, aColor );
    GRSetColorPen( aPainter, aColor, aWidth );
    
    int radius = (int)hypot( aStart.x - aCenter.x, aStart.y - aCenter.y );
    QRect rect( aCenter.x - radius, aCenter.y - radius, 2 * radius, 2 * radius );
    int startAngle = (int)( atan2( aStart.y - aCenter.y, aStart.x - aCenter.x ) * 180 / M_PI * 16 );
    int endAngle = (int)( atan2( aEnd.y - aCenter.y, aEnd.x - aCenter.x ) * 180 / M_PI * 16 );
    int spanAngle = endAngle - startAngle;
    if( spanAngle < 0 ) spanAngle += 360 * 16;
    
    aPainter->drawArc( rect, startAngle, spanAngle );
}


void GRFilledArc( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, const VECTOR2I& aCenter,
                  int width, const QColor& Color, const QColor& BgColor )
{
    GRSetBrush( painter, BgColor, FILLED );
    GRSetColorPen( painter, Color, width );
    
    int radius = (int)hypot( aStart.x - aCenter.x, aStart.y - aCenter.y );
    QRect rect( aCenter.x - radius, aCenter.y - radius, 2 * radius, 2 * radius );
    int startAngle = (int)( atan2( aStart.y - aCenter.y, aStart.x - aCenter.x ) * 180 / M_PI * 16 );
    int endAngle = (int)( atan2( aEnd.y - aCenter.y, aEnd.x - aCenter.x ) * 180 / M_PI * 16 );
    int spanAngle = endAngle - startAngle;
    if( spanAngle < 0 ) spanAngle += 360 * 16;
    
    painter->drawPie( rect, startAngle, spanAngle );
}


void GRRect( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
             const QColor& aColor )
{
    GRSRect( painter, aStart.x, aStart.y, aEnd.x, aEnd.y, aWidth, aColor );
}


void GRFilledRect( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
                   const QColor& aColor, const QColor& aBgColor )
{
    GRSFilledRect( painter, aStart.x, aStart.y, aEnd.x, aEnd.y, aWidth, aColor, aBgColor );
}


void GRSRect( QPainter* aPainter, int x1, int y1, int x2, int y2, int aWidth, const QColor& aColor )
{
    VECTOR2I points[5];
    points[0] = VECTOR2I( x1, y1 );
    points[1] = VECTOR2I( x1, y2 );
    points[2] = VECTOR2I( x2, y2 );
    points[3] = VECTOR2I( x2, y1 );
    points[4] = points[0];
    GRSClosedPoly( aPainter, 5, points, NOT_FILLED, aWidth, aColor, aColor );
}


void GRSFilledRect( QPainter* aPainter, int x1, int y1, int x2, int y2, int aWidth, const QColor& aColor,
                    const QColor& aBgColor )
{
    VECTOR2I points[5];
    points[0] = VECTOR2I( x1, y1 );
    points[1] = VECTOR2I( x1, y2 );
    points[2] = VECTOR2I( x2, y2 );
    points[3] = VECTOR2I( x2, y1 );
    points[4] = points[0];

    GRSetBrush( aPainter, aBgColor, FILLED );
    GRSetColorPen( aPainter, aBgColor, aWidth );

    vector2IQtDrawPolygon( aPainter, points, 5 );
}