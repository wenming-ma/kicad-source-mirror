
#ifndef PREVIEW_PREVIEW_UTILS__H_
#define PREVIEW_PREVIEW_UTILS__H_

#include <eda_units.h>
#include <gal/color4d.h>
#include <math/vector2d.h>
#include <QString>
#include <QStringList>

namespace KIGFX
{
class GAL;
class VIEW;

namespace PREVIEW
{

struct TEXT_DIMS
{
    VECTOR2I GlyphSize;
    int      StrokeWidth;
    int      ShadowWidth;
    double   LinePitch;
};

/**
 * Default alpha of "de-emphasised" features (like previously locked-in lines.
 */
double PreviewOverlayDeemphAlpha( bool aDeemph = true );


/**
 * Get a formatted string showing a dimension to a sane precision with an optional prefix and
 * unit suffix.
 */
QString DimensionLabel( const QString& prefix, double aVal, const EDA_IU_SCALE& aIuScale,
                        EDA_UNITS aUnits, bool aIncludeUnits = true );

/**
 * Set the GAL glyph height to a constant scaled value, so that it always looks the same on screen.
 *
 * @param aGal the GAL to draw on.
 * @param aRelativeSize similar to HTML font sizes; 0 will give a standard size while +1 etc.
 *                      will give larger and -1 etc. will give smaller.
 * @returns the text widths for the resulting glyph size.
 */
TEXT_DIMS GetConstantGlyphHeight( KIGFX::GAL* aGal, int aRelativeSize = 0 );

COLOR4D GetShadowColor( const COLOR4D& aColor );

/**
 * Draw strings next to the cursor.
 *
 * The GAL attribute context will be restored to its original state after this function is called.
 *
 * @param aGal the GAL to draw on.
 * @param aCursorPos the position of the cursor to draw next to.
 * @param aTextQuadrant a vector pointing to the quadrant to draw the text in.
 * @param aStrings list of strings to draw, top to bottom.
 */
void DrawTextNextToCursor( KIGFX::VIEW* aView, const VECTOR2D& aCursorPos,
                           const VECTOR2D& aTextQuadrant, const QStringList& aStrings,
                           bool aDrawingDropShadows );

} // namespace PREVIEW
} // namespace KIGFX

#endif  // PREVIEW_PREVIEW_UTILS__H_
