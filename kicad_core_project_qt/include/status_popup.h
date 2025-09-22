
#ifndef __STATUS_POPUP_H_
#define __STATUS_POPUP_H_


#include <math/vector2d.h>
#include <QWidget>
#include <QTimer>
#include <QVBoxLayout>
#include <QLabel>
#include <QPoint>
#include <QColor>

class EDA_DRAW_FRAME;

/**
 * A tiny, headerless popup window used to display useful status (e.g. line length
 * tuning info) next to the mouse cursor.
 */
class STATUS_POPUP: public QWidget
{
public:
    STATUS_POPUP( QWidget* aParent );
    virtual ~STATUS_POPUP() { hide(); }

    virtual void Popup( QWidget* aFocus = nullptr );
    virtual void PopupFor( int aMsecs );
    virtual void Move( const QPoint& aWhere );
    virtual void Move( const VECTOR2I& aWhere );

    /**
     * Hide the popup after a specified time.
     *
     * @param aMsecs is the time expressed in milliseconds
     */
    void Expire( int aMsecs );

    QWidget* GetPanel() { return m_panel; }

protected:
    void updateSize();

    void onCharHook( QKeyEvent& aEvent );

    /// Expire timer even handler.
    void onExpire();

protected:
    QWidget*     m_panel;
    QVBoxLayout* m_topSizer;
    QTimer*      m_expireTimer;
};


/**
 * Extension of #STATUS_POPUP for displaying a single line text.
 */
class STATUS_TEXT_POPUP : public STATUS_POPUP
{
public:
    STATUS_TEXT_POPUP( QWidget* aParent );
    virtual ~STATUS_TEXT_POPUP() {}

    /**
     * Display a text.
     *
     * @param aText text to be displayed.
     */
    void SetText( const QString& aText );

    /**
     * Change text color.
     *
     * @param aColor new text color.
     */
    void SetTextColor( const QColor& aColor );

protected:
    QLabel* m_statusLine;
};


#endif /* __STATUS_POPUP_H_*/
