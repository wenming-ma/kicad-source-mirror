#ifndef DIALOG_SHIM_
#define DIALOG_SHIM_

#include <kicommon.h>
#include <eda_units.h>
#include <kiway_holder.h>
#include <QDialog>
#include <QEvent>
#include <QKeyEvent>
#include <QPaintEvent>
#include <QCloseEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QFocusEvent>
#include <QHash>
#include <QVector>
#include <core/raii.h>

class EDA_BASE_FRAME;

class QGridEvent;
class QEventLoop;

class KICOMMON_API DIALOG_SHIM : public QDialog, public KIWAY_HOLDER
{
public:
    DIALOG_SHIM( QWidget* aParent, int id, const QString& title,
                 const QPoint& pos = QPoint(-1, -1),
                 const QSize& size = QSize(-1, -1),
                 Qt::WindowFlags style = Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint,
                 const QString& name = QString() );

    ~DIALOG_SHIM();

    void SetInitialFocus( QWidget* aWindow )
    {
        m_initialFocusTarget = aWindow;
    }

    int ShowModal();

    int ShowQuasiModal();      // disable only the parent window, otherwise modal.

    void EndQuasiModal( int retCode );  // End quasi-modal mode

    bool IsQuasiModal() const { return m_qmodal_showing; }

    void PrepareForModalSubDialog();
    void CleanupAfterModalSubDialog();

    void show();

    bool setEnabled( bool enable );

    void paintEvent( QPaintEvent* event );

    void OnModify();
    void ClearModify();

    void SetPosition( const QPoint& aNewPosition );

    EDA_UNITS GetUserUnits() const
    {
        return m_units;
    }

    void SelectAllInTextCtrls( QList<QWidget*>& children );

    void SetupStandardButtons( QHash<int, QString> aLabels = {} );

    static bool IsCtrl( int aChar, const QKeyEvent& e )
    {
        return e.key() == aChar && (e.modifiers() & Qt::ControlModifier) && 
                !(e.modifiers() & Qt::AltModifier) && !(e.modifiers() & Qt::ShiftModifier) && 
                !(e.modifiers() & Qt::MetaModifier);
    }

    static bool IsShiftCtrl( int aChar, const QKeyEvent& e )
    {
        return e.key() == aChar && (e.modifiers() & Qt::ControlModifier) && 
                !(e.modifiers() & Qt::AltModifier) && (e.modifiers() & Qt::ShiftModifier) && 
                !(e.modifiers() & Qt::MetaModifier);
    }

protected:
    void finishDialogSettings();

    void setSizeInDU( int x, int y );

    int horizPixelsFromDU( int x ) const;

    int vertPixelsFromDU( int y ) const;

    void resetSize();

    virtual void keyPressEvent( QKeyEvent* aEvt );

    virtual void TearDownQuasiModal() {}

private:
    void closeEvent( QCloseEvent* aEvent );

    void resizeEvent( QResizeEvent* aEvent );
    void moveEvent( QMoveEvent* aEvent );

    void buttonClicked();

    void onChildSetFocus( QFocusEvent* aEvent );

protected:
    EDA_UNITS              m_units;    // userUnits for display and parsing
    std::string            m_hash_key; // alternate for class_map when classname re-used

    bool                   m_useCalculatedSize;

    bool                   m_firstPaintEvent;
    QWidget*               m_initialFocusTarget;
    bool                   m_isClosing;

    QEventLoop*            m_qmodal_loop;  // points to nested event_loop, NULL means not qmodal and dismissed
    bool                   m_qmodal_showing;
    WINDOW_DISABLER*       m_qmodal_parent_disabler;

    EDA_BASE_FRAME*        m_parentFrame;

    QVector<QWidget*>      m_tabOrder;

    QSize                  m_initialSize;
    bool                   m_userPositioned;
    bool                   m_userResized;

    QHash<QWidget*, QString> m_beforeEditValues;
};

#endif  // DIALOG_SHIM_
