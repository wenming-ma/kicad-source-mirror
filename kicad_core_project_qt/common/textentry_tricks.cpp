#include <QKeyEvent>
#include <QLineEdit>
#include <QApplication>
#include <QClipboard>
#include <textentry_tricks.h>


void TEXTENTRY_TRICKS::OnCharHook( QLineEdit* aTextEntry, QKeyEvent& aEvent )
{
    if( isCtrl( 'X', aEvent ) )
    {
        aTextEntry->cut();
    }
    else if( isCtrl( 'C', aEvent ) )
    {
        aTextEntry->copy();
    }
    else if( isCtrl( 'V', aEvent ) )
    {
        aTextEntry->paste();
    }
    else if( isCtrl( 'A', aEvent ) )
    {
        aTextEntry->selectAll();
    }
    else if( aEvent.key() == Qt::Key_Backspace )
    {
        int start = aTextEntry->selectionStart();
        int length = aTextEntry->selectedText().length();

        if( length > 0 )
        {
            QString text = aTextEntry->text();
            text.remove( start, length );
            aTextEntry->setText( text );
            aTextEntry->setCursorPosition( start );
        }
        else if( start > 0 )
        {
            QString text = aTextEntry->text();
            text.remove( start - 1, 1 );
            aTextEntry->setText( text );
            aTextEntry->setCursorPosition( start - 1 );
        }
    }
    else if( aEvent.key() == Qt::Key_Delete )
    {
        int start = aTextEntry->selectionStart();
        int length = aTextEntry->selectedText().length();

        if( length > 0 )
        {
            QString text = aTextEntry->text();
            text.remove( start, length );
            aTextEntry->setText( text );
            aTextEntry->setCursorPosition( start );
        }
        else if( start < aTextEntry->text().length() )
        {
            QString text = aTextEntry->text();
            text.remove( start, 1 );
            aTextEntry->setText( text );
        }
    }
    else
    {
        aEvent.accept();
    }
}


bool TEXTENTRY_TRICKS::isCtrl( int aChar, const QKeyEvent& e )
{
    return e.modifiers() & Qt::ControlModifier && e.key() == aChar;
}


bool TEXTENTRY_TRICKS::isShiftCtrl( int aChar, const QKeyEvent& e )
{
    return ( e.modifiers() & Qt::ControlModifier ) && ( e.modifiers() & Qt::ShiftModifier ) && e.key() == aChar;
}
