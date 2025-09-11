#ifndef GR_BASIC
#define GR_BASIC

#include <gal/color4d.h>
#include <math/box2.h>
#include <vector>
#include <QPen>
#include <QPainter>

using KIGFX::COLOR4D;


/// Drawmode. Compositing mode plus a flag or two
enum GR_DRAWMODE {
    GR_OR                 = 0x01000000,
    GR_XOR                = 0x02000000,
    GR_AND                = 0x04000000,
    GR_NXOR               = 0x08000000,
    GR_INVERT             = 0x10000000,
    GR_ALLOW_HIGHCONTRAST = 0x20000000,
    GR_COPY               = 0x40000000,
    GR_HIGHLIGHT          = 0x80000000,
    UNSPECIFIED_DRAWMODE  = -1
};

inline GR_DRAWMODE operator~( const GR_DRAWMODE& a )
{
    return static_cast<GR_DRAWMODE>( ~int( a ) );
}

inline GR_DRAWMODE operator|( const GR_DRAWMODE& a, const GR_DRAWMODE& b )
{
    return static_cast<GR_DRAWMODE>( int( a ) | int( b ) );
}

inline GR_DRAWMODE operator&( const GR_DRAWMODE& a, const GR_DRAWMODE& b )
{
    return static_cast<GR_DRAWMODE>( int( a ) & int( b ) );
}

extern GR_DRAWMODE g_XorMode;

typedef enum {
    /* Line styles for Get/SetLineStyle. */
    GR_SOLID_LINE  = 0,
    GR_DOTTED_LINE = 1,
    GR_DASHED_LINE = 3
} GRLineStypeType;


void GRResetPenAndBrush( QPainter* painter );
void GRSetColorPen( QPainter* painter, const COLOR4D& Color, int width = 1,
                    Qt::PenStyle stype = Qt::SolidLine );
void GRSetBrush( QPainter* painter, const COLOR4D& Color, bool fill = false );

void GRForceBlackPen( bool flagforce );

bool GetGRForceBlackPenState( void );

void GRLine( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
             const COLOR4D& aColor, Qt::PenStyle aStyle = Qt::SolidLine );
void GRLine( QPainter* painter, int x1, int y1, int x2, int y2, int width, const COLOR4D& Color,
             Qt::PenStyle aStyle = Qt::SolidLine );
void GRMoveTo( int x, int y );
void GRLineTo( QPainter* painter, int x, int y, int width, const COLOR4D& Color );

void GRPoly( QPainter* painter, int n, const VECTOR2I* Points, bool Fill, int width, const COLOR4D& Color,
             const COLOR4D& BgColor );

void GRClosedPoly( QPainter* aPainter, int aPointCount, const VECTOR2I* aPoints, bool doFill,
                   const COLOR4D& aColor );

void GRFilledCircle( QPainter* aPainter, const VECTOR2I& aPos, int aRadius, int aWidth,
                     const COLOR4D& aStrokeColor, const COLOR4D& aFillColor );
void GRCircle( QPainter* aPainter, const VECTOR2I& aPos, int aRadius, int aWidth, const COLOR4D& aColor );

void GRArc( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, const VECTOR2I& aCenter,
            int aWidth, const COLOR4D& aColor );
void GRFilledArc( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, const VECTOR2I& aCenter,
                  int width, const COLOR4D& Color, const COLOR4D& BgColor );

void GRFilledSegment( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
                      const COLOR4D& aColor );

void GRCSegm( QPainter* aPainter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
              const COLOR4D& aColor );

void GRFilledRect( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
                   const COLOR4D& aColor, const COLOR4D& aBgColor );
void GRRect( QPainter* painter, const VECTOR2I& aStart, const VECTOR2I& aEnd, int aWidth,
             const COLOR4D& aColor );

void GRSFilledRect( QPainter* painter, int x1, int y1, int x2, int y2, int width, const COLOR4D& Color,
                    const COLOR4D& BgColor );

#endif      /* define GR_BASIC */
