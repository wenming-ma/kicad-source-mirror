
#ifndef KICAD_PANEL_MOUSE_SETTINGS_H
#define KICAD_PANEL_MOUSE_SETTINGS_H

#include <dialogs/panel_mouse_settings_base.h>


class COMMON_SETTINGS;
class PAGED_DIALOG;


struct SCROLL_MOD_SET
{
    int zoom;
    int panh;
    int panv;
    bool zoomReverse;
    bool panHReverse;
};


class PANEL_MOUSE_SETTINGS : public PANEL_MOUSE_SETTINGS_BASE
{
public:
    PANEL_MOUSE_SETTINGS( QWidget* aParent );

    ~PANEL_MOUSE_SETTINGS();

    void ResetPanel() override;

protected:
    bool TransferDataFromWindow() override;

    bool TransferDataToWindow() override;

    void OnScrollRadioButton( QEvent& event ) override;
    void onMouseDefaults( QEvent& event ) override;
    void onTrackpadDefaults( QEvent& event ) override;

private:
    void applySettingsToPanel( const COMMON_SETTINGS& aSettings );

    SCROLL_MOD_SET getScrollModSet();

    void updateScrollModButtons();

    bool isScrollModSetValid( const SCROLL_MOD_SET& aSet );

private:
    SCROLL_MOD_SET m_currentScrollMod;
};


#endif
