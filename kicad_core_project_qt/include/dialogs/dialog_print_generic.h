
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef DIALOG_PRINT_GENERIC_H
#define DIALOG_PRINT_GENERIC_H

#include <dialogs/dialog_print_generic_base.h>
#include <QtWidgets/QDoubleValidator>
#include <QtWidgets/QLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtGui/QCloseEvent>
#include <widgets/unit_binder.h>

class EDA_DRAW_FRAME;
class APP_SETTINGS_BASE;
struct PRINTOUT_SETTINGS;
class QPageSetupDialog;
class QPrintPreviewWidget;
class QPrinter;

class DIALOG_PRINT_GENERIC : public DIALOG_PRINT_GENERIC_BASE
{
public:
    DIALOG_PRINT_GENERIC( EDA_DRAW_FRAME* aParent, PRINTOUT_SETTINGS* aSettings );
    virtual ~DIALOG_PRINT_GENERIC();

    /**
     * Set 'print border and title block' to a requested value and hides the
     * corresponding checkbox.
     */
    void ForcePrintBorder( bool aValue );

protected:
    /**
     * Create a printout with a requested title.
     */
    virtual QPrintPreviewWidget* createPrintout( const QString& aTitle ) = 0;

    virtual void saveSettings();

    QLayout* getMainSizer()
    {
        return m_bUpperSizer;
    }

    QGridLayout* getOptionsSizer()
    {
        return m_gbOptionsSizer;
    }

    QGroupBox* getOptionsBox()
    {
        return m_sbOptionsSizer->groupBox();
    }

    /**
     * Return scale value selected in the dialog.
     *
     * If this value is outside limits, it will be clamped.
     */
    double getScaleValue();

    /**
    * Select a corresponding scale radio button and update custom scale value if needed.
    *
    * @param aValue is the scale value to be selected (0 stands for fit-to-page).
    */
    void setScaleValue( double aValue );

    // There is no TransferDataFromWindow() so options are saved
    // even if the dialog is closed without printing
    bool TransferDataToWindow() override;

private:
    void onPageSetup() override;
    void onPrintPreview() override;
    void onPrintButtonClick() override;
    void onCancelButtonClick() override;

    // Needed to save the dialogs settings as TransferDataFromWindow()
    // is not called for 'Cancel' button that closes the window.
    void closeEvent( QCloseEvent* event ) override;

    void onSetCustomScale() override;

    void initPrintData();

protected:
    APP_SETTINGS_BASE* m_config;

    PRINTOUT_SETTINGS* m_settings;

private:
    QDoubleValidator* m_scaleValidator;

    static QPrinter* s_PrintData;
    static QPageSetupDialog* s_pageSetupData;
};

#endif // DIALOG_PRINT_GENERIC_H
