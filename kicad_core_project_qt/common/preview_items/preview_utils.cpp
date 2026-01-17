// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
// Qt transformation of KiCad preview_utils.cpp - wxWidgets to Qt framework migration

#include <preview_items/preview_utils.h>
#include <gal/graphics_abstraction_layer.h>
#include <base_units.h>
#include <gal/painter.h>
#include <view/view.h>
#include <gal/hidpi_gl_canvas.h>
#include <QtCore/QString>
#include <QtCore/QStringList>

double KIGFX::PREVIEW::PreviewOverlayDeemphAlpha( bool aDeemph )
{
    return aDeemph ? 0.5 : 1.0;
}


QString KIGFX::PREVIEW::DimensionLabel( const QString& prefix, double aVal,
                                        const EDA_IU_SCALE& aIuScale, EDA_UNITS aUnits,
                                        bool aIncludeUnits )
{
    QString str;

    if( prefix.size() )
        str += prefix + ": ";

    QString fmtStr;

    // show a sane precision for the preview, which doesn't need to be accurate down to the
    // nanometre
    switch( aUnits )
    {
    case EDA_UNITS::UM:       fmtStr = "%.0f"; break;  // 1um
    case EDA_UNITS::MM:       fmtStr = "%.3f"; break;  // 1um
    case EDA_UNITS::CM:       fmtStr = "%.4f"; break;  // 1um
    case EDA_UNITS::MILS:     fmtStr = "%.1f"; break;  // 0.1mil
    case EDA_UNITS::INCH:     fmtStr = "%.4f"; break;  // 0.1mil
    case EDA_UNITS::DEGREES:  fmtStr = "%.1f"; break;  // 0.1deg
    case EDA_UNITS::PERCENT:  fmtStr = "%.1f"; break;  // 0.1%
    case EDA_UNITS::UNSCALED: fmtStr = "%f";   break;
    }

    str += QString::asprintf( fmtStr.toStdString().c_str(), EDA_UNIT_UTILS::UI::ToUserUnit( aIuScale, aUnits, aVal ) );

    if( aIncludeUnits )
        str += EDA_UNIT_UTILS::GetText( aUnits );

    return str;
}


KIGFX::PREVIEW::TEXT_DIMS KIGFX::PREVIEW::GetConstantGlyphHeight( KIGFX::GAL* aGal,
                                                                  int aRelativeSize )
{
    constexpr double aspectRatio = 1.0;
    constexpr double hdpiSizes[] = { 7,  8,  9,  11,  13, 14, 16 };
    constexpr double sizes[] =     { 8, 10, 12,  14,  15, 16, 18 };

    double height;
    double thicknessFactor;
    double shadowFactor;
    double linePitchFactor;

    HIDPI_GL_CANVAS* canvas = dynamic_cast<HIDPI_GL_CANVAS*>( aGal );

    if( canvas && canvas->GetScaleFactor() > 1 )
    {
        height = hdpiSizes[ 3 + aRelativeSize ];
        thicknessFactor = 0.15;
        shadowFactor = 0.10;
        linePitchFactor = 1.7;
    }
    else
    {
        height = sizes[ 3 + aRelativeSize ];
        thicknessFactor = 0.20;
        shadowFactor = 0.15;
        linePitchFactor = 1.9;
    }

    height /= aGal->GetWorldScale();

    TEXT_DIMS textDims;

    textDims.GlyphSize = VECTOR2I( height * aspectRatio, height );
    textDims.StrokeWidth = height * thicknessFactor;
    textDims.ShadowWidth = height * shadowFactor;
    textDims.LinePitch = height * linePitchFactor;

    return textDims;
}


KIGFX::COLOR4D KIGFX::PREVIEW::GetShadowColor( const KIGFX::COLOR4D& aColor )
{
    if( aColor.GetBrightness() > 0.5 )
        return COLOR4D::BLACK;
    else
        return COLOR4D::WHITE;
}


void KIGFX::PREVIEW::DrawTextNextToCursor( KIGFX::VIEW* aView, const VECTOR2D& aCursorPos,
                                           const VECTOR2D& aTextQuadrant,
                                           const QStringList& aStrings,
                                           bool aDrawingDropShadows )
{
    KIGFX::GAL*      gal = aView->GetGAL();

    GAL_SCOPED_ATTRS settings( *gal, GAL_SCOPED_ATTRS::STROKE_FILL );

    KIFONT::FONT*    font = KIFONT::FONT::GetFont();

    // constant text size on screen
    TEXT_DIMS        textDims = GetConstantGlyphHeight( gal );
    TEXT_ATTRIBUTES  textAttrs;

    // radius string goes on the right of the cursor centre line with a small horizontal
    // offset (enough to keep clear of a system cursor if present)
    VECTOR2D         textPos = aCursorPos;

    bool             viewFlipped = gal->IsFlippedX();

    // if the text goes above the cursor, shift it up
    if( aTextQuadrant.y > 0 )
        textPos.y -= textDims.LinePitch * ( aStrings.size() + 1 );

    if( aTextQuadrant.x < 0 )
    {
        if( viewFlipped )
            textAttrs.m_Halign = GR_TEXT_H_ALIGN_RIGHT;
        else
            textAttrs.m_Halign = GR_TEXT_H_ALIGN_LEFT;

        textPos.x += 15.0 / gal->GetWorldScale();
    }
    else
    {
        if( viewFlipped )
            textAttrs.m_Halign = GR_TEXT_H_ALIGN_LEFT;
        else
            textAttrs.m_Halign = GR_TEXT_H_ALIGN_RIGHT;

        textPos.x -= 15.0 / gal->GetWorldScale();
    }

    // text is left (or right) aligned, so a shadow text need a small offset to be draw
    // around the basic text
    int shadowXoffset = aDrawingDropShadows ? textDims.ShadowWidth : 0;

    // Due to the fact a shadow text is drawn left or right aligned,
    // it needs an offset = shadowWidth/2 to be drawn at the same place as normal text
    // But for some reason we need to slightly modify this offset
    // for a better look for KiCad font (better alignment of shadow shape)
    const float adjust = 1.2f;      // Value chosen after tests
    shadowXoffset *= adjust;

    if( ( textAttrs.m_Halign == GR_TEXT_H_ALIGN_LEFT ) != viewFlipped )
        textPos.x -= shadowXoffset;
    else
        textPos.x += shadowXoffset;

    gal->SetStrokeColor( aView->GetPainter()->GetSettings()->GetLayerColor( LAYER_AUX_ITEMS ) );
    textAttrs.m_Mirrored = viewFlipped; // Prevent text flipping when view is flipped
    textAttrs.m_Size = textDims.GlyphSize;
    textAttrs.m_StrokeWidth = textDims.StrokeWidth;
    gal->SetIsFill( false );
    gal->SetIsStroke( true );

    if( aDrawingDropShadows )
    {
        textAttrs.m_StrokeWidth = textDims.StrokeWidth + ( 2 * textDims.ShadowWidth );
        gal->SetStrokeColor( GetShadowColor( gal->GetStrokeColor() ) );
    }

    // write strings top-to-bottom
    for( const QString& str : aStrings )
    {
        textPos.y += textDims.LinePitch;
        font->Draw( gal, str, textPos, textAttrs, KIFONT::METRICS::Default() );
    }
}

// Qt transformation completed - wxWidgets to Qt framework migration
