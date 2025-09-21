
#ifndef  EDA_LIST_DIALOG_H
#define  EDA_LIST_DIALOG_H


#include <../common/dialogs/eda_list_dialog_base.h>


class EDA_DRAW_FRAME;

/**
 * A dialog which shows:
 *  - a list of elements for selection,
 *  - a text control to display help or info about the selected item.
 *  - 2 buttons (OK and Cancel)
 *
 */
class EDA_LIST_DIALOG : public EDA_LIST_DIALOG_BASE
{
public:

    /**
     * @param aParent Pointer to the parent window.
     * @param aTitle The title shown on top.
     * @param aItemHeaders an optional array containing the column header names for the dialog.
     * @param aItemList A QStringList of the list of elements.
     * @param aPreselectText An item name if an item must be preselected.
     */
    EDA_LIST_DIALOG( QWidget* aParent, const QString& aTitle, const QStringList& aItemHeaders,
                     const std::vector<QStringList>& aItemList,
                     const QString& aPreselectText = QString(),
                     bool aSortList = true );

    EDA_LIST_DIALOG( QWidget* aParent, const QString& aTitle, bool aSortList = true );

    void SetListLabel( const QString& aLabel );
    void SetOKLabel( const QString& aLabel );
    void HideFilter();

    void Append( const QStringList& aItemStr );
    void InsertItems( const std::vector<QStringList>& aItemList, int aPosition = 0 );

    /**
     * Return the selected text from \a aColumn in the QListWidget in the dialog.
     *
     * @param aColumn is the column to return the text from.
     * @return the selected text from \a aColumn.
     */
    QString GetTextSelection( int aColumn = 0 );

    long GetSelection();

    bool Show( bool show ) override;

protected:
    void initDialog( const QStringList& aItemHeaders, const std::vector<QStringList>& aItemList,
                     const QString& aPreselectText);

private:
    virtual void onSize( QResizeEvent& event ) override;
    void onListItemActivated( QItemSelection& event ) override;
    void textChangeInFilterBox(const QString& text) override;

    void sortList();

private:
    // The list of items, locally stored
    std::vector<QStringList> m_itemsList;
    bool                       m_sortList;
};


#endif    // EDA_LIST_DIALOG_H
