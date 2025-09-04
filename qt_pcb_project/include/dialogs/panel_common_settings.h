
#ifndef KICAD_DIALOG_SUITE_OPTIONS_H
#define KICAD_DIALOG_SUITE_OPTIONS_H

#include <dialogs/panel_common_settings_base.h>


class COMMON_SETTINGS;
class DIALOG_SHIM;
class STEPPED_SLIDER;


class PANEL_COMMON_SETTINGS : public PANEL_COMMON_SETTINGS_BASE
{
public:
    PANEL_COMMON_SETTINGS( QWidget* aParent );
    ~PANEL_COMMON_SETTINGS() override;

    void ResetPanel() override;

protected:
    bool TransferDataFromWindow() override;
    bool TransferDataToWindow() override;

    void applySettingsToPanel( COMMON_SETTINGS& aSettings );

    void OnTextEditorClick( QEvent& event ) override;
    void OnPDFViewerClick( QEvent& event ) override;
    void OnRadioButtonPdfViewer( QEvent& event ) override;

    void OnCanvasScaleChange( QEvent& aEvent );

    void OnCanvasScaleAuto( QEvent& aEvent ) override;

private:
    void ShowFileManagerWidgets( bool aBool );
    void setPdfViewerPathState();
};

#endif //KICAD_DIALOG_SUITE_OPTIONS_H
