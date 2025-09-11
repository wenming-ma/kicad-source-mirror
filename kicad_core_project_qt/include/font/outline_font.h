
#ifndef OUTLINE_FONT_H_
#define OUTLINE_FONT_H_

#include <gal/gal.h>
#include <geometry/shape_poly_set.h>
#ifdef _MSC_VER
#include <ft2build.h>
#else
#include <freetype2/ft2build.h>
#endif
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <font/font.h>
#include <font/glyph.h>
#include <font/outline_decomposer.h>
#include <embedded_files.h>

#include <mutex>
#include <QString>

namespace KIFONT
{
class OUTLINE_FONT : public FONT
{
public:

    enum class EMBEDDING_PERMISSION
    {
        INSTALLABLE,
        EDITABLE,
        PRINT_PREVIEW_ONLY,
        RESTRICTED,
        INVALID
    };

    OUTLINE_FONT();

    bool IsOutline() const override { return true; }

    bool IsBold() const override
    {
        return m_face && ( m_fakeBold || ( m_face->style_flags & FT_STYLE_FLAG_BOLD ) );
    }

    bool IsItalic() const override
    {
        return m_face && ( m_fakeItal || ( m_face->style_flags & FT_STYLE_FLAG_ITALIC ) );
    }

    void SetFakeBold()
    {
        m_fakeBold = true;
    }

    void SetFakeItal()
    {
        m_fakeItal = true;
    }

    const QString& GetFileName() const { return m_fontFileName; }

    EMBEDDING_PERMISSION GetEmbeddingPermission() const;

    static OUTLINE_FONT* LoadFont( const QString& aFontFileName, bool aBold, bool aItalic,
                                   const std::vector<QString>* aEmbeddedFiles,
                                   bool aForDrawingSheet );

    double GetInterline( double aGlyphHeight, const METRICS& aFontMetrics ) const override;

    VECTOR2I GetTextAsGlyphs( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                              const QString& aText, const VECTOR2I& aSize,
                              const VECTOR2I& aPosition, const EDA_ANGLE& aAngle, bool aMirror,
                              const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle ) const override;

    void GetLinesAsGlyphs( std::vector<std::unique_ptr<GLYPH>>* aGlyphs, const QString& aText,
                           const VECTOR2I& aPosition, const TEXT_ATTRIBUTES& aAttrs,
                           const METRICS& aFontMetrics ) const;

    const FT_Face& GetFace() const { return m_face; }

#if 0
    void RenderToOpenGLCanvas( KIGFX::OPENGL_FREETYPE& aTarget, const QString& aString,
                               const VECTOR2D& aSize, const QPoint& aPosition,
                               const EDA_ANGLE& aAngle, bool aMirror ) const;
#endif

protected:
    FT_Error loadFace( const QString& aFontFileName, int aFaceIndex );

    BOX2I getBoundingBox( const std::vector<std::unique_ptr<GLYPH>>& aGlyphs ) const;

    VECTOR2I getTextAsGlyphs( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                              const QString& aText, const VECTOR2I& aSize,
                              const VECTOR2I& aPosition, const EDA_ANGLE& aAngle, bool aMirror,
                              const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    VECTOR2I getTextAsGlyphsUnlocked( BOX2I* aBoundingBox,
                                      std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                                      const QString& aText, const VECTOR2I& aSize,
                                      const VECTOR2I& aPosition, const EDA_ANGLE& aAngle,
                                      bool aMirror, const VECTOR2I& aOrigin,
                                      TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    // FreeType variables
    static std::mutex m_freeTypeMutex;
    static FT_Library m_freeType;
    FT_Face           m_face;

    const int         m_faceSize;
    bool              m_fakeBold;
    bool              m_fakeItal;

    bool              m_forDrawingSheet;
    QString           m_fontFileName;

    // cache for glyphs converted to straight segments
    // key is glyph index (FT_GlyphSlot field glyph_index)
    std::map<unsigned int, std::vector<std::vector<VECTOR2D>>> m_contourCache;

    static constexpr double m_outlineFontSizeCompensation = 1.4;
    static constexpr int m_charSizeScaler = 64;
    static constexpr double m_subscriptSuperscriptSize = 0.64;

    static constexpr double m_underlineOffsetScaler = -0.16;

    int faceSize( int aSize ) const
    {
        return aSize * m_charSizeScaler * m_outlineFontSizeCompensation;
    };

    int faceSize() const { return faceSize( m_faceSize ); }

    int subscriptSize( int aSize ) const
    {
        return KiROUND( faceSize( aSize ) * m_subscriptSuperscriptSize );
    }
    int subscriptSize() const { return subscriptSize( m_faceSize ); }

    static constexpr double m_subscriptVerticalOffset   = -0.25;
    static constexpr double m_superscriptVerticalOffset = 0.45;
};

} //namespace KIFONT

#endif // OUTLINE_FONT_H_
