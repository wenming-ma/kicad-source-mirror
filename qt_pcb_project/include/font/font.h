
#ifndef FONT_H_
#define FONT_H_

#include <gal/gal.h>
#include <iostream>
#include <map>
#include <algorithm>
#include <QString>
#include <QStringList>
#include <font/glyph.h>
#include <font/text_attributes.h>

namespace KIGFX
{
class GAL;
}


enum TEXT_STYLE
{
    BOLD        = 1,
    ITALIC      = 1 << 1,
    SUBSCRIPT   = 1 << 2,
    SUPERSCRIPT = 1 << 3,
    OVERBAR     = 1 << 4,
    UNDERLINE   = 1 << 5
};


// Tilt factor for italic style
static constexpr double ITALIC_TILT = 1.0 / 8;


using TEXT_STYLE_FLAGS = unsigned int;


inline bool IsBold( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::BOLD;
}


inline bool IsItalic( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::ITALIC;
}


inline bool IsSuperscript( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::SUPERSCRIPT;
}


inline bool IsSubscript( TEXT_STYLE_FLAGS aFlags )
{
    return aFlags & TEXT_STYLE::SUBSCRIPT;
}


namespace KIFONT
{
class GAL_API METRICS
{
public:
    double GetOverbarVerticalPosition( double aGlyphHeight ) const
    {
        return aGlyphHeight * m_OverbarHeight;
    }

    double GetUnderlineVerticalPosition( double aGlyphHeight ) const
    {
        return aGlyphHeight * m_UnderlineOffset;
    }

    double GetInterline( double aFontHeight ) const
    {
        return aFontHeight * m_InterlinePitch;
    }

    static const METRICS& Default();

public:
    double m_InterlinePitch  =  1.68;
    double m_OverbarHeight   =  1.23;
    double m_UnderlineOffset = -0.16;
};

class GAL_API FONT
{
public:
    explicit FONT();

    virtual ~FONT()
    { }

    virtual bool IsStroke() const  { return false; }
    virtual bool IsOutline() const { return false; }
    virtual bool IsBold() const    { return false; }
    virtual bool IsItalic() const  { return false; }

    static FONT* GetFont( const QString& aFontName = QString(), bool aBold = false,
                          bool aItalic = false,
                          const std::vector<QString>* aEmbeddedFiles = nullptr,
                          bool aForDrawingSheet = false );
    static bool IsStroke( const QString& aFontName );

    const QString& GetName() const { return m_fontName; };
    inline const char* NameAsToken() const { return GetName().toUtf8().constData(); }

    void Draw( KIGFX::GAL* aGal, const QString& aText, const VECTOR2I& aPosition,
               const VECTOR2I& aCursor, const TEXT_ATTRIBUTES& aAttributes,
               const METRICS& aFontMetrics ) const;

    void Draw( KIGFX::GAL* aGal, const QString& aText, const VECTOR2I& aPosition,
               const TEXT_ATTRIBUTES& aAttributes, const METRICS& aFontMetrics ) const
    {
        Draw( aGal, aText, aPosition, VECTOR2I( 0, 0 ), aAttributes, aFontMetrics );
    }

    VECTOR2I StringBoundaryLimits( const QString& aText, const VECTOR2I& aSize, int aThickness,
                                   bool aBold, bool aItalic, const METRICS& aFontMetrics ) const;

    void LinebreakText( QString& aText, int aColumnWidth, const VECTOR2I& aGlyphSize,
                        int aThickness, bool aBold, bool aItalic ) const;

    virtual double GetInterline( double aGlyphHeight, const METRICS& aFontMetrics ) const = 0;

    virtual VECTOR2I GetTextAsGlyphs( BOX2I* aBBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                                      const QString& aText, const VECTOR2I& aSize,
                                      const VECTOR2I& aPosition, const EDA_ANGLE& aAngle,
                                      bool aMirror, const VECTOR2I& aOrigin,
                                      TEXT_STYLE_FLAGS aTextStyle ) const = 0;

protected:
    inline unsigned linesCount( const QString& aText ) const
    {
        if( aText.isEmpty() )
            return 0;
        else
            return aText.count('\n') + 1;
    }

    void drawSingleLineText( KIGFX::GAL* aGal, BOX2I* aBoundingBox, const QString& aText,
                             const VECTOR2I& aPosition, const VECTOR2I& aSize,
                             const EDA_ANGLE& aAngle, bool aMirror, const VECTOR2I& aOrigin,
                             bool aItalic, bool aUnderline, const METRICS& aFontMetrics ) const;

    VECTOR2I boundingBoxSingleLine( BOX2I* aBBox, const QString& aText, const VECTOR2I& aPosition,
                                    const VECTOR2I& aSize, bool aItalic,
                                    const METRICS& aFontMetrics ) const;

    void getLinePositions( const QString& aText, const VECTOR2I& aPosition,
                           QStringList& aTextLines, std::vector<VECTOR2I>& aPositions,
                           std::vector<VECTOR2I>& aExtents, const TEXT_ATTRIBUTES& aAttrs,
                           const METRICS& aFontMetrics ) const;

    VECTOR2I drawMarkup( BOX2I* aBoundingBox, std::vector<std::unique_ptr<GLYPH>>* aGlyphs,
                         const QString& aText, const VECTOR2I& aPosition,
                         const VECTOR2I& aSize, const EDA_ANGLE& aAngle, bool aMirror,
                         const VECTOR2I& aOrigin, TEXT_STYLE_FLAGS aTextStyle,
                         const METRICS& aFontMetrics ) const;

    void wordbreakMarkup( std::vector<std::pair<QString, int>>* aWords, const QString& aText,
                          const VECTOR2I& aSize, TEXT_STYLE_FLAGS aTextStyle ) const;

private:
    static FONT* getDefaultFont();

protected:
    QString     m_fontName;
    QString     m_fontFileName;

private:
    static FONT* s_defaultFont;

    static std::map< std::tuple<QString, bool, bool, bool>, FONT* > s_fontMap;
};

} //namespace KIFONT


inline std::ostream& operator<<(std::ostream& os, const KIFONT::FONT& aFont)
{
    os << "[Font \"" << aFont.GetName() << "\"" << ( aFont.IsStroke() ? " stroke" : "" )
       << ( aFont.IsOutline() ? " outline" : "" ) << ( aFont.IsBold() ? " bold" : "" )
       << ( aFont.IsItalic() ? " italic" : "" ) << "]";
    return os;
}


inline std::ostream& operator<<(std::ostream& os, const KIFONT::FONT* aFont)
{
    os << *aFont;
    return os;
}

#endif // FONT_H_
