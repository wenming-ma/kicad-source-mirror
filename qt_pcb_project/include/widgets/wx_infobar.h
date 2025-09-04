// QT_TRANSFORMATION_COMPLETED
#ifndef INFOBAR_H_
#define INFOBAR_H_

#include <functional>
#include <optional>
#include <QEvent>
#include <QFrame>
#include <QTimer>
#include <QWidget>
#include <QBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QObject>
#include <reporter.h>


class QBoxLayout;
class QPushButton;


enum
{
    // ID for the close button on the frame's infobar
    ID_CLOSE_INFOBAR = 2000,
};


// Custom events for infobar - Qt custom events will be used instead
const int KIEVT_SHOW_INFOBAR = QEvent::User + 1;
const int KIEVT_DISMISS_INFOBAR = QEvent::User + 2;

class WX_INFOBAR : public QFrame
{
    Q_OBJECT
public:
    WX_INFOBAR( QWidget* aParent, void* aMgr = nullptr, int aWinid = -1 );

    ~WX_INFOBAR();
    enum class MESSAGE_TYPE
    {
        GENERIC,
        OUTDATED_SAVE,
        DRC_RULES_ERROR,
        DRC_VIOLATION
    };

    MESSAGE_TYPE GetMessageType() const { return m_type; }

    void SetShowTime( int aTime );

    void AddCloseButton( const QString& aTooltip = "Hide this message." );

    void AddButton( QPushButton* aButton );

    void AddButton( QPushButton* aHypertextButton );

    void AddButton( int aId, const QString& aLabel = QString() );

    void RemoveAllButtons();

    bool HasCloseButton() const;

    QPushButton* GetCloseButton() const;

    void SetCallback( std::function<void(void)> aCallback )
    {
        m_callback = aCallback;
    }

    void ShowMessageFor( const QString& aMessage, int aTime, int aFlags = 1,
                         MESSAGE_TYPE aType = WX_INFOBAR::MESSAGE_TYPE::GENERIC );

    void ShowMessage( const QString& aMessage, int aFlags = 1 );

    void ShowMessage( const QString& aMessage, int aFlags, MESSAGE_TYPE aType );

    void Dismiss();

    void QueueShowMessage( const QString& aMessage, int aFlags = 1 );

    void QueueDismiss();

    bool IsLocked()
    {
        return m_updateLock;
    }

signals:
    void messageShown();
    void messageDismissed();
    void closeButtonClicked();

protected slots:
    void onTimer();
    void onCloseButton();

protected:
    void onShowInfoBar( QEvent* aEvent );

    void onDismissInfoBar( QEvent* aEvent );

    void onThemeChange();

    void onSize();

    void updateAuiLayout( bool aShow );

protected:
    int           m_showTime;
    bool          m_updateLock;
    QTimer*       m_showTimer;
    void*         m_auiManager;
    MESSAGE_TYPE  m_type;
    QString       m_message;

    std::optional<std::function<void(void)>> m_callback;
};


class EDA_INFOBAR_PANEL : public QWidget
{
    Q_OBJECT

public:
    EDA_INFOBAR_PANEL( QWidget* aParent, int aId = -1,
                       const QPoint& aPos = QPoint(),
                       const QSize& aSize = QSize( -1,-1 ),
                       long aStyle = 0,
                       const QString& aName = QString() );

    void AddInfoBar( WX_INFOBAR* aInfoBar );

    void AddOtherItem( QWidget* aOtherItem );

protected:
    QBoxLayout* m_mainSizer;
};


class INFOBAR_REPORTER : public REPORTER
{
public:
    INFOBAR_REPORTER( WX_INFOBAR* aInfoBar ) :
            REPORTER(), m_messageSet( false ), m_infoBar( aInfoBar ),
            m_severity( RPT_SEVERITY_UNDEFINED )
    {
    }

    virtual ~INFOBAR_REPORTER() {};

    REPORTER& Report( const QString& aText, SEVERITY aSeverity = RPT_SEVERITY_UNDEFINED ) override;

    bool HasMessage() const override;

    void Finalize();

private:
    bool                      m_messageSet;
    WX_INFOBAR*               m_infoBar;
    std::unique_ptr<QString>  m_message;
    SEVERITY                  m_severity;
};
#endif // INFOBAR_H_
