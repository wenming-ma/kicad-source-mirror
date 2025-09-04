// QT_TRANSFORMATION_COMPLETED
#ifndef STD_BITMAP_BUTTON_H
#define STD_BITMAP_BUTTON_H

#include <kicommon.h>
#include <QWidget>
#include <QPixmap>
#include <QPainter>
#include <QMouseEvent>
#include <QFocusEvent>

class QPushButton;

class KICOMMON_API STD_BITMAP_BUTTON : public QWidget
{
    Q_OBJECT

public:
    // For use with Qt Designer on a sub-classed QPushButton
    STD_BITMAP_BUTTON( QWidget* aParent, int aId, const QPixmap& aDummyBitmap,
                       const QPoint& aPos = QPoint(), const QSize& aSize = QSize(),
                       int aStyle = 0 );

    ~STD_BITMAP_BUTTON();

    void SetBitmap( const QPixmap& aBmp );
    void setEnabled( bool aEnable = true ) override;

protected:
    void focusOutEvent( QFocusEvent* aEvent ) override;
    void leaveEvent( QEvent* aEvent ) override;
    void enterEvent( QEvent* aEvent ) override;
    void mouseReleaseEvent( QMouseEvent* aEvent ) override;
    void mousePressEvent( QMouseEvent* aEvent ) override;
    void paintEvent( QPaintEvent* aEvent ) override;
    void changeEvent( QEvent* aEvent ) override;

private:
    int       m_stateButton  = 0;
    bool      m_bIsEnable    = true;
    QPixmap   m_bitmap;
};

#endif /*STD_BITMAP_BUTTON_H*/
