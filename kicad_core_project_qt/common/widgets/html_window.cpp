/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2021 Mikołaj Wielgus <wielgusmikolaj@gmail.com>
 * Copyright The KiCad Developers, see AUTHORS.TXT for contributors.
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

#include <QMenu>
#include <QClipboard>
#include <QApplication>
#include <QGuiApplication>
#include <QPalette>
#include <widgets/html_window.h>


HTML_WINDOW::HTML_WINDOW( QWidget* aParent, int aId, const QPoint& aPos,
                          const QSize& aSize, long aStyle, const QString& aName ) :
        QTextBrowser( aParent )
{
    Q_UNUSED( aId );
    Q_UNUSED( aStyle );
    
    setObjectName( aName );
    
    if( !aPos.isNull() )
        move( aPos );
    
    if( !aSize.isNull() )
        resize( aSize );
    
    // Set up for HTML rendering
    setOpenExternalLinks( false );
    setReadOnly( true );
    
    // Connect theme change signal
    connect( qApp, &QGuiApplication::paletteChanged,
             this, [this](const QPalette&) { onThemeChanged(); } );
}


HTML_WINDOW::~HTML_WINDOW()
{
}


bool HTML_WINDOW::SetPage( const QString& aSource )
{
    m_pageSource = aSource;

    QPalette palette = QApplication::palette();
    QColor fgColor = palette.color( QPalette::WindowText );
    QColor bgColor = palette.color( QPalette::Window );
    QColor linkColor = palette.color( QPalette::Link );

    QString html = QString( "<html>\n<body text='%1' bgcolor='%2' link='%3'>\n" )
                          .arg( fgColor.name() )
                          .arg( bgColor.name() )
                          .arg( linkColor.name() );
    html.append( aSource );
    html.append( "\n</body>\n</html>" );

    setHtml( html );
    return true;
}


bool HTML_WINDOW::AppendToPage( const QString& aSource )
{
    return SetPage( m_pageSource + aSource );
}


void HTML_WINDOW::ThemeChanged()
{
    SetPage( m_pageSource );
}


void HTML_WINDOW::onThemeChanged()
{
    ThemeChanged();
}


void HTML_WINDOW::onRightClick( QMouseEvent* event )
{
    Q_UNUSED( event );
    
    QMenu popup( this );
    QAction* copyAction = popup.addAction( tr( "Copy" ) );
    copyAction->setShortcut( QKeySequence::Copy );
    
    connect( copyAction, &QAction::triggered, this, [this]() {
        QClipboard* clipboard = QApplication::clipboard();
        if( clipboard && textCursor().hasSelection() )
        {
            clipboard->setText( textCursor().selectedText() );
        }
    });
    
    popup.exec( QCursor::pos() );
}


void HTML_WINDOW::contextMenuEvent( QContextMenuEvent* event )
{
    QMouseEvent mouseEvent( QEvent::MouseButtonPress, 
                            event->pos(), 
                            event->globalPos(),
                            Qt::RightButton, 
                            Qt::RightButton, 
                            Qt::NoModifier );
    onRightClick( &mouseEvent );
}