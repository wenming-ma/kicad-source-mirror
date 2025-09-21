
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef EDA_REORDERABLE_LIST_DIALOG_H
#define EDA_REORDERABLE_LIST_DIALOG_H

#include <eda_reorderable_list_dialog_base.h>

/**
 * A dialog which allows selecting a list of items from a list of available items, and reordering
 * those items.
 */
class EDA_REORDERABLE_LIST_DIALOG : public EDA_REORDERABLE_LIST_DIALOG_BASE
{
public:

    /**
     * @param aParent Pointer to the parent window.
     * @param aTitle The title shown on top.
     * @param aAllItems A list of elements.
     * @param aEnabledItems A list of elements that are already in the "enabled" category.
     */
    EDA_REORDERABLE_LIST_DIALOG( QWidget* aParent, const QString& aTitle,
                                 const std::vector<QString>& aAllItems,
                                 const std::vector<QString>& aEnabledItems );

    const std::vector<QString>& EnabledList() { return m_enabledItems; }

protected:
    void onAddItem() override;
    void onRemoveItem() override;
    void onMoveUp() override;
    void onMoveDown() override;
    void onAvailableListItemSelected() override;
    void onEnabledListItemSelected() override;

private:
    void updateItems();

    bool getSelectedItem( QListWidget* aList, QListWidgetItem*& aInfo );

    std::vector<QString> m_availableItems;
    std::vector<QString> m_enabledItems;

    long m_selectedAvailable;
    long m_selectedEnabled;
};

#endif
