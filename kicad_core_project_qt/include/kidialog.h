
#ifndef KIDIALOG_H_
#define KIDIALOG_H_

#include <kicommon.h>
#include <QMessageBox>
#include <QString>
#include <QWidget>

// Helper class to create more flexible dialogs, including 'do not show again' checkbox handling.
class KICOMMON_API KIDIALOG : public QMessageBox
{
public:
    // Dialog type. Selects appropriate icon and default dialog title
    enum KD_TYPE { KD_NONE, KD_INFO, KD_QUESTION, KD_WARNING, KD_ERROR };

    KIDIALOG( QWidget* aParent, const QString& aMessage, const QString& aCaption,
              QMessageBox::StandardButtons aButtons = QMessageBox::Ok );
    KIDIALOG( QWidget* aParent, const QString& aMessage, KD_TYPE aType,
              const QString& aCaption = "" );

    bool SetOKCancelLabels( const QString& ok, const QString& cancel );

    // Shows the 'do not show again' checkbox.
    void DoNotShowCheckbox( QString file, int line );

    // Checks the 'do not show again' setting for the dialog.
    bool DoNotShowAgain() const;
    void ForceShowAgain();

    void show() override;
    int exec() override;

protected:
    // Helper functions for QMessageBox constructor
    static QString getCaption( KD_TYPE aType, const QString& aCaption );
    static QMessageBox::Icon getIcon( KD_TYPE aType );

protected:
    unsigned long m_hash;               // Unique id
    bool          m_cancelMeansCancel;  // If the Cancel button is renamed then it should be
                                        // saved by the DoNotShowAgain checkbox.  If it's really
                                        // a cancel then it should not.
};


#endif /* KIDIALOG_H_ */
