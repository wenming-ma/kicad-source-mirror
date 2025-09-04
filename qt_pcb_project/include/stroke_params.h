
#pragma once

#include <map>
#include <bitmaps.h>
#include <units_provider.h>
#include <gal/color4d.h>
#include <QString>
#include <geometry/shape.h>

class OUTPUTFORMATTER;
class MSG_PANEL_ITEM;

namespace KIGFX
{
class RENDER_SETTINGS;
}


/**
 * Dashed line types.
 */
enum class LINE_STYLE
{
    DEFAULT    = -1,
    SOLID      = 0,
    FIRST_TYPE = SOLID,
    DASH,
    DOT,
    DASHDOT,
    DASHDOTDOT,
    LAST_TYPE = DASHDOTDOT
};


struct LINE_STYLE_DESC
{
    QString       name;
    const BITMAPS bitmap;
};


// A cover of LINE_STYLE for the properties manager (so that it can have different
// strings from the normal LINE_STYLE)
enum class WIRE_STYLE
{
    DEFAULT    = -1,
    SOLID      = 0,
    DASH,
    DOT,
    DASHDOT,
    DASHDOTDOT
};



/**
 * Conversion map between LINE_STYLE values and style names displayed.
 */
extern const std::map<LINE_STYLE, struct LINE_STYLE_DESC> lineTypeNames;


#define DEFAULT_LINE_STYLE_LABEL _( "Solid" )
#define DEFAULT_WIRE_STYLE_LABEL _( "Default" )
#define INDETERMINATE_STYLE _( "Leave unchanged" )


/**
 * Simple container to manage line stroke parameters.
 */
class STROKE_PARAMS
{
public:
    STROKE_PARAMS( int aWidth = 0, LINE_STYLE aLineStyle = LINE_STYLE::DEFAULT,
                   const KIGFX::COLOR4D& aColor = KIGFX::COLOR4D::UNSPECIFIED ) :
            m_width( aWidth ),
            m_lineStyle( aLineStyle ),
            m_color( aColor )
    {
    }

    int GetWidth() const { return m_width; }
    void SetWidth( int aWidth ) { m_width = aWidth; }

    LINE_STYLE GetLineStyle() const { return m_lineStyle; }
    void       SetLineStyle( LINE_STYLE aLineStyle ) { m_lineStyle = aLineStyle; }

    KIGFX::COLOR4D GetColor() const { return m_color; }
    void SetColor( const KIGFX::COLOR4D& aColor ) { m_color = aColor; }

    bool operator!=( const STROKE_PARAMS& aOther ) const
    {
        return m_width != aOther.m_width
                || m_lineStyle != aOther.m_lineStyle
                || m_color != aOther.m_color;
    }

    void Format( OUTPUTFORMATTER* out, const EDA_IU_SCALE& aIuScale ) const;

    void GetMsgPanelInfo( UNITS_PROVIDER* aUnitsProvider, std::vector<MSG_PANEL_ITEM>& aList,
                          bool aIncludeStyle = true, bool aIncludeWidth = true );

    // Helper functions

    static QString GetLineStyleToken( LINE_STYLE aStyle );

    static void Stroke( const SHAPE* aShape, LINE_STYLE aLineStyle, int aWidth,
                        const KIGFX::RENDER_SETTINGS* aRenderSettings,
                        const std::function<void( const VECTOR2I& a,
                                                  const VECTOR2I& b )>& aStroker );

private:
    int            m_width;
    LINE_STYLE     m_lineStyle;
    KIGFX::COLOR4D m_color;
};
