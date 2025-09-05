
#ifndef STROKE_FONT_H
#define STROKE_FONT_H

#include <gal/gal.h>
#include <map>
#include <deque>
#include <algorithm>
#include <core/utf8.h>
#include <math/box2.h>
#include <font/font.h>
#include <QString>

namespace KIGFX
{
class GAL;
}

namespace KIFONT
{
class GAL_API STROKE_FONT : public FONT
{
public:
    STROKE_FONT();

    bool IsStroke() const override { return true; }

    static STROKE_FONT* LoadFont( const QString& aFontName );

    double GetInterline( double aGlyphHeight, const METRICS& aFontMetrics ) const override;

    VECTOR2I GetTextAsGlyphs( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                              const QString& aText, const VECTOR2I& aSize,
                              const VECTOR2I& aPosition, const EDA_ANGLE& aAngle, bool aMirror,
                              const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle ) const override;

private:
    void loadNewStrokeFont( const char* const aNewStrokeFont[], int aNewStrokeFontSize );

private:
    const std::vector<std::shared_ptr<GLYPH>>* m_glyphs;
    const std::vector<BOX2D>*                  m_glyphBoundingBoxes;
    double                                     m_maxGlyphWidth;
};

} //namespace KIFONT

#endif // STROKE_FONT_H
