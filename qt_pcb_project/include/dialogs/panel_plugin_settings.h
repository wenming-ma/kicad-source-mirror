#ifndef KICAD_PANEL_PLUGIN_SETTINGS_H
#define KICAD_PANEL_PLUGIN_SETTINGS_H

#include <dialogs/panel_plugin_settings_base.h>

class PAGED_DIALOG;


class PANEL_PLUGIN_SETTINGS : public PANEL_PLUGIN_SETTINGS_BASE
{
public:
    PANEL_PLUGIN_SETTINGS( QWidget* aParent );

    void ResetPanel() override;

protected:
    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

    void OnPythonInterpreterChanged( QFileDialogEvent& event ) override;
    void OnBtnDetectAutomaticallyClicked( QEvent& aEvent ) override;
    void OnEnableApiChecked( QEvent& aEvent ) override;

private:
    void updateApiStatusText();
    void validatePythonInterpreter();

    bool m_pythonInterpreterValid;
};

#endif //KICAD_PANEL_PLUGIN_SETTINGS_H
