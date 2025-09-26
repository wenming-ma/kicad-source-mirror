
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24

#ifndef WX_PANEL_H
#define WX_PANEL_H

#include <QWidget>
#include <QPaintEvent>
#include <gal/color4d.h>

class WX_PANEL : public QWidget
{
public:
    WX_PANEL( QWidget* parent = nullptr, const QPoint& pos = QPoint(),
              const QSize& size = QSize( -1,-1 ), Qt::WindowFlags flags = Qt::Widget,
              const QString& name = QString() );

    ~WX_PANEL();

    void SetBorders( bool aLeft, bool aRight, bool aTop, bool aBottom )
    {
        m_leftBorder = aLeft;
        m_rightBorder = aRight;
        m_topBorder = aTop;
        m_bottomBorder = aBottom;
    }

    void SetBorderColor( const KIGFX::COLOR4D& aColor )
    {
        m_borderColor = aColor;
    }

protected:
    void paintEvent( QPaintEvent* event ) override;

private:
    bool   m_leftBorder;
    bool   m_rightBorder;
    bool   m_topBorder;
    bool   m_bottomBorder;

    KIGFX::COLOR4D m_borderColor;
};


#endif //WX_PANEL_H
