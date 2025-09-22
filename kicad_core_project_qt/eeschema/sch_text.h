
#ifndef SCH_TEXT_H
#define SCH_TEXT_H


#include <eda_text.h>
#include <sch_item.h>
#include <sch_connection.h>   // for CONNECTION_TYPE
#include <schematic.h>


// UNUSED_SYMBOL: HTML_MESSAGE_BOX destructor - Forward declaration not needed since class is unused
// class HTML_MESSAGE_BOX;

class SCH_TEXT : public SCH_ITEM, public EDA_TEXT
{
public:
    SCH_TEXT( const VECTOR2I& aPos = { 0, 0 }, const QString& aText = QString(),
              SCH_LAYER_ID aLayer = LAYER_NOTES, KICAD_T aType = SCH_TEXT_T );

    SCH_TEXT( const SCH_TEXT& aText );

    ~SCH_TEXT() override { }

    static bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && SCH_TEXT_T == aItem->Type();
    }

    QString GetClass() const override
    {
        return "SCH_TEXT";
    }

    QString GetFriendlyName() const override
    {
        return "Text";
    }

    KIFONT::FONT* GetDrawFont( const RENDER_SETTINGS* aSettings ) const override;

    virtual QString GetShownText( const SCH_SHEET_PATH* aPath, bool aAllowExtraText,
                                  int aDepth = 0 ) const;

    QString GetShownText( bool aAllowExtraText, int aDepth = 0 ) const override
    {
        SCHEMATIC* schematic = Schematic();

        if( schematic )
            return GetShownText( &schematic->CurrentSheet(), aAllowExtraText, aDepth );
        else
            return GetText();
    }

    int GetSchTextSize() const { return GetTextWidth(); }
    void SetSchTextSize( int aSize ) { SetTextSize( VECTOR2I( aSize, aSize ) ); }

    bool IsHypertext() const override
    {
        return HasHyperlink();
    }

    void DoHypertextAction( EDA_DRAW_FRAME* aFrame ) const override;

    void SetExcludedFromSim( bool aExclude ) override { m_excludedFromSim = aExclude; }
    bool GetExcludedFromSim() const override { return m_excludedFromSim; }

    /**
     * This offset depends on the orientation, the type of text, and the area required to
     * draw the associated graphic symbol or to put the text above a wire.
     *
     * @return the offset between the SCH_TEXT position and the text itself position
     */
    virtual VECTOR2I GetSchematicTextOffset( const RENDER_SETTINGS* aSettings ) const;

    void SwapData( SCH_ITEM* aItem ) override;

    const BOX2I GetBoundingBox() const override;

    bool operator<( const SCH_ITEM& aItem ) const override;

    int GetTextOffset( const RENDER_SETTINGS* aSettings = nullptr ) const;

    int GetPenWidth() const override;

    void Move( const VECTOR2I& aMoveVector ) override
    {
        EDA_TEXT::Offset( aMoveVector );
    }

    void NormalizeJustification( bool inverse );

    void MirrorHorizontally( int aCenter ) override;
    void MirrorVertically( int aCenter ) override;
    void Rotate( const VECTOR2I& aCenter, bool aRotateCCW ) override;

    virtual void Rotate90( bool aClockwise );
    virtual void MirrorSpinStyle( bool aLeftRight );

    void BeginEdit( const VECTOR2I& aStartPoint ) override;
    void CalcEdit( const VECTOR2I& aPosition ) override;

    bool Matches( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) const override
    {
        return SCH_ITEM::Matches( GetText(), aSearchData );
    }

    bool Replace( const EDA_SEARCH_DATA& aSearchData, void* aAuxData ) override
    {
        return EDA_TEXT::Replace( aSearchData );
    }

    bool IsReplaceable() const override { return true; }

    std::vector<int> ViewGetLayers() const override;

    QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const override;

    BITMAPS GetMenuImage() const override;

    VECTOR2I GetPosition() const override { return EDA_TEXT::GetTextPos(); }
    void     SetPosition( const VECTOR2I& aPosition ) override
    {
        EDA_TEXT::SetTextPos( aPosition );
    }

    bool HitTest( const VECTOR2I& aPosition, int aAccuracy = 0 ) const override;
    bool HitTest( const BOX2I& aRect, bool aContained, int aAccuracy = 0 ) const override;

    void Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                const VECTOR2I& offset, bool aForceNoFill, bool aDimmed ) override;

    void PrintBackground( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                          const VECTOR2I& aOffset, bool aDimmed ) override {}

    void Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
               int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed ) override;

    EDA_ITEM* Clone() const override
    {
        return new SCH_TEXT( *this );
    }

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    double Similarity( const SCH_ITEM& aItem ) const override;

    bool operator==( const SCH_ITEM& aItem ) const override;

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override;
#endif

    // UNUSED_SYMBOL: HTML_MESSAGE_BOX destructor - Method signature commented out since return type is unused
    // static HTML_MESSAGE_BOX* ShowSyntaxHelp( QWidget* aParentWindow );
    static void* ShowSyntaxHelp( QWidget* aParentWindow );

protected:
    const KIFONT::METRICS& getFontMetrics() const override { return GetFontMetrics(); }

    /**
     * @copydoc SCH_ITEM::compare()
     *
     * The text specific sort order is as follows:
     *      - Text string, case insensitive compare.
     *      - Text horizontal (X) position.
     *      - Text vertical (Y) position.
     *      - Text width.
     *      - Text height.
     */
    int compare( const SCH_ITEM& aOther, int aCompareFlags = 0 ) const override;

protected:
    bool            m_excludedFromSim;
};


#endif /* SCH_TEXT_H */
