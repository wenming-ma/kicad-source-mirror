
#include <gr_basic.h>
#include <plotters/plotter.h>
#include <trigo.h>
#include <math/util.h>          // for KiROUND
#include <font/font.h>

#include <callback_gal.h>


int GetPenSizeForBold( int aTextSize )
{
    return KiROUND( aTextSize / 5.0 );
}


int GetPenSizeForDemiBold( int aTextSize )
{
    return KiROUND( aTextSize / 6 );
}


int GetPenSizeForBold( const QSize& aTextSize )
{
    return GetPenSizeForBold( std::min( aTextSize.width(), aTextSize.height() ) );
}


int GetPenSizeForDemiBold( const QSize& aTextSize )
{
    return GetPenSizeForDemiBold( std::min( aTextSize.width(), aTextSize.height() ) );
}


int GetPenSizeForNormal( int aTextSize )
{
    return KiROUND( aTextSize / 8.0 );
}


int GetPenSizeForNormal( const QSize& aTextSize )
{
    return GetPenSizeForNormal( std::min( aTextSize.width(), aTextSize.height() ) );
}


int ClampTextPenSize( int aPenSize, int aSize, bool aStrict )
{
    double scale    = aStrict ? 0.18 : 0.25;
    int    maxWidth = KiROUND( (double) aSize * scale );

    return std::min( aPenSize, maxWidth );
}


float ClampTextPenSize( float aPenSize, int aSize, bool aStrict )
{
    double scale    = aStrict ? 0.18 : 0.25;
    float maxWidth = (float) aSize * scale;

    return std::min( aPenSize, maxWidth );
}


int ClampTextPenSize( int aPenSize, const VECTOR2I& aSize, bool aStrict )
{
    int size = std::min( std::abs( aSize.x ), std::abs( aSize.y ) );

    return ClampTextPenSize( aPenSize, size, aStrict );
}


int GRTextWidth( const QString& aText, KIFONT::FONT* aFont, const VECTOR2I& aSize,
                 int aThickness, bool aBold, bool aItalic, const KIFONT::METRICS& aFontMetrics )
{
    if( !aFont )
        aFont = KIFONT::FONT::GetFont();

    return KiROUND( aFont->StringBoundaryLimits( aText, aSize, aThickness, aBold, aItalic,
                                                 aFontMetrics ).x );
}


void GRPrintText( QPaintDevice* aDC, const VECTOR2I& aPos, const QColor& aColor, const QString& aText,
                  const EDA_ANGLE& aOrient, const VECTOR2I& aSize,
                  enum GR_TEXT_H_ALIGN_T aH_justify, enum GR_TEXT_V_ALIGN_T aV_justify,
                  int aWidth, bool aItalic, bool aBold, KIFONT::FONT* aFont,
                  const KIFONT::METRICS& aFontMetrics )
{
    KIGFX::GAL_DISPLAY_OPTIONS empty_opts;
    bool                       fill_mode = true;

    if( !aFont )
        aFont = KIFONT::FONT::GetFont();

    if( aWidth == 0 ) // Use default values if aWidth == 0
    {
        if( aBold )
            aWidth = GetPenSizeForBold( std::min( aSize.x, aSize.y ) );
        else
            aWidth = GetPenSizeForNormal( std::min( aSize.x, aSize.y ) );
    }

    if( aWidth < 0 )
    {
        aWidth = -aWidth;
        fill_mode = false;
    }

    QPainter painter( aDC );

    CALLBACK_GAL callback_gal( empty_opts,
            // Stroke callback
            [&]( const VECTOR2I& aPt1, const VECTOR2I& aPt2 )
            {
                if( fill_mode )
                    GRLine( &painter, aPt1, aPt2, aWidth, aColor );
                else
                    GRCSegm( &painter, aPt1, aPt2, aWidth, aColor );
            },
            // Polygon callback
            [&]( const SHAPE_LINE_CHAIN& aPoly )
            {
                GRClosedPoly( &painter, aPoly.PointCount(), aPoly.CPoints().data(), true, aColor );
            } );

    TEXT_ATTRIBUTES attributes;
    attributes.m_Angle = aOrient;
    attributes.m_StrokeWidth = aWidth;
    attributes.m_Italic = aItalic;
    attributes.m_Bold = aBold;
    attributes.m_Halign = aH_justify;
    attributes.m_Valign = aV_justify;
    attributes.m_Size = aSize;

    aFont->Draw( &callback_gal, aText, aPos, attributes, aFontMetrics );
}


