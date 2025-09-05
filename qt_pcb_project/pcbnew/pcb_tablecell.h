#ifndef PCB_TABLECELL_H
#define PCB_TABLECELL_H

#include <QString>
#include <pcb_textbox.h>


class PCB_TABLECELL : public PCB_TEXTBOX
{
public:
    PCB_TABLECELL( BOARD_ITEM* parent );

    static inline bool ClassOf( const EDA_ITEM* aItem )
    {
        return aItem && PCB_TABLECELL_T == aItem->Type();
    }

    QString GetClass() const override
    {
        return "PCB_TABLECELL";
    }

    virtual QString GetFriendlyName() const override
    {
        return _( "Table Cell" );
    }

    EDA_ITEM* Clone() const override
    {
        return new PCB_TABLECELL( *this );
    }

    int GetRow() const;
    int GetColumn() const;

    // @return the spreadsheet nomenclature for the cell (ie: B3 for 2nd column, 3rd row)
    QString GetAddr() const;

    int  GetColSpan() const { return m_colSpan; }
    void SetColSpan( int aSpan ) { m_colSpan = aSpan; }

    int  GetRowSpan() const { return m_rowSpan; }
    void SetRowSpan( int aSpan ) { m_rowSpan = aSpan; }

    int GetRowHeight() const;
    void SetRowHeight( int aHeight );

    int GetColumnWidth() const;
    void SetColumnWidth( int aWidth );

    QString GetItemDescription( UNITS_PROVIDER* aUnitsProvider, bool aFull ) const override;

    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    double Similarity( const BOARD_ITEM& aBoardItem ) const override;

    bool operator==( const PCB_TABLECELL& aBoardItem ) const;
    bool operator==( const BOARD_ITEM& aBoardItem ) const override;

protected:
    virtual void swapData( BOARD_ITEM* aImage ) override;

protected:
    int m_colSpan;
    int m_rowSpan;
};


#endif /* PCB_TABLECELL_H */
