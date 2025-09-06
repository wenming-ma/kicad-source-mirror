// QT_TRANSFORMATION_COMPLETED

#ifndef BITMAP_BUTTON_H_
#define BITMAP_BUTTON_H_

#include <kicommon.h>
#include <QWidget>
#include <QPixmap>
#include <QColor>
#include <QFont>
#include <QSize>
#include <QPoint>
#include <QString>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QEvent>
#include <QFocusEvent>
#include <QObject>
#include <QEnterEvent>


class KICOMMON_API BITMAP_BUTTON : public QWidget
{
    Q_OBJECT
public:
    BITMAP_BUTTON( QWidget* aParent, int aId, const QPoint& aPos = QPoint(),
                   const QSize& aSize = QSize(),
                   int aStyles = 0 );

    BITMAP_BUTTON( QWidget* aParent, int aId, const QPixmap& aDummyBitmap,
                   const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                   int aStyles = 0 );

    ~BITMAP_BUTTON();

    void SetPadding( int aPadding );

    void SetBitmap( const QPixmap& aBmp );

    void SetDisabledBitmap( const QPixmap& aBmp );

    void setEnabled( bool aEnable = true );

    void SetIsCheckButton();

    void SetIsRadioButton();

    void Check( bool aCheck = true );

    bool IsChecked() const;

    void SetIsSeparator();

    void AcceptDragInAsClick( bool aAcceptDragIn = true );

    void SetShowBadge( bool aShowBadge ) { m_showBadge = aShowBadge; }

    void SetBadgeText( const QString& aText ) { m_badgeText = aText; }

    void SetBadgeColors( const QColor& aBadgeColor, const QColor& aBadgeTextColor )
    {
        m_badgeColor = aBadgeColor;
        m_badgeTextColor = aBadgeTextColor;
    }

    void SetBitmapCentered( bool aCentered = true )
    {
        m_centerBitmap = aCentered;
    }

    void SetIsToolbarButton( bool aIsToolbar = true ) { m_isToolbarButton = aIsToolbar; }
    bool IsToolbarButton() const { return m_isToolbarButton; }

signals:
    void clicked();
    void toggled( bool checked );

protected:
    void setupEvents();

    void leaveEvent( QEvent* aEvent ) override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void enterEvent( QEnterEvent* aEvent ) override;
#else
    void enterEvent( QEvent* aEvent ) override;
#endif
    void focusOutEvent( QFocusEvent* aEvent ) override;
    void focusInEvent( QFocusEvent* aEvent ) override;
    void mouseReleaseEvent( QMouseEvent* aEvent ) override;
    void mousePressEvent( QMouseEvent* aEvent ) override;
    void paintEvent( QPaintEvent* aEvent ) override;

    virtual QSize sizeHint() const override;

    void setFlag( int aFlag )
    {
        m_buttonState |= aFlag;
    }

    void clearFlag( int aFlag )
    {
        m_buttonState &= ~aFlag;
    }

    bool hasFlag( int aFlag ) const
    {
        return m_buttonState & aFlag;
    }

    void invalidateBestSize();

private:
    QPixmap   m_normalBitmap;
    QPixmap   m_disabledBitmap;

    bool      m_isRadioButton;
    bool      m_showBadge;
    QString   m_badgeText;
    QColor    m_badgeColor;
    QColor    m_badgeTextColor;
    QFont     m_badgeFont;
    int       m_buttonState;
    int       m_padding;
    QSize     m_unadjustedMinSize;
    bool      m_isToolbarButton;

    bool      m_acceptDraggedInClicks;
    bool      m_centerBitmap;
};

#endif /*BITMAP_BUTTON_H_*/
