
#ifndef  HOTKEY_CYCLE_POPUP_H
#define  HOTKEY_CYCLE_POPUP_H

#include <eda_view_switcher_base.h>

class EDA_DRAW_FRAME;
class QTimer;

/**
 * Similar to EDA_VIEW_SWITCHER, this dialog is a popup that shows feedback when using a hotkey to
 * cycle through a set of options.  This variant is designed for use with single-stroke hotkeys
 * (rather than chorded hotkeys like Ctrl+Tab) as feedback rather than as an interactive selector.
 */
class HOTKEY_CYCLE_POPUP : public EDA_VIEW_SWITCHER_BASE
{
public:
    HOTKEY_CYCLE_POPUP( EDA_DRAW_FRAME* aParent );

    ~HOTKEY_CYCLE_POPUP();

    void Popup( const QString& aTitle, const QStringList& aItems, int aSelection );

protected:
    bool TryBefore( QEvent& aEvent ) override;

private:
    QTimer* m_showTimer;
    EDA_DRAW_FRAME* m_drawFrame;
};

#endif    // HOTKEY_CYCLE_POPUP_H
