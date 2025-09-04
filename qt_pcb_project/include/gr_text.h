#ifndef GR_TEXT_H
#define GR_TEXT_H

#include <font/text_attributes.h>
#include <QtGui/QPainter>

class QPainter;


namespace KIGFX
{
    class COLOR4D;
}

namespace KIFONT
{
    class METRICS;
}

class PLOTTER;

// Clamp pen width to prevent characters from becoming cluttered
int ClampTextPenSize( int aPenSize, int aSize, bool aStrict = false );
float ClampTextPenSize( float aPenSize, int aSize, bool aStrict = false );
int ClampTextPenSize( int aPenSize, const VECTOR2I& aSize, bool aStrict = false );

// Get best pen size for bold text
int GetPenSizeForBold( int aTextSize );
int GetPenSizeForBold( const QSize& aTextSize );

// Get best pen size for demibold text
int GetPenSizeForDemiBold( int aTextSize );
int GetPenSizeForDemiBold( const QSize& aTextSize );

// Get best pen size for normal text
int GetPenSizeForNormal( int aTextSize );
int GetPenSizeForNormal( const QSize& aTextSize );

inline void InferBold( TEXT_ATTRIBUTES* aAttrs )
{
    int    penSize( aAttrs->m_StrokeWidth );
    QSize textSize( aAttrs->m_Size.x, aAttrs->m_Size.y );

    aAttrs->m_Bold = abs( penSize - GetPenSizeForBold( textSize ) )
                   < abs( penSize - GetPenSizeForNormal( textSize ) );
}


// Return the margin for knocking out text
inline int GetKnockoutTextMargin( const VECTOR2I& aSize, int aThickness )
{
    return std::max( KiROUND( aThickness / 2 ), KiROUND( aSize.y / 9.0 ) );
}


// Get the X size of the graphic text
int GRTextWidth( const QString& aText, KIFONT::FONT* aFont, const VECTOR2I& aSize,
                 int aThickness, bool aBold, bool aItalic, const KIFONT::METRICS& aFontMetrics );

// Print graphic text through QPainter
void GRPrintText( QPainter* aPainter, const VECTOR2I& aPos, const KIGFX::COLOR4D& aColor,
                  const QString& aText, const EDA_ANGLE& aOrient, const VECTOR2I& aSize,
                  enum GR_TEXT_H_ALIGN_T aH_justify, enum GR_TEXT_V_ALIGN_T aV_justify,
                  int aWidth, bool aItalic, bool aBold, KIFONT::FONT* aFont,
                  const KIFONT::METRICS& aFontMetrics );


#endif /* GR_TEXT_H */
