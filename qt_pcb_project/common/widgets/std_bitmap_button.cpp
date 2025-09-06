// QT_TRANSFORMATION_COMPLETED
#include "widgets/std_bitmap_button.h"
#include <QPainter>
#include <QStyleOption>

STD_BITMAP_BUTTON::STD_BITMAP_BUTTON( QWidget* aParent, int aId, const QPixmap& aDummyBitmap,
                                      const QPoint& aPos, const QSize& aSize, int aStyle )
    : QWidget( aParent )
{
    if( !aPos.isNull() )
        move( aPos );
    
    if( !aSize.isNull() )
        resize( aSize );
    
    m_bitmap = aDummyBitmap;
    
    // Enable mouse tracking for hover effects
    setMouseTracking( true );
}

STD_BITMAP_BUTTON::~STD_BITMAP_BUTTON()
{
}

void STD_BITMAP_BUTTON::SetBitmap( const QPixmap& aBmp )
{
    m_bitmap = aBmp;
    update();
}

void STD_BITMAP_BUTTON::setEnabled( bool aEnable )
{
    m_bIsEnable = aEnable;
    QWidget::setEnabled( aEnable );
    update();
}

void STD_BITMAP_BUTTON::focusOutEvent( QFocusEvent* aEvent )
{
    m_stateButton = 0;
    update();
    QWidget::focusOutEvent( aEvent );
}

void STD_BITMAP_BUTTON::leaveEvent( QEvent* aEvent )
{
    m_stateButton = 0;
    update();
    QWidget::leaveEvent( aEvent );
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void STD_BITMAP_BUTTON::enterEvent( QEnterEvent* aEvent )
#else
void STD_BITMAP_BUTTON::enterEvent( QEvent* aEvent )
#endif
{
    if( m_bIsEnable )
    {
        m_stateButton = 1;
        update();
    }
    QWidget::enterEvent( aEvent );
}

void STD_BITMAP_BUTTON::mousePressEvent( QMouseEvent* aEvent )
{
    if( m_bIsEnable && aEvent->button() == Qt::LeftButton )
    {
        m_stateButton = 2;
        update();
    }
    QWidget::mousePressEvent( aEvent );
}

void STD_BITMAP_BUTTON::mouseReleaseEvent( QMouseEvent* aEvent )
{
    if( m_bIsEnable && aEvent->button() == Qt::LeftButton )
    {
        bool wasPressed = (m_stateButton == 2);
        m_stateButton = rect().contains( aEvent->pos() ) ? 1 : 0;
        update();
        
        // Emit clicked signal if button was pressed and released inside the widget
        if( wasPressed && rect().contains( aEvent->pos() ) )
        {
            emit clicked();
        }
    }
    QWidget::mouseReleaseEvent( aEvent );
}

void STD_BITMAP_BUTTON::paintEvent( QPaintEvent* aEvent )
{
    QPainter painter( this );
    
    // Draw button background based on state
    QStyleOption opt;
    opt.initFrom( this );
    
    if( !m_bIsEnable )
    {
        opt.state &= ~QStyle::State_Enabled;
    }
    else if( m_stateButton == 2 )
    {
        opt.state |= QStyle::State_Sunken;
    }
    else if( m_stateButton == 1 )
    {
        opt.state |= QStyle::State_MouseOver;
    }
    
    style()->drawPrimitive( QStyle::PE_PanelButtonCommand, &opt, &painter, this );
    
    // Draw the bitmap centered
    if( !m_bitmap.isNull() )
    {
        int x = (width() - m_bitmap.width()) / 2;
        int y = (height() - m_bitmap.height()) / 2;
        
        if( m_stateButton == 2 )
        {
            // Offset when pressed
            x += 1;
            y += 1;
        }
        
        if( !m_bIsEnable )
        {
            // Draw disabled version
            QPixmap disabledPixmap = m_bitmap;
            QPainter p( &disabledPixmap );
            p.setCompositionMode( QPainter::CompositionMode_SourceAtop );
            p.fillRect( disabledPixmap.rect(), QColor( 128, 128, 128, 128 ) );
            p.end();
            painter.drawPixmap( x, y, disabledPixmap );
        }
        else
        {
            painter.drawPixmap( x, y, m_bitmap );
        }
    }
}

void STD_BITMAP_BUTTON::changeEvent( QEvent* aEvent )
{
    if( aEvent->type() == QEvent::EnabledChange )
    {
        m_bIsEnable = isEnabled();
        update();
    }
    QWidget::changeEvent( aEvent );
}