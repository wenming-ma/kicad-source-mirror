
#ifndef DS_PROXY_UNDO_ITEM_H
#define DS_PROXY_UNDO_ITEM_H

#include <QString>
#include <eda_item.h>
#include <title_block.h>
#include <page_info.h>


class DS_PROXY_UNDO_ITEM : public EDA_ITEM
{
public:
    DS_PROXY_UNDO_ITEM( const EDA_DRAW_FRAME* aFrame );

    /*
     * Restores the saved drawing sheet layout to the global drawing sheet record, and the saved
     * page info and title blocks to the given frame.  The WS_DRAW_ITEMs are rehydrated and
     * installed in aView if it is not null (ie: if we're in the PageLayout Editor).
     */
    void Restore( EDA_DRAW_FRAME* aFrame, KIGFX::VIEW* aView = nullptr );

#if defined(DEBUG)
    /// @copydoc EDA_ITEM::Show()
    void Show( int x, std::ostream& st ) const override { }
#endif

    QString GetClass() const override
    {
        return "DS_PROXY_UNDO_ITEM";
    }

protected:
    TITLE_BLOCK m_titleBlock;
    PAGE_INFO   m_pageInfo;
    QString     m_layoutSerialization;
    int         m_selectedDataItem;
    int         m_selectedDrawItem;
};

#endif /* DS_PROXY_UNDO_ITEM_H */
