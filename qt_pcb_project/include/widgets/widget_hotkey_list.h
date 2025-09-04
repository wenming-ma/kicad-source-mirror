
// QT_TRANSFORMATION_COMPLETED
#ifndef __widget_hotkey_list__
#define __widget_hotkey_list__

#include <unordered_map>
#include <utility>
#include <vector>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QWidget>
#include <QKeyEvent>
#include <QString>
#include <QPoint>

#include <hotkeys_basic.h>
#include <hotkey_store.h>


class WIDGET_HOTKEY_CLIENT_DATA;

class WIDGET_HOTKEY_LIST : public QTreeWidget
{
    Q_OBJECT

public:
    WIDGET_HOTKEY_LIST( QWidget* aParent, HOTKEY_STORE& aHotkeyStore, bool aReadOnly );

    void ApplyFilterString( const QString& aFilterStr );

    void ResetAllHotkeys( bool aResetToDefault );

    bool TransferDataToControl();

    bool TransferDataFromControl();

    static long MapKeypressToKeycode( const QKeyEvent& aEvent );

signals:
    void HotkeyChanged( TOOL_ACTION* aAction );

protected slots:
    void onItemActivated( QTreeWidgetItem* aItem, int aColumn );
    void onCustomContextMenuRequested( const QPoint& aPos );

protected:
    void editItem( QTreeWidgetItem* aItem, int aEditId );

    void resetItem( QTreeWidgetItem* aItem, int aResetId );

    void onMenu( int aActionId );

    bool resolveKeyConflicts( TOOL_ACTION* aAction, long aKey );

private:
    WIDGET_HOTKEY_CLIENT_DATA* getHKClientData( QTreeWidgetItem* aItem );

    void updateFromClientData();

    void updateShownItems( const QString& aFilterStr );

    void changeHotkey( HOTKEY& aHotkey, long aKey, bool alternate );

    void updateColumnWidths();

private:
    HOTKEY_STORE&  m_hk_store;
    bool           m_readOnly;

    std::unordered_map<long, QString> m_reservedHotkeys;

    QTreeWidgetItem* m_context_menu_item;
};

#endif // __widget_hotkey_list__
