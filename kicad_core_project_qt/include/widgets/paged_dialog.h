// QT_TRANSFORMATION_COMPLETED
#ifndef PAGED_DIALOG_H
#define PAGED_DIALOG_H

#include <dialog_shim.h>
#include <widgets/wx_treebook.h>
#include <widgets/qt_infobar.h>
#include <QWidget>
#include <QString>
#include <QSize>
#include <QPushButton>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <vector>


class WX_TREEBOOK;

class PAGED_DIALOG : public DIALOG_SHIM
{
    Q_OBJECT

public:
    PAGED_DIALOG( QWidget* aParent, const QString& aTitle, bool aShowReset, bool aShowOpenFolder,
                  const QString& aAuxiliaryAction = QString(),
                  const QSize&   aInitialSize = QSize() );
    ~PAGED_DIALOG() override;

    WX_TREEBOOK* GetTreebook() { return m_treebook; }

    void SetInitialPage( const QString& aPage, const QString& aParentPage = QString() );

    void SetModified() { m_modified = true; }

    void SetError( const QString& aMessage, const QString& aPageName, int aCtrlId, int aRow = -1,
                   int aCol = -1 );

    void SetError( const QString& aMessage, QWidget* aPage, QWidget* aCtrl, int aRow = -1,
                   int aCol = -1 );

    void UpdateResetButton( int aPage );

    static PAGED_DIALOG* GetDialog( QWidget* aWindow );

protected:
    void finishInitialization();

    bool TransferDataFromWindow() override;

    virtual void onAuxiliaryAction();
    virtual void onResetButton();
    virtual void onOpenPreferencesButton();
    virtual void onPageChanged( int aEvent );
    virtual void onPageChanging( int aEvent );
    virtual void onCharHook( QKeyEvent* aEvent );

    WX_TREEBOOK* m_treebook;
    QPushButton* m_auxiliaryButton;
    QPushButton* m_resetButton;
    QPushButton* m_openPrefsDirButton;
    QT_INFOBAR*  m_infoBar;

private:
    QString     m_title;
    bool        m_modified;

    QVBoxLayout* m_buttonsSizer;

    std::vector<bool> m_macHack;
};


#endif //PAGED_DIALOG_H
