
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

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
    bool TransferDataFromWindow();
    bool TransferDataToWindow();

    void applySettingsToPanel( COMMON_SETTINGS& aSettings );

    void OnTextEditorClick() override;
    void OnPDFViewerClick() override;
    void OnRadioButtonPdfViewer() override;

    /**
     * Event fired when the canvas scale field is modified
     */
    void OnCanvasScaleChange();

    /**
     * Event fired when the canvas auto-scale option is changed
     */
    void OnCanvasScaleAuto() override;

private:
    void ShowFileManagerWidgets( bool aBool );
    void setPdfViewerPathState();
};

#endif //KICAD_DIALOG_SUITE_OPTIONS_H
