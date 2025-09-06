/*
 * This program source code file is part of KICAD, a free EDA CAD application.
 *
 * Copyright (C) 2020 Ian McInerney <ian.s.mcinerney at ieee dot org>
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include <kiplatform/ui.h>
#include <pgm_base.h>
#include <settings/common_settings.h>
#include <widgets/bitmap_button.h>
#include <QtWidgets/QPushButton>
#include <QtGui/QPainter>
#include <QtWidgets/QStyle>
#include <QtWidgets/QStyleOption>
#include <QtWidgets/QApplication>
#include <QtGui/QPaintEvent>
#include <QtGui/QMouseEvent>
#include <QtGui/QFocusEvent>

#define wxCONTROL_SEPARATOR 0x0100
#define wxCONTROL_CURRENT   0x0001
#define wxCONTROL_PRESSED   0x0002
#define wxCONTROL_FOCUSED   0x0004
#define wxCONTROL_DISABLED  0x0008
#define wxCONTROL_CHECKABLE 0x0010
#define wxCONTROL_CHECKED   0x0020
#define wxCONTROL_SELECTED  0x0040


BITMAP_BUTTON::BITMAP_BUTTON( QWidget* aParent, int aId, const QPoint& aPos,
                              const QSize& aSize, int aStyles ) :
        QWidget( aParent ),
        m_isRadioButton( false ),
        m_showBadge( false ),
        m_badgeColor( QColor( 210, 0, 0 ) ), // dark red
        m_badgeTextColor( QColor( "white" ) ),
        m_buttonState( 0 ),
        m_padding( 0 ),
        m_isToolbarButton( false ),
        m_acceptDraggedInClicks( false ),
        m_centerBitmap( true )
{
    m_badgeFont = font();
    m_badgeFont.setPointSize( m_badgeFont.pointSize() - 1 );
    m_badgeFont.setBold( true );

    setupEvents();
}


BITMAP_BUTTON::BITMAP_BUTTON( QWidget* aParent, int aId, const QPixmap& aDummyBitmap,
                              const QPoint& aPos, const QSize& aSize, int aStyles ) :
        QWidget( aParent ),
        m_isRadioButton( false ),
        m_showBadge( false ),
        m_badgeColor( QColor( 210, 0, 0 ) ), // dark red
        m_badgeTextColor( QColor( "white" ) ),
        m_buttonState( 0 ),
        m_padding( 5 ),
        m_isToolbarButton( false ),
        m_acceptDraggedInClicks( false ),
        m_centerBitmap( true )
{
    m_badgeFont = font();
    m_badgeFont.setPointSize( m_badgeFont.pointSize() - 1 );
    m_badgeFont.setBold( true );

    setupEvents();
}


void BITMAP_BUTTON::setupEvents()
{
    // Qt event handling is done through virtual method overrides
    setMouseTracking( true );
    setFocusPolicy( Qt::StrongFocus );
}


BITMAP_BUTTON::~BITMAP_BUTTON()
{
    // Qt automatically handles cleanup
}


QSize BITMAP_BUTTON::sizeHint() const
{
    if( hasFlag( wxCONTROL_SEPARATOR ) )
        return QSize( m_unadjustedMinSize.width() + m_padding * 2, QPushButton().sizeHint().height() );

    return m_unadjustedMinSize + QSize( m_padding * 2, m_padding * 2 );
}


void BITMAP_BUTTON::invalidateBestSize()
{
    updateGeometry();
}


void BITMAP_BUTTON::SetPadding( int aPadding )
{
    m_padding = aPadding;
    invalidateBestSize();
}


void BITMAP_BUTTON::SetBitmap( const QPixmap& aBmp )
{
    m_normalBitmap = aBmp;

    // This is a bit of a hack, but fixes button scaling issues on some platforms when those buttons
    // use KiScaledBitmap.  When that method is retired, this can probably be revisited.
    if( m_isToolbarButton )
    {
        m_unadjustedMinSize = m_normalBitmap.size();
    }
    else
    {
#ifndef _WIN32
        m_unadjustedMinSize = m_normalBitmap.size();
#else
        m_unadjustedMinSize = m_normalBitmap.size();
#endif
    }

    invalidateBestSize();
}


void BITMAP_BUTTON::SetDisabledBitmap( const QPixmap& aBmp )
{
    m_disabledBitmap = aBmp;
}


void BITMAP_BUTTON::AcceptDragInAsClick( bool aAcceptDragIn )
{
    m_acceptDraggedInClicks = aAcceptDragIn;
}


void BITMAP_BUTTON::leaveEvent( QEvent* aEvent )
{
    if( hasFlag( wxCONTROL_CURRENT | wxCONTROL_PRESSED ) )
    {
        clearFlag( wxCONTROL_CURRENT | wxCONTROL_PRESSED );
        update();
    }

    QWidget::leaveEvent( aEvent );
}


#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void BITMAP_BUTTON::enterEvent( QEnterEvent* aEvent )
#else
void BITMAP_BUTTON::enterEvent( QEvent* aEvent )
#endif
{
    if( !hasFlag( wxCONTROL_CURRENT ) )
    {
        setFlag( wxCONTROL_CURRENT );
        update();
    }

    QWidget::enterEvent( aEvent );
}


void BITMAP_BUTTON::focusOutEvent( QFocusEvent* aEvent )
{
    if( hasFlag( wxCONTROL_FOCUSED | wxCONTROL_CURRENT | wxCONTROL_PRESSED | wxCONTROL_SELECTED ) )
    {
        clearFlag( wxCONTROL_FOCUSED | wxCONTROL_CURRENT | wxCONTROL_PRESSED | wxCONTROL_SELECTED );
        update();
    }

    QWidget::focusOutEvent( aEvent );
}


void BITMAP_BUTTON::focusInEvent( QFocusEvent* aEvent )
{
    if( !hasFlag( wxCONTROL_CHECKABLE ) )
    {
        if( !hasFlag( wxCONTROL_FOCUSED ) )
        {
            setFlag( wxCONTROL_FOCUSED );
            update();
        }
    }

    QWidget::focusInEvent( aEvent );
}


void BITMAP_BUTTON::mouseReleaseEvent( QMouseEvent* aEvent )
{
    // Only create a button event when the control is enabled
    // and only accept clicks that came without prior mouse-down if configured
    if( !hasFlag( wxCONTROL_DISABLED )
            && ( m_acceptDraggedInClicks || hasFlag( wxCONTROL_PRESSED | wxCONTROL_FOCUSED ) ) )
    {
        emit clicked();
    }

    clearFlag( wxCONTROL_PRESSED );
    update();

    QWidget::mouseReleaseEvent( aEvent );
}


void BITMAP_BUTTON::mousePressEvent( QMouseEvent* aEvent )
{
    if( hasFlag( wxCONTROL_CHECKABLE ) )
    {
        if( hasFlag( wxCONTROL_CHECKED ) && !m_isRadioButton )
        {
            clearFlag( wxCONTROL_CHECKED );
            emit toggled( false );
        }
        else
        {
            setFlag( wxCONTROL_CHECKED );
            emit toggled( true );
        }
    }
    else
    {
        setFlag( wxCONTROL_PRESSED );
    }

    update();

    QWidget::mousePressEvent( aEvent );
}


void BITMAP_BUTTON::paintEvent( QPaintEvent* aEvent )
{
    bool    darkMode       = KIPLATFORM::UI::IsDarkTheme();
    QColor highlightColor = palette().highlight().color();

    // The drawing rectangle
    QRect    rect( QPoint( 0, 0 ), size() );
    QPainter painter( this );

    if( hasFlag( wxCONTROL_SEPARATOR ) )
    {
        painter.setPen( QPen( palette().midlight().color() ) );
        painter.drawLine( QPoint( size().width() / 2, 0 ), QPoint( size().width() / 2, size().height() ) );
        return;
    }

    // This drawing is done so the button looks the same as an AUI toolbar button
    if( !hasFlag( wxCONTROL_DISABLED ) )
    {
        if( hasFlag( wxCONTROL_PRESSED ) )
        {
            painter.setPen( QPen( highlightColor ) );
            QColor brushColor = highlightColor;
            brushColor.setAlpha( darkMode ? 51 : 100 );  // 20% or 150% lightness equivalent
            painter.setBrush( QBrush( brushColor ) );
            painter.drawRect( rect );
        }
        else if( hasFlag( wxCONTROL_CURRENT | wxCONTROL_FOCUSED ) )
        {
            painter.setPen( QPen( highlightColor ) );
            QColor brushColor = highlightColor;
            brushColor.setAlpha( darkMode ? 102 : 60 );  // 40% or 170% lightness equivalent

            // Checked items need a lighter hover rectangle
            if( hasFlag( wxCONTROL_CHECKED ) )
                brushColor.setAlpha( darkMode ? 128 : 40 );  // 50% or 180% lightness equivalent

            painter.setBrush( QBrush( brushColor ) );
            painter.drawRect( rect );
        }
        else if( hasFlag( wxCONTROL_CHECKED ) )
        {
            painter.setPen( QPen( highlightColor ) );
            QColor brushColor = highlightColor;
            brushColor.setAlpha( darkMode ? 102 : 60 );  // 40% or 170% lightness equivalent
            painter.setBrush( QBrush( brushColor ) );
            painter.drawRect( rect );
        }
    }

    const QPixmap& bmp = hasFlag( wxCONTROL_DISABLED ) ? m_disabledBitmap : m_normalBitmap;

    QPoint drawBmpPos( m_padding, m_padding );
    QPixmap bmpImg;
    QSize bmSize;

    if( !bmp.isNull() )
    {
        bmpImg = bmp;
        bmSize = bmpImg.size();
    }

    if( m_centerBitmap )
    {
        drawBmpPos.setX( ( rect.width() - bmSize.width() ) / 2 );
        drawBmpPos.setY( ( rect.height() - bmSize.height() ) / 2 );
    }

    // Draw the bitmap with the upper-left corner offset by the padding
    if( !bmp.isNull() )
        painter.drawPixmap( drawBmpPos, bmpImg );

    // Draw the badge
    if( m_showBadge )
    {
        painter.setFont( m_badgeFont );

        QSize text_padding( 3, 1 );

        if( m_padding )
            text_padding *= 2;

        QSize box_size = painter.fontMetrics().size( Qt::TextSingleLine, m_badgeText ) + text_padding;
        QSize box_offset = box_size;

        if( m_padding != 0 )
            box_offset += QSize( m_padding / 3, m_padding / 3 );

        painter.setPen( QPen( m_badgeColor ) );
        painter.setBrush( QBrush( m_badgeColor ) );
        painter.drawRoundedRect( QRect( rect.bottomRight() - QPoint( box_offset.width(), box_offset.height() ), 
                                box_size ), 2, 2 );

        painter.setPen( m_badgeTextColor );
        painter.drawText( rect.bottomRight() - QPoint( box_offset.width(), box_offset.height() ) + QPoint( text_padding.width() / 2, text_padding.height() / 2 ),
                         m_badgeText );
    }
}


void BITMAP_BUTTON::setEnabled( bool aEnable )
{
    // If the requested state is already the current state, don't do anything
    if( aEnable != hasFlag( wxCONTROL_DISABLED ) )
        return;

    QWidget::setEnabled( aEnable );

    if( aEnable && hasFlag( wxCONTROL_DISABLED ) )
    {
        clearFlag( wxCONTROL_DISABLED );
        update();
    }

    if( !aEnable && !hasFlag( wxCONTROL_DISABLED ) )
    {
        setFlag( wxCONTROL_DISABLED );
        update();
    }
}


void BITMAP_BUTTON::SetIsCheckButton()
{
    setFlag( wxCONTROL_CHECKABLE );
}


void BITMAP_BUTTON::SetIsRadioButton()
{
    setFlag( wxCONTROL_CHECKABLE );
    m_isRadioButton = true;
}


void BITMAP_BUTTON::SetIsSeparator()
{
    setFlag( wxCONTROL_SEPARATOR | wxCONTROL_DISABLED );
    invalidateBestSize();
}


void BITMAP_BUTTON::Check( bool aCheck )
{
    Q_ASSERT_X( hasFlag( wxCONTROL_CHECKABLE ), "BITMAP_BUTTON", "Button is not a checkButton." );

    if( aCheck && !hasFlag( wxCONTROL_CHECKED ) )
    {
        setFlag( wxCONTROL_CHECKED );
        update();
    }

    if( !aCheck && hasFlag( wxCONTROL_CHECKED ) )
    {
        clearFlag( wxCONTROL_CHECKED );
        update();
    }
}


bool BITMAP_BUTTON::IsChecked() const
{
    Q_ASSERT_X( hasFlag( wxCONTROL_CHECKABLE ), "BITMAP_BUTTON", "Button is not a checkButton." );

    return hasFlag( wxCONTROL_CHECKED );
}

