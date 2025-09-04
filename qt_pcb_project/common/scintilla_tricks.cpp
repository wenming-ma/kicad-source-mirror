// QT_TRANSFORMATION_COMPLETED
// QT_TRANSFORMATION_COMPLETED

#include <string_utils.h>
#include <scintilla_tricks.h>
#include <widgets/wx_grid.h>
#include <widgets/ui_common.h>
#include <gal/color4d.h>
#include <dialog_shim.h>
#include <QClipboard>
#include <QApplication>
#include <QStyleHints>
#include <QKeyEvent>
#include <QWidget>
#include <QTimer>
#include <QClipboard>
#include <QApplication>
#include <QStyleHints>
#include <QKeyEvent>
#include <QWidget>
#include <QTimer>
#include <confirm.h>

SCINTILLA_TRICKS::SCINTILLA_TRICKS( QsciScintilla* aScintilla, const QString& aBraces,
SCINTILLA_TRICKS::SCINTILLA_TRICKS( QsciScintilla* aScintilla, const QString& aBraces,
                                    bool aSingleLine,
                                    std::function<void( QKeyEvent& )> onAcceptFn,
                                    std::function<void( QKeyEvent& )> onCharAddedFn ) :
                                    std::function<void( QKeyEvent& )> onAcceptFn,
                                    std::function<void( QKeyEvent& )> onCharAddedFn ) :
        m_te( aScintilla ),
        m_braces( aBraces ),
        m_lastCaretPos( -1 ),
        m_lastSelStart( -1 ),
        m_lastSelEnd( -1 ),
        m_suppressAutocomplete( false ),
        m_singleLine( aSingleLine ),
        m_onAcceptFn( std::move( onAcceptFn ) ),
        m_onCharAddedFn( std::move( onCharAddedFn ) )
{
    // Always use LF as eol char, regardless the platform
    m_te->setEolMode( QsciScintilla::EolUnix );
    m_te->setEolMode( QsciScintilla::EolUnix );

    // A hack which causes Scintilla to auto-size the text editor canvas
    // See: https://github.com/jacobslusser/ScintillaNET/issues/216
    m_te->setScrollWidth( 1 );
    m_te->setScrollWidthTracking( true );
    m_te->setScrollWidth( 1 );
    m_te->setScrollWidthTracking( true );

    if( m_singleLine )
    {
        m_te->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_te->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_te->setVerticalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
        m_te->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
    }

    setupStyles();

    // Set up autocomplete
    m_te->setAutoCompletionCaseSensitivity( false );
    m_te->setAutoCompletionThreshold( 1 );
    m_te->setAutoCompletionCaseSensitivity( false );
    m_te->setAutoCompletionThreshold( 1 );

    if( aBraces.length() >= 2 )
        m_te->setAutoCompletionFillups( QString( m_braces[1] ) );
    if( aBraces.length() >= 2 )
        m_te->setAutoCompletionFillups( QString( m_braces[1] ) );

    // Hook up events
    connect( m_te, &QsciScintilla::cursorPositionChanged, this, &SCINTILLA_TRICKS::onScintillaUpdateUI );
    connect( m_te, &QsciScintilla::textChanged, this, &SCINTILLA_TRICKS::onModified );
    connect( m_te, &QsciScintilla::cursorPositionChanged, this, &SCINTILLA_TRICKS::onScintillaUpdateUI );
    connect( m_te, &QsciScintilla::textChanged, this, &SCINTILLA_TRICKS::onModified );

    // Handle autocomplete
    connect( m_te, &QsciScintilla::textChanged, this, &SCINTILLA_TRICKS::onChar );
    connect( m_te, &QsciScintilla::textChanged, this, &SCINTILLA_TRICKS::onChar );

    // Install event filter for key events
    m_te->installEventFilter( this );
    // Install event filter for key events
    m_te->installEventFilter( this );
}


void SCINTILLA_TRICKS::onThemeChanged()
void SCINTILLA_TRICKS::onThemeChanged()
{
    setupStyles();
}


void SCINTILLA_TRICKS::setupStyles()
{
    QPalette palette = m_te->palette();
    KIGFX::COLOR4D foreground( palette.color( QPalette::Text ) );
    KIGFX::COLOR4D background( palette.color( QPalette::Base ) );
    KIGFX::COLOR4D highlight( palette.color( QPalette::Highlight ) );
    KIGFX::COLOR4D highlightText( palette.color( QPalette::HighlightedText ) );
    QPalette palette = m_te->palette();
    KIGFX::COLOR4D foreground( palette.color( QPalette::Text ) );
    KIGFX::COLOR4D background( palette.color( QPalette::Base ) );
    KIGFX::COLOR4D highlight( palette.color( QPalette::Highlight ) );
    KIGFX::COLOR4D highlightText( palette.color( QPalette::HighlightedText ) );

    m_te->setColor( foreground.ToColour(), QsciScintilla::STYLE_DEFAULT );
    m_te->setPaper( background.ToColour(), QsciScintilla::STYLE_DEFAULT );
    m_te->clearStyles();
    m_te->setColor( foreground.ToColour(), QsciScintilla::STYLE_DEFAULT );
    m_te->setPaper( background.ToColour(), QsciScintilla::STYLE_DEFAULT );
    m_te->clearStyles();

    // Scintilla doesn't handle alpha channel, which at least OSX uses in some highlight colours,
    // such as "graphite".
    highlight = highlight.Mix( background, highlight.a ).WithAlpha( 1.0 );
    highlightText = highlightText.Mix( background, highlightText.a ).WithAlpha( 1.0 );

    m_te->setSelectionForegroundColor( highlightText.ToColour() );
    m_te->setSelectionBackgroundColor( highlight.ToColour() );
    m_te->setCaretForegroundColor( foreground.ToColour() );
    m_te->setSelectionForegroundColor( highlightText.ToColour() );
    m_te->setSelectionBackgroundColor( highlight.ToColour() );
    m_te->setCaretForegroundColor( foreground.ToColour() );

    if( !m_singleLine )
    {
        // Set a monospace font with a tab width of 4.  This is the closest we can get to having
        // Scintilla mimic the stroke font's tab positioning.
        QFont fixedFont = KIUI::GetMonospacedUIFont();
        QFont fixedFont = KIUI::GetMonospacedUIFont();

        for( int i = 0; i < QsciScintilla::STYLE_MAX; ++i )
            m_te->setFont( fixedFont, i );
        for( int i = 0; i < QsciScintilla::STYLE_MAX; ++i )
            m_te->setFont( fixedFont, i );

        m_te->setTabWidth( 4 );
        m_te->setTabWidth( 4 );
    }

    // Set up the brace highlighting.  Scintilla doesn't handle alpha, so we construct our own
    // 20% wash by blending with the background.
    KIGFX::COLOR4D braceText = foreground;
    KIGFX::COLOR4D braceHighlight = braceText.Mix( background, 0.2 );

    m_te->setColor( highlightText.ToColour(), QsciScintilla::STYLE_BRACELIGHT );
    m_te->setPaper( braceHighlight.ToColour(), QsciScintilla::STYLE_BRACELIGHT );
    m_te->setColor( QColor( Qt::red ), QsciScintilla::STYLE_BRACEBAD );
    m_te->setColor( highlightText.ToColour(), QsciScintilla::STYLE_BRACELIGHT );
    m_te->setPaper( braceHighlight.ToColour(), QsciScintilla::STYLE_BRACELIGHT );
    m_te->setColor( QColor( Qt::red ), QsciScintilla::STYLE_BRACEBAD );
}


bool isCtrlSlash( QKeyEvent* aEvent )
bool isCtrlSlash( QKeyEvent* aEvent )
{
    if( !( aEvent->modifiers() & Qt::ControlModifier ) || ( aEvent->modifiers() & Qt::MetaModifier ) )
    if( !( aEvent->modifiers() & Qt::ControlModifier ) || ( aEvent->modifiers() & Qt::MetaModifier ) )
        return false;

    if( aEvent->text() == "/" )
    if( aEvent->text() == "/" )
        return true;

    // OK, now the keyboard layout hacks start.
    // OK, now the keyboard layout hacks start.
    // Many Latin America and European keyboards have have the / over the 7.  We know that
    // some systems mess this up and return Shift+7 through the key event.  However, other
    // some systems mess this up and return Shift+7 through the key event.  However, other
    // keyboards (such as France and Belgium) have 7 in the shifted position, so a Shift+7
    // *could* be legitimate.

    // However, we *are* checking Ctrl, so to assume any Shift+7 is a Ctrl-/ really only
    // disallows Ctrl+Shift+7 from doing something else, which is probably OK.  (This routine
    // is only used in the Scintilla editor, not in the rest of KiCad.)

    // The other main shifted location of / is over : (France and Belgium), so we'll sacrifice
    // Ctrl+Shift+: too.

    if( ( aEvent->modifiers() & Qt::ShiftModifier ) && ( aEvent->text() == "7" || aEvent->text() == ":" ) )
    if( ( aEvent->modifiers() & Qt::ShiftModifier ) && ( aEvent->text() == "7" || aEvent->text() == ":" ) )
        return true;

    // A few keyboards have / in an Alt position.  Since we're expressly not checking Alt for
    // up or down, those should work.  However, if they don't, there's room below for yet
    // another hack....

    return false;
}


void SCINTILLA_TRICKS::onChar( QKeyEvent& aEvent )
void SCINTILLA_TRICKS::onChar( QKeyEvent& aEvent )
{
    m_onCharAddedFn( aEvent );
}


void SCINTILLA_TRICKS::onModified()
void SCINTILLA_TRICKS::onModified()
{
    if( m_singleLine )
    {
        QString curr_text = m_te->text();
        QString curr_text = m_te->text();

        if( curr_text.contains( "\n" ) || curr_text.contains( "\r" ) )
        if( curr_text.contains( "\n" ) || curr_text.contains( "\r" ) )
        {
            // Scintilla won't allow us to call setText() from within this event processor,
            // Scintilla won't allow us to call setText() from within this event processor,
            // so we have to delay the processing.
            QTimer::singleShot( 0, [this]()
            QTimer::singleShot( 0, [this]()
                       {
                           QString text = m_te->text();
                           int line, index;
                           m_te->getCursorPosition( &line, &index );
                           QString text = m_te->text();
                           int line, index;
                           m_te->getCursorPosition( &line, &index );

                           text.replace( "\n", "" );
                           text.replace( "\r", "" );
                           m_te->setText( text );
                           m_te->setCursorPosition( line, index > 0 ? index - 1 : 0 );
                           text.replace( "\n", "" );
                           text.replace( "\r", "" );
                           m_te->setText( text );
                           m_te->setCursorPosition( line, index > 0 ? index - 1 : 0 );
                       } );
        }
    }

    int line, index;
    m_te->getCursorPosition( &line, &index );
    if( m_singleLine || line == 0 )
    int line, index;
    m_te->getCursorPosition( &line, &index );
    if( m_singleLine || line == 0 )
    {
        // If the font is larger than the height of a single-line text box we can get issues
        // with the text disappearing every other character due to dodgy scrolling behaviour.
        QTimer::singleShot( 0, [this]()
        QTimer::singleShot( 0, [this]()
                   {
                       m_te->ensureCursorVisible();
                       m_te->ensureLineVisible( 0 );
                       m_te->ensureCursorVisible();
                       m_te->ensureLineVisible( 0 );
                   } );
    }
}


void SCINTILLA_TRICKS::onCharHook( QKeyEvent& aEvent )
void SCINTILLA_TRICKS::onCharHook( QKeyEvent& aEvent )
{
    QString c = aEvent.text();
    QString c = aEvent.text();

    if( m_te->isListActive() )
    if( m_te->isListActive() )
    {
        if( aEvent.key() == Qt::Key_Escape )
        if( aEvent.key() == Qt::Key_Escape )
        {
            m_te->cancelList();
            m_te->cancelList();
            m_suppressAutocomplete = true; // Don't run autocomplete again on the next char...
        }
        else if( aEvent.key() == Qt::Key_Return || aEvent.key() == Qt::Key_Enter )
        else if( aEvent.key() == Qt::Key_Return || aEvent.key() == Qt::Key_Enter )
        {
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int start = m_te->positionFromLineIndex( line, 0 );
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int start = m_te->positionFromLineIndex( line, 0 );

            // QScintilla handles autocomplete internally
            // QScintilla handles autocomplete internally

            int finish;
            m_te->getCursorPosition( &line, &index );
            finish = m_te->positionFromLineIndex( line, index );
            int finish;
            m_te->getCursorPosition( &line, &index );
            finish = m_te->positionFromLineIndex( line, index );

            if( finish > start )
            {
                // Select the last substitution token (if any) in the autocompleted text
                QString text = m_te->text( line );
                int selStart = text.indexOf( "<", index );
                int selEnd = text.indexOf( ">", selStart + 1 );
                QString text = m_te->text( line );
                int selStart = text.indexOf( "<", index );
                int selEnd = text.indexOf( ">", selStart + 1 );

                if( selStart >= 0 && selEnd > selStart )
                {
                    int startPos = m_te->positionFromLineIndex( line, selStart );
                    int endPos = m_te->positionFromLineIndex( line, selEnd + 1 );
                    m_te->setSelection( startPos, endPos );
                }
                if( selStart >= 0 && selEnd > selStart )
                {
                    int startPos = m_te->positionFromLineIndex( line, selStart );
                    int endPos = m_te->positionFromLineIndex( line, selEnd + 1 );
                    m_te->setSelection( startPos, endPos );
                }
            }
        }
        else
        {
            // Let QScintilla handle the event
            // Let QScintilla handle the event
        }

        return;
    }

#ifdef __APPLE__
    if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_Space )
#ifdef __APPLE__
    if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_Space )
#else
    if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Space )
    if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Space )
#endif
    {
        m_suppressAutocomplete = false;

        QKeyEvent event( QEvent::KeyPress, Qt::Key_Space, Qt::ControlModifier, " " );
        QKeyEvent event( QEvent::KeyPress, Qt::Key_Space, Qt::ControlModifier, " " );
        m_onCharAddedFn( event );

        return;
    }

    if( !aEvent.text().isEmpty() && !aEvent.text().at(0).isLetter() )
    if( !aEvent.text().isEmpty() && !aEvent.text().at(0).isLetter() )
        m_suppressAutocomplete = false;

    if( ( aEvent.key() == Qt::Key_Return || aEvent.key() == Qt::Key_Enter )
        && ( m_singleLine || ( aEvent.modifiers() & Qt::ShiftModifier ) ) )
    if( ( aEvent.key() == Qt::Key_Return || aEvent.key() == Qt::Key_Enter )
        && ( m_singleLine || ( aEvent.modifiers() & Qt::ShiftModifier ) ) )
    {
        m_onAcceptFn( aEvent );
    }
    else if( ConvertSmartQuotesAndDashes( &c ) )
    {
        m_te->insert( c );
        m_te->insert( c );
    }
    else if( aEvent.key() == Qt::Key_Tab )
    else if( aEvent.key() == Qt::Key_Tab )
    {
        QWidget* ancestor = m_te->parentWidget();
        QWidget* ancestor = m_te->parentWidget();

        while( ancestor && !dynamic_cast<WX_GRID*>( ancestor ) )
            ancestor = ancestor->parentWidget();
            ancestor = ancestor->parentWidget();

        if( aEvent.modifiers() & Qt::ControlModifier )
        if( aEvent.modifiers() & Qt::ControlModifier )
        {
            bool forward = !( aEvent.modifiers() & Qt::ShiftModifier );
            bool forward = !( aEvent.modifiers() & Qt::ShiftModifier );

            if( DIALOG_SHIM* dlg = dynamic_cast<DIALOG_SHIM*>( m_te->window() ) )
                dlg->NavigateIn( forward ? 1 : 0 );
            if( DIALOG_SHIM* dlg = dynamic_cast<DIALOG_SHIM*>( m_te->window() ) )
                dlg->NavigateIn( forward ? 1 : 0 );
        }
        else if( dynamic_cast<WX_GRID*>( ancestor ) )
        {
            WX_GRID* grid = static_cast<WX_GRID*>( ancestor );
            int      row = grid->GetGridCursorRow();
            int      col = grid->GetGridCursorCol();

            if( aEvent.modifiers() & Qt::ShiftModifier )
            if( aEvent.modifiers() & Qt::ShiftModifier )
            {
                if( col > 0 )
                {
                    col--;
                }
                else if( row > 0 )
                {
                    col = (int) grid->GetNumberCols() - 1;

                    if( row > 0 )
                        row--;
                    else
                        row = (int) grid->GetNumberRows() - 1;
                }
            }
            else
            {
                if( col < (int) grid->GetNumberCols() - 1 )
                {
                    col++;
                }
                else if( row < grid->GetNumberRows() - 1 )
                {
                    col = 0;

                    if( row < grid->GetNumberRows() - 1 )
                        row++;
                    else
                        row = 0;
                }
            }

            grid->SetGridCursor( row, col );
        }
        else
        {
            m_te->indent();
            m_te->indent();
        }
    }
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Z )
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Z )
    {
        m_te->undo();
        m_te->undo();
    }
    else if( ( aEvent.modifiers() == ( Qt::ShiftModifier | Qt::ControlModifier ) && aEvent.key() == Qt::Key_Z )
            || ( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Y ) )
    else if( ( aEvent.modifiers() == ( Qt::ShiftModifier | Qt::ControlModifier ) && aEvent.key() == Qt::Key_Z )
            || ( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_Y ) )
    {
        m_te->redo();
        m_te->redo();
    }
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_A )
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_A )
    {
        m_te->selectAll();
        m_te->selectAll();
    }
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_X )
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_X )
    {
        m_te->cut();
        m_te->cut();

        QClipboard* clipboard = QApplication::clipboard();
        // Allow data to be available after closing KiCad by ensuring clipboard is flushed
        QClipboard* clipboard = QApplication::clipboard();
        // Allow data to be available after closing KiCad by ensuring clipboard is flushed
    }
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_C )
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_C )
    {
        m_te->copy();
        m_te->copy();

        QClipboard* clipboard = QApplication::clipboard();
        // Allow data to be available after closing KiCad by ensuring clipboard is flushed
        QClipboard* clipboard = QApplication::clipboard();
        // Allow data to be available after closing KiCad by ensuring clipboard is flushed
    }
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_V )
    else if( aEvent.modifiers() == Qt::ControlModifier && aEvent.key() == Qt::Key_V )
    {
        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();

        QClipboard* clipboard = QApplication::clipboard();

        if( clipboard->mimeData()->hasText() )
        {
            QString str = clipboard->text();
        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();

        QClipboard* clipboard = QApplication::clipboard();

        if( clipboard->mimeData()->hasText() )
        {
            QString str = clipboard->text();

            ConvertSmartQuotesAndDashes( &str );
            ConvertSmartQuotesAndDashes( &str );

            if( m_singleLine )
            {
                str.replace( "\n", QString() );
                str.replace( "\r", QString() );
            }
            if( m_singleLine )
            {
                str.replace( "\n", QString() );
                str.replace( "\r", QString() );
            }

            m_te->beginUndoAction();
            m_te->insert( str );
            m_te->endUndoAction();
            m_te->beginUndoAction();
            m_te->insert( str );
            m_te->endUndoAction();
        }
    }
    else if( aEvent.key() == Qt::Key_Backspace )
    else if( aEvent.key() == Qt::Key_Backspace )
    {
        if( aEvent.modifiers() == Qt::ControlModifier )
#ifdef __APPLE__
            m_te->setSelection( m_te->positionFromLineIndex( m_te->getCursorPosition(), 0 ), m_te->SendScintilla( QsciScintilla::SCI_GETCURRENTPOS ) );
        else if( aEvent.modifiers() == Qt::AltModifier )
        if( aEvent.modifiers() == Qt::ControlModifier )
#ifdef __APPLE__
            m_te->setSelection( m_te->positionFromLineIndex( m_te->getCursorPosition(), 0 ), m_te->SendScintilla( QsciScintilla::SCI_GETCURRENTPOS ) );
        else if( aEvent.modifiers() == Qt::AltModifier )
#endif
        {
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int wordStart = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, m_te->positionFromLineIndex( line, index ), true );
            m_te->setSelection( wordStart, m_te->positionFromLineIndex( line, index ) );
        }

        m_te->removeSelectedText();
        {
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int wordStart = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, m_te->positionFromLineIndex( line, index ), true );
            m_te->setSelection( wordStart, m_te->positionFromLineIndex( line, index ) );
        }

        m_te->removeSelectedText();
    }
    else if( aEvent.key() == Qt::Key_Delete )
    else if( aEvent.key() == Qt::Key_Delete )
    {
        if( !m_te->hasSelectedText() )
        if( !m_te->hasSelectedText() )
        {
#ifndef __APPLE__
            if( aEvent.modifiers() == Qt::ControlModifier )
            {
                int line, index;
                m_te->getCursorPosition( &line, &index );
                int wordEnd = m_te->SendScintilla( QsciScintilla::SCI_WORDENDPOSITION, m_te->positionFromLineIndex( line, index ), true );
                m_te->setSelection( m_te->positionFromLineIndex( line, index ), wordEnd );
            }
#ifndef __APPLE__
            if( aEvent.modifiers() == Qt::ControlModifier )
            {
                int line, index;
                m_te->getCursorPosition( &line, &index );
                int wordEnd = m_te->SendScintilla( QsciScintilla::SCI_WORDENDPOSITION, m_te->positionFromLineIndex( line, index ), true );
                m_te->setSelection( m_te->positionFromLineIndex( line, index ), wordEnd );
            }
            else
#endif
            {
                int line, index;
                m_te->getCursorPosition( &line, &index );
                int pos = m_te->positionFromLineIndex( line, index );
                m_te->setSelection( pos, pos + 1 );
            }
        }

        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();
            {
                int line, index;
                m_te->getCursorPosition( &line, &index );
                int pos = m_te->positionFromLineIndex( line, index );
                m_te->setSelection( pos, pos + 1 );
            }
        }

        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();
    }
    else if( isCtrlSlash( &aEvent ) )
    else if( isCtrlSlash( &aEvent ) )
    {
        int startLine, startIndex, endLine, endIndex;
        m_te->getSelection( &startLine, &startIndex, &endLine, &endIndex );
        int startLine, startIndex, endLine, endIndex;
        m_te->getSelection( &startLine, &startIndex, &endLine, &endIndex );
        bool comment = firstNonWhitespace( startLine ) != '#';
        int  whitespaceCount;

        m_te->beginUndoAction();
        m_te->beginUndoAction();

        for( int ii = startLine; ii <= endLine; ++ii )
        {
            if( comment )
            {
                int lineStart = m_te->positionFromLineIndex( ii, 0 );
                m_te->SendScintilla( QsciScintilla::SCI_INSERTTEXT, lineStart, "#" );
            }
            {
                int lineStart = m_te->positionFromLineIndex( ii, 0 );
                m_te->SendScintilla( QsciScintilla::SCI_INSERTTEXT, lineStart, "#" );
            }
            else if( firstNonWhitespace( ii, &whitespaceCount ) == '#' )
            {
                int lineStart = m_te->positionFromLineIndex( ii, 0 );
                m_te->SendScintilla( QsciScintilla::SCI_DELETERANGE, lineStart + whitespaceCount, 1 );
            }
            {
                int lineStart = m_te->positionFromLineIndex( ii, 0 );
                m_te->SendScintilla( QsciScintilla::SCI_DELETERANGE, lineStart + whitespaceCount, 1 );
            }
        }

        int startPos = m_te->positionFromLineIndex( startLine, 0 );
        int endPos = m_te->positionFromLineIndex( endLine, m_te->text( endLine ).length() );
        m_te->setSelection( startPos, endPos );
        int startPos = m_te->positionFromLineIndex( startLine, 0 );
        int endPos = m_te->positionFromLineIndex( endLine, m_te->text( endLine ).length() );
        m_te->setSelection( startPos, endPos );

        m_te->endUndoAction();
        m_te->endUndoAction();
    }
#ifdef __APPLE__
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_A )
#ifdef __APPLE__
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_A )
    {
        int line, index;
        m_te->getCursorPosition( &line, &index );
        m_te->setCursorPosition( line, 0 );
        int line, index;
        m_te->getCursorPosition( &line, &index );
        m_te->setCursorPosition( line, 0 );
    }
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_E )
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_E )
    {
        int line, index;
        m_te->getCursorPosition( &line, &index );
        m_te->setCursorPosition( line, m_te->text( line ).length() );
        int line, index;
        m_te->getCursorPosition( &line, &index );
        m_te->setCursorPosition( line, m_te->text( line ).length() );
    }
    else if( ( aEvent.modifiers() & Qt::MetaModifier ) && aEvent.key() == Qt::Key_B )
    else if( ( aEvent.modifiers() & Qt::MetaModifier ) && aEvent.key() == Qt::Key_B )
    {
        if( aEvent.modifiers() & Qt::AltModifier )
            m_te->SendScintilla( QsciScintilla::SCI_WORDLEFT );
        if( aEvent.modifiers() & Qt::AltModifier )
            m_te->SendScintilla( QsciScintilla::SCI_WORDLEFT );
        else
            m_te->SendScintilla( QsciScintilla::SCI_CHARLEFT );
            m_te->SendScintilla( QsciScintilla::SCI_CHARLEFT );
    }
    else if( ( aEvent.modifiers() & Qt::MetaModifier ) && aEvent.key() == Qt::Key_F )
    else if( ( aEvent.modifiers() & Qt::MetaModifier ) && aEvent.key() == Qt::Key_F )
    {
        if( aEvent.modifiers() & Qt::AltModifier )
            m_te->SendScintilla( QsciScintilla::SCI_WORDRIGHT );
        if( aEvent.modifiers() & Qt::AltModifier )
            m_te->SendScintilla( QsciScintilla::SCI_WORDRIGHT );
        else
            m_te->SendScintilla( QsciScintilla::SCI_CHARRIGHT );
            m_te->SendScintilla( QsciScintilla::SCI_CHARRIGHT );
    }
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_D )
    else if( aEvent.modifiers() == Qt::MetaModifier && aEvent.key() == Qt::Key_D )
    {
        if( !m_te->hasSelectedText() )
        {
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int pos = m_te->positionFromLineIndex( line, index );
            m_te->setSelection( pos, pos + 1 );
        }

        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();
        if( !m_te->hasSelectedText() )
        {
            int line, index;
            m_te->getCursorPosition( &line, &index );
            int pos = m_te->positionFromLineIndex( line, index );
            m_te->setSelection( pos, pos + 1 );
        }

        if( m_te->hasSelectedText() )
            m_te->removeSelectedText();
    }
#endif
    else if( aEvent.key() == Qt::Key_F21 )
    else if( aEvent.key() == Qt::Key_F21 )
    {
        // Proxy for a system color changed event
        // Proxy for a system color changed event
        setupStyles();
    }
    else
    {
        // Let QScintilla handle the event
        // Let QScintilla handle the event
    }
}


int SCINTILLA_TRICKS::firstNonWhitespace( int aLine, int* aWhitespaceCharCount )
{
    QString lineText = m_te->text( aLine );
    QString lineText = m_te->text( aLine );

    if( aWhitespaceCharCount )
        *aWhitespaceCharCount = 0;

    for( int ii = 0; ii < lineText.length(); ++ii )
    for( int ii = 0; ii < lineText.length(); ++ii )
    {
        QChar c = lineText.at( ii );
        QChar c = lineText.at( ii );

        if( c == ' ' || c == '\t' )
        {
            if( aWhitespaceCharCount )
                *aWhitespaceCharCount += 1;

            continue;
        }
        else
        {
            return c.unicode();
            return c.unicode();
        }
    }

    return '\r';
}


void SCINTILLA_TRICKS::onScintillaUpdateUI()
void SCINTILLA_TRICKS::onScintillaUpdateUI()
{
    auto isBrace = [this]( QChar c ) -> bool
    auto isBrace = [this]( QChar c ) -> bool
                   {
                       return m_braces.indexOf( c ) >= 0;
                       return m_braces.indexOf( c ) >= 0;
                   };

    // Has the caret changed position?
    int line, index;
    m_te->getCursorPosition( &line, &index );
    int caretPos = m_te->positionFromLineIndex( line, index );
    
    int selStartLine, selStartIndex, selEndLine, selEndIndex;
    m_te->getSelection( &selStartLine, &selStartIndex, &selEndLine, &selEndIndex );
    int selStart = m_te->positionFromLineIndex( selStartLine, selStartIndex );
    int selEnd = m_te->positionFromLineIndex( selEndLine, selEndIndex );
    int line, index;
    m_te->getCursorPosition( &line, &index );
    int caretPos = m_te->positionFromLineIndex( line, index );
    
    int selStartLine, selStartIndex, selEndLine, selEndIndex;
    m_te->getSelection( &selStartLine, &selStartIndex, &selEndLine, &selEndIndex );
    int selStart = m_te->positionFromLineIndex( selStartLine, selStartIndex );
    int selEnd = m_te->positionFromLineIndex( selEndLine, selEndIndex );

    if( m_lastCaretPos != caretPos || m_lastSelStart != selStart || m_lastSelEnd != selEnd )
    {
        m_lastCaretPos = caretPos;
        m_lastSelStart = selStart;
        m_lastSelEnd = selEnd;
        int bracePos1 = -1;
        int bracePos2 = -1;

        // Is there a brace to the left or right?
        QString text = m_te->text();
        if( caretPos > 0 && caretPos - 1 < text.length() && isBrace( text.at( caretPos - 1 ) ) )
            bracePos1 = caretPos - 1;
        else if( caretPos < text.length() && isBrace( text.at( caretPos ) ) )
        QString text = m_te->text();
        if( caretPos > 0 && caretPos - 1 < text.length() && isBrace( text.at( caretPos - 1 ) ) )
            bracePos1 = caretPos - 1;
        else if( caretPos < text.length() && isBrace( text.at( caretPos ) ) )
            bracePos1 = caretPos;

        if( bracePos1 >= 0 )
        {
            // Find the matching brace
            bracePos2 = m_te->SendScintilla( QsciScintilla::SCI_BRACEMATCH, bracePos1, 0 );
            bracePos2 = m_te->SendScintilla( QsciScintilla::SCI_BRACEMATCH, bracePos1, 0 );

            if( bracePos2 == -1 )
            {
                m_te->SendScintilla( QsciScintilla::SCI_BRACEBADLIGHT, bracePos1 );
                m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, 0 );
                m_te->SendScintilla( QsciScintilla::SCI_BRACEBADLIGHT, bracePos1 );
                m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, 0 );
            }
            else
            {
                m_te->SendScintilla( QsciScintilla::SCI_BRACEHIGHLIGHT, bracePos1, bracePos2 );
                int column = m_te->SendScintilla( QsciScintilla::SCI_GETCOLUMN, bracePos1 );
                m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, column );
                m_te->SendScintilla( QsciScintilla::SCI_BRACEHIGHLIGHT, bracePos1, bracePos2 );
                int column = m_te->SendScintilla( QsciScintilla::SCI_GETCOLUMN, bracePos1 );
                m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, column );
            }
        }
        else
        {
            // Turn off brace matching
            m_te->SendScintilla( QsciScintilla::SCI_BRACEHIGHLIGHT, -1, -1 );
            m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, 0 );
            m_te->SendScintilla( QsciScintilla::SCI_BRACEHIGHLIGHT, -1, -1 );
            m_te->SendScintilla( QsciScintilla::SCI_SETHIGHLIGHTGUIDE, 0 );
        }
    }
}


void SCINTILLA_TRICKS::DoTextVarAutocomplete(
        const std::function<void( const QString& xRef, QStringList* tokens )>& getTokensFn )
        const std::function<void( const QString& xRef, QStringList* tokens )>& getTokensFn )
{
    QStringList autocompleteTokens;
    int line, index;
    m_te->getCursorPosition( &line, &index );
    int text_pos = m_te->positionFromLineIndex( line, index );
    int start = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, text_pos, true );
    QString partial;
    QStringList autocompleteTokens;
    int line, index;
    m_te->getCursorPosition( &line, &index );
    int text_pos = m_te->positionFromLineIndex( line, index );
    int start = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, text_pos, true );
    QString partial;

    QString fullText = m_te->text();
    QString fullText = m_te->text();
    auto textVarRef =
            [&]( int pos ) -> bool
            [&]( int pos ) -> bool
            {
                return pos >= 2 && pos - 2 < fullText.length() && pos - 1 < fullText.length()
                       && fullText.at( pos - 2 ) == '$'
                       && fullText.at( pos - 1 ) == '{';
                return pos >= 2 && pos - 2 < fullText.length() && pos - 1 < fullText.length()
                       && fullText.at( pos - 2 ) == '$'
                       && fullText.at( pos - 1 ) == '{';
            };

    // Check for cross-reference
    if( start > 1 && start - 1 < fullText.length() && fullText.at( start - 1 ) == ':' )
    if( start > 1 && start - 1 < fullText.length() && fullText.at( start - 1 ) == ':' )
    {
        int refStart = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, start - 1, true );
        int refStart = m_te->SendScintilla( QsciScintilla::SCI_WORDSTARTPOSITION, start - 1, true );

        if( textVarRef( refStart ) )
        {
            partial = fullText.mid( start, text_pos - start );
            getTokensFn( fullText.mid( refStart, start - 1 - refStart ), &autocompleteTokens );
            partial = fullText.mid( start, text_pos - start );
            getTokensFn( fullText.mid( refStart, start - 1 - refStart ), &autocompleteTokens );
        }
    }
    else if( textVarRef( start ) )
    {
        partial = fullText.mid( start, text_pos - start );
        getTokensFn( QString(), &autocompleteTokens );
        partial = fullText.mid( start, text_pos - start );
        getTokensFn( QString(), &autocompleteTokens );
    }

    DoAutocomplete( partial, autocompleteTokens );
    m_te->setFocus();
    m_te->setFocus();
}


void SCINTILLA_TRICKS::DoAutocomplete( const QString& aPartial, const QStringList& aTokens )
void SCINTILLA_TRICKS::DoAutocomplete( const QString& aPartial, const QStringList& aTokens )
{
    if( m_suppressAutocomplete )
        return;

    QStringList matchedTokens;
    QStringList matchedTokens;

    for( const QString& token : aTokens )
    for( const QString& token : aTokens )
    {
        // Using simple contains check for string matching
        if( token.toLower().contains( aPartial.toLower() ) )
        // Using simple contains check for string matching
        if( token.toLower().contains( aPartial.toLower() ) )
            matchedTokens.push_back( token );
    }

    if( matchedTokens.size() > 0 )
    {
        // NB: tokens MUST be in alphabetical order because the Scintilla engine is going
        // to do a binary search on them
        std::sort( matchedTokens.begin(), matchedTokens.end(),
                   []( const QString& first, const QString& second ) -> bool
                   {
                       return first.compare( second, Qt::CaseInsensitive ) < 0;
                   });
        std::sort( matchedTokens.begin(), matchedTokens.end(),
                   []( const QString& first, const QString& second ) -> bool
                   {
                       return first.compare( second, Qt::CaseInsensitive ) < 0;
                   });

        m_te->SendScintilla( QsciScintilla::SCI_AUTOCSETSEPARATOR, '\t' );
        m_te->SendScintilla( QsciScintilla::SCI_AUTOCSHOW, aPartial.size(), matchedTokens.join( '\t' ).toLatin1().data() );
        m_te->SendScintilla( QsciScintilla::SCI_AUTOCSETSEPARATOR, '\t' );
        m_te->SendScintilla( QsciScintilla::SCI_AUTOCSHOW, aPartial.size(), matchedTokens.join( '\t' ).toLatin1().data() );
    }
}


void SCINTILLA_TRICKS::CancelAutocomplete()
{
    m_te->SendScintilla( QsciScintilla::SCI_AUTOCCANCEL );
    m_te->SendScintilla( QsciScintilla::SCI_AUTOCCANCEL );
}


bool SCINTILLA_TRICKS::eventFilter( QObject* obj, QEvent* event )
{
    if( obj == m_te && event->type() == QEvent::KeyPress )
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
        onCharHook( *keyEvent );
        return false; // Let QScintilla also handle the event
    }
    return QObject::eventFilter( obj, event );
}
bool SCINTILLA_TRICKS::eventFilter( QObject* obj, QEvent* event )
{
    if( obj == m_te && event->type() == QEvent::KeyPress )
    {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>( event );
        onCharHook( *keyEvent );
        return false; // Let QScintilla also handle the event
    }
    return QObject::eventFilter( obj, event );
}