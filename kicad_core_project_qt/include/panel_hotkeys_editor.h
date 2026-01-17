#ifndef PANEL_HOTKEYS_EDITOR_H
#define PANEL_HOTKEYS_EDITOR_H

#include <hotkeys_basic.h>
#include <hotkey_store.h>

#include <widgets/resettable_panel.h>
#include <widgets/widget_hotkey_list.h>

#include <vector>

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QLineEdit>


class QWidget;
class QVBoxLayout;
class TOOL_MANAGER;
class QLineEdit;


class PANEL_HOTKEYS_EDITOR : public RESETTABLE_PANEL
{
public:
    PANEL_HOTKEYS_EDITOR( EDA_BASE_FRAME* aFrame, QWidget* aWindow, bool aReadOnly );
    ~PANEL_HOTKEYS_EDITOR();

    std::vector<TOOL_ACTION*>& ActionsList() { return m_actions; }

    virtual bool TransferDataToWindow();
    virtual bool TransferDataFromWindow();

    void ResetPanel() override;

    QString GetResetTooltip() const override
    {
        return QStringLiteral( "Reset all hotkeys to the built-in KiCad defaults" );
    }

private:
    void installButtons( QVBoxLayout* aSizer );

    void OnFilterSearch();

    void ImportHotKeys();

    void dumpHotkeys();

    QLineEdit* m_filterSearch;

protected:
    EDA_BASE_FRAME*            m_frame;
    bool                       m_readOnly;

    std::vector<TOOL_ACTION*>  m_actions;
    HOTKEY_STORE               m_hotkeyStore;
    WIDGET_HOTKEY_LIST*        m_hotkeyListCtrl;
};


#endif  // PANEL_HOTKEYS_EDITOR_H
