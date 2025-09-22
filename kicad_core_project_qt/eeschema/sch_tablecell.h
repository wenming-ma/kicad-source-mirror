
#ifndef SCH_TABLECELL_H
#define SCH_TABLECELL_H

#include <QString>
#include <sch_textbox.h>


class SCH_TABLECELL : public SCH_TEXTBOX
{
public:
    SCH_TABLECELL( int aLineWidth = 0, FILL_T aFillType = FILL_T::NO_FILL );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && SCH_TABLECELL_T == aItem->Type();
    }

    virtual QString GetClass() const override
    {
        return "SCH_TABLECELL";
    }

    QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const override;

    EDA_ITEM* Clone() const override
    {
        return new SCH_TABLECELL( *this );
    }

    void SwapData( SCH_ITEM* aItem ) override;

    int GetRow() const;
    int GetColumn() const;

    /// @return the spreadsheet nomenclature for the cell (ie: B3 for 2nd column, 3rd row)
    QString GetAddr() const;

    int  GetColSpan() const { return m_colSpan; }
    void SetColSpan( int aSpan ) { m_colSpan = aSpan; }

    int  GetRowSpan() const { return m_rowSpan; }
    void SetRowSpan( int aSpan ) { m_rowSpan = aSpan; }

    int GetRowHeight() const;
    void SetRowHeight( int aHeight );

    int GetColumnWidth() const;
    void SetColumnWidth( int aWidth );

    void Print( const SCH_RENDER_SETTINGS* aSettings, int aUnit, int aBodyStyle,
                const VECTOR2I& offset, bool aForceNoFill, bool aDimmed ) override;

    void Plot( PLOTTER* aPlotter, bool aBackground, const SCH_PLOT_OPTS& aPlotOpts,
               int aUnit, int aBodyStyle, const VECTOR2I& aOffset, bool aDimmed ) override;

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    double Similarity( const SCH_ITEM& aOther ) const override;

    bool operator==( const SCH_TABLECELL& aOther ) const;
    bool operator==( const SCH_ITEM& aOther ) const override;

protected:
    int m_colSpan;
    int m_rowSpan;
};


#endif /* SCH_TABLECELL_H */
