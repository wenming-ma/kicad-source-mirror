#include <grid_tricks.h>
#include <QtCore/QString>
#include <QtGui/QClipboard>
#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtCore/QTextStream>
#include <QtWidgets/QMenu>
#include <QtGui/QCursor>
#include <QtCore/QMimeData>
#include <widgets/grid_text_helpers.h>
#include <vector>
#include <string>

// Utility functions for string splitting to replace QStringList functionality
namespace {
    std::vector<std::string> splitString(const QString& str, const QChar& separator, Qt::SplitBehavior behavior = Qt::KeepEmptyParts) {
        QStringList qlist = str.split(separator, behavior);
        std::vector<std::string> result;
        result.reserve(qlist.size());
        for (const QString& item : qlist) {
            result.push_back(item.toStdString());
        }
        return result;
    }
    
    QString joinStrings(const std::vector<std::string>& strings, const QChar& separator) {
        QStringList qlist;
        qlist.reserve(strings.size());
        for (const std::string& str : strings) {
            qlist.push_back(QString::fromStdString(str));
        }
        return qlist.join(separator);
    }
}

#define COL_SEP     QLatin1Char( '\t' )
#define ROW_SEP     QLatin1Char( '\n' )
#define ROW_SEP_R   QLatin1Char( '\r' )


GRID_TRICKS::GRID_TRICKS( QT_GRID* aGrid ) :
    QObject( aGrid ),
    m_grid( aGrid ),
    m_addHandler( []( QAction* ) {} ),
    m_enableSingleClickEdit( true ),
    m_multiCellEditEnabled( true )
{
    init();
}


GRID_TRICKS::GRID_TRICKS( QT_GRID* aGrid, std::function<void( QAction* )> aAddHandler ) :
    QObject( aGrid ),
    m_grid( aGrid ),
    m_addHandler( aAddHandler ),
    m_enableSingleClickEdit( true ),
    m_multiCellEditEnabled( true )
{
    init();
}


void GRID_TRICKS::init()
{
    m_sel_row_start = 0;
    m_sel_col_start = 0;
    m_sel_row_count = 0;
    m_sel_col_count = 0;

    connect( m_grid, &QT_GRID::cellClicked, this, &GRID_TRICKS::onGridCellLeftClick );
    connect( m_grid, &QT_GRID::cellDoubleClicked, this, &GRID_TRICKS::onGridCellLeftDClick );
    connect( m_grid, &QT_GRID::cellRightClicked, this, &GRID_TRICKS::onGridCellRightClick );
    connect( m_grid, &QT_GRID::labelLeftClicked, this, &GRID_TRICKS::onGridLabelLeftClick );
    connect( m_grid, &QT_GRID::labelRightClicked, this, &GRID_TRICKS::onGridLabelRightClick );
    connect( m_grid, &QT_GRID::keyPressed, this, &GRID_TRICKS::onKeyDown );
    connect( m_grid, &QT_GRID::charHook, this, &GRID_TRICKS::onCharHook );
    connect( m_grid, &QT_GRID::updateUI, this, &GRID_TRICKS::onUpdateUI );
    connect( m_grid, &QT_GRID::mouseMotion, this, &GRID_TRICKS::onGridMotion );

    m_grid->setMouseTracking( true );
}


bool GRID_TRICKS::isTextEntry( int aRow, int aCol )
{
    return m_grid->IsTextEntry( aRow, aCol );
}


bool GRID_TRICKS::isCheckbox( int aRow, int aCol )
{
    return m_grid->IsCheckbox( aRow, aCol );
}


bool GRID_TRICKS::isReadOnly( int aRow, int aCol )
{
    return !m_grid->IsEditable() || m_grid->IsReadOnly( aRow, aCol );
}


bool GRID_TRICKS::toggleCell( int aRow, int aCol, bool aPreserveSelection )
{
    if( isCheckbox( aRow, aCol ) )
    {
        if( !aPreserveSelection )
        {
            m_grid->clearSelection();
            m_grid->SetGridCursor( aRow, aCol );
        }

        if( m_grid->CanGetValueAs( aRow, aCol, "bool" ) && m_grid->CanSetValueAs( aRow, aCol, "bool" ) )
        {
            m_grid->SetValueAsBool( aRow, aCol, !m_grid->GetValueAsBool( aRow, aCol ) );
        }
        else
        {
            if( m_grid->GetCellValue( aRow, aCol ) == "1" )
                m_grid->SetCellValue( aRow, aCol, "0" );
            else
                m_grid->SetCellValue( aRow, aCol, "1" );
        }

        m_grid->ForceRefresh();

        QString newValue = m_grid->GetCellValue( aRow, aCol );
        m_grid->EmitCellChanged( aRow, aCol, newValue );

        return true;
    }

    return false;
}


bool GRID_TRICKS::showEditor( int aRow, int aCol )
{
    if( m_grid->GetGridCursorRow() != aRow || m_grid->GetGridCursorCol() != aCol )
        m_grid->SetGridCursor( aRow, aCol );

    if( !isReadOnly( aRow, aCol ) )
    {
        m_grid->clearSelection();

        m_sel_row_start = aRow;
        m_sel_col_start = aCol;
        m_sel_row_count = 1;
        m_sel_col_count = 1;

        if( m_grid->GetSelectionMode() == QT_GRID::SelectRows )
        {
            std::vector<int> rows = m_grid->GetSelectedRows();
            if( rows.size() != 1 || rows[0] != aRow )
                m_grid->SelectRow( aRow );
        }

        m_grid->ShowEditorOnMouseUp();

        return true;
    }

    return false;
}


void GRID_TRICKS::onGridCellLeftClick( int row, int col )
{
    if( !m_grid->HasModifiers() )
    {
        bool toggled = false;

        if( toggleCell( row, col, true ) )
            toggled = true;
        else if( m_enableSingleClickEdit && showEditor( row, col ) )
            return;

        if( toggled )
        {
            getSelectedArea();

            if( !m_grid->GetSelectedCells().empty() || m_sel_row_count < 2 )
            {
                m_grid->clearSelection();
                return;
            }

            QString newVal = m_grid->GetCellValue( row, col );

            for( int otherRow = m_sel_row_start; otherRow < m_sel_row_start + m_sel_row_count; ++otherRow )
            {
                if( otherRow == row )
                    continue;

                m_grid->SetCellValue( otherRow, col, newVal );
            }

            return;
        }
    }
}


void GRID_TRICKS::onGridCellLeftDClick( int row, int col )
{
    if( !handleDoubleClick( row, col ) )
        onGridCellLeftClick( row, col );
}


void GRID_TRICKS::onGridMotion( QMouseEvent* aEvent )
{
    QPoint pt = aEvent->pos();
    QPoint pos = m_grid->CalcScrolledPosition( pt );

    int col = m_grid->XToCol( pos.x() );
    int row = m_grid->YToRow( pos.y() );

    if( ( col == -1 ) || ( row == -1 ) || !m_tooltipEnabled[col] )
    {
        m_grid->setToolTip( QString() );
        return;
    }

    m_grid->setToolTip( m_grid->GetCellValue( row, col ) );
}


bool GRID_TRICKS::handleDoubleClick( int row, int col )
{
    Q_UNUSED( row )
    Q_UNUSED( col )
    return false;
}


void GRID_TRICKS::getSelectedArea()
{
    std::vector<std::vector<int>> blocks = m_grid->GetSelectionBlockTopLeft();
    std::vector<std::vector<int>> botRight = m_grid->GetSelectionBlockBottomRight();
    std::vector<int> cols = m_grid->GetSelectedCols();
    std::vector<int> rows = m_grid->GetSelectedRows();

    if( !blocks.empty() && !botRight.empty() )
    {
        m_sel_row_start = blocks[0][0];
        m_sel_col_start = blocks[0][1];
        m_sel_row_count = botRight[0][0] - m_sel_row_start + 1;
        m_sel_col_count = botRight[0][1] - m_sel_col_start + 1;
    }
    else if( !cols.empty() )
    {
        m_sel_col_start = cols[0];
        m_sel_col_count = cols.size();
        m_sel_row_start = 0;
        m_sel_row_count = m_grid->GetNumberRows();
    }
    else if( !rows.empty() )
    {
        m_sel_col_start = 0;
        m_sel_col_count = m_grid->GetNumberCols();
        m_sel_row_start = rows[0];
        m_sel_row_count = rows.size();
    }
    else
    {
        m_sel_row_start = m_grid->GetGridCursorRow();
        m_sel_col_start = m_grid->GetGridCursorCol();
        m_sel_row_count = m_sel_row_start >= 0 ? 1 : 0;
        m_sel_col_count = m_sel_col_start >= 0 ? 1 : 0;
    }
}


void GRID_TRICKS::onGridCellRightClick( int row, int col )
{
    QMenu menu;
    showPopupMenu( menu, row, col );
}


void GRID_TRICKS::onGridLabelLeftClick( int section )
{
    Q_UNUSED( section )
    m_grid->CommitPendingChanges();
}


void GRID_TRICKS::onGridLabelRightClick( int section )
{
    Q_UNUSED( section )
    QMenu menu;

    for( int i = 0; i < m_grid->GetNumberCols(); ++i )
    {
        int id = GRIDTRICKS_FIRST_SHOWHIDE + i;
        QAction* action = menu.addAction( m_grid->GetColLabelValue( i ) );
        action->setCheckable( true );
        action->setChecked( m_grid->IsColShown( i ) );
        action->setData( id );
        connect( action, &QAction::triggered, this, [this, action]() {
            onPopupSelection( action );
        });
    }

    menu.exec( QCursor::pos() );
}


void GRID_TRICKS::showPopupMenu( QMenu& menu, int row, int col )
{
    Q_UNUSED( row )
    Q_UNUSED( col )
    
    QAction* cutAction = menu.addAction( QObject::tr("Cut") + "\tCtrl+X" );
    cutAction->setData( GRIDTRICKS_ID_CUT );
    QAction* copyAction = menu.addAction( QObject::tr("Copy") + "\tCtrl+C" );
    copyAction->setData( GRIDTRICKS_ID_COPY );
    
    QAction* pasteAction = nullptr;
    QAction* deleteAction = nullptr;
    
    if( m_multiCellEditEnabled )
    {
        pasteAction = menu.addAction( QObject::tr("Paste") + "\tCtrl+V" );
        pasteAction->setData( GRIDTRICKS_ID_PASTE );
        deleteAction = menu.addAction( QObject::tr("Delete") + "\tDel" );
        deleteAction->setData( GRIDTRICKS_ID_DELETE );
    }

    QAction* selectAllAction = menu.addAction( QObject::tr("Select All") + "\tCtrl+A" );
    selectAllAction->setData( GRIDTRICKS_ID_SELECT );

    cutAction->setEnabled( false );
    if( deleteAction ) deleteAction->setEnabled( false );
    if( pasteAction ) pasteAction->setEnabled( false );

    getSelectedArea();

    auto anyCellsWritable = [&]()
    {
        for( int row = m_sel_row_start; row < m_sel_row_start + m_sel_row_count; ++row )
        {
            for( int col = m_sel_col_start; col < m_sel_col_start + m_sel_col_count; ++col )
            {
                if( !isReadOnly( row, col ) && isTextEntry( row, col ) )
                    return true;
            }
        }
        return false;
    };

    if( anyCellsWritable() )
    {
        cutAction->setEnabled( true );
        if( deleteAction ) deleteAction->setEnabled( true );
    }

    QClipboard* clipboard = QApplication::clipboard();
    if( clipboard->mimeData()->hasText() && m_grid->IsEditable() && pasteAction )
    {
        pasteAction->setEnabled( true );
    }

    connect( cutAction, &QAction::triggered, this, [this, cutAction]() { onPopupSelection( cutAction ); });
    connect( copyAction, &QAction::triggered, this, [this, copyAction]() { onPopupSelection( copyAction ); });
    if( pasteAction ) connect( pasteAction, &QAction::triggered, this, [this, pasteAction]() { onPopupSelection( pasteAction ); });
    if( deleteAction ) connect( deleteAction, &QAction::triggered, this, [this, deleteAction]() { onPopupSelection( deleteAction ); });
    connect( selectAllAction, &QAction::triggered, this, [this, selectAllAction]() { onPopupSelection( selectAllAction ); });

    menu.exec( QCursor::pos() );
}


void GRID_TRICKS::onPopupSelection( QAction* action )
{
    doPopupSelection( action );
}


void GRID_TRICKS::doPopupSelection( QAction* action )
{
    int menu_id = action->data().toInt();

    switch( menu_id )
    {
    case GRIDTRICKS_ID_CUT:
        cutcopy( true, true );
        break;

    case GRIDTRICKS_ID_COPY:
        cutcopy( true, false );
        break;

    case GRIDTRICKS_ID_DELETE:
        cutcopy( false, true );
        break;

    case GRIDTRICKS_ID_PASTE:
        paste_clipboard();
        break;

    case GRIDTRICKS_ID_SELECT:
        m_grid->SelectAll();
        break;

    default:
        if( menu_id >= GRIDTRICKS_FIRST_SHOWHIDE && m_grid->CommitPendingChanges( false ) )
        {
            int col = menu_id - GRIDTRICKS_FIRST_SHOWHIDE;

            if( m_grid->IsColShown( col ) )
                m_grid->HideCol( col );
            else
                m_grid->ShowCol( col );
        }
    }
}


void GRID_TRICKS::onCharHook( QKeyEvent* ev )
{
    bool handled = false;

    if( ( ev->key() == Qt::Key_Return || ev->key() == Qt::Key_Enter )
        && ev->modifiers() == Qt::NoModifier
        && m_grid->GetGridCursorRow() == m_grid->GetNumberRows() - 1 )
    {
        if( m_grid->IsCellEditControlShown() )
        {
            if( m_grid->CommitPendingChanges() )
                handled = true;
        }
        else
        {
            QAction* dummy = nullptr;
            m_addHandler( dummy );
            handled = true;
        }
    }
    else if( ev->modifiers() == Qt::ControlModifier && ev->key() == Qt::Key_V )
    {
        if( m_grid->IsCellEditControlShown() )
        {
            QClipboard* clipboard = QApplication::clipboard();
            if( clipboard->mimeData()->hasText() )
            {
                QString text = clipboard->text();
                if( text.contains( COL_SEP ) || text.contains( ROW_SEP ) )
                {
                    QString stripped( text );
                    stripped.replace( ROW_SEP, " " );
                    stripped.replace( ROW_SEP_R, " " );
                    stripped.replace( COL_SEP, " " );
                    
                    QWidget* editor = m_grid->GetCurrentEditor();
                    if( editor && m_grid->IsEditorEditable( editor ) )
                        m_grid->WriteTextToEditor( editor, stripped );
                    else
                        paste_text( stripped );
                    
                    handled = true;
                }
            }
            m_grid->ForceRefresh();
        }
    }
    else if( ev->key() == Qt::Key_Escape )
    {
        if( m_grid->IsCellEditControlShown() )
        {
            m_grid->CancelPendingChanges();
            handled = true;
        }
    }

    if( !handled )
        ev->accept();
}


void GRID_TRICKS::onKeyDown( QKeyEvent* ev )
{
    if( ev->modifiers() == Qt::ControlModifier && ev->key() == Qt::Key_A )
    {
        m_grid->SelectAll();
        return;
    }
    else if( ev->modifiers() == Qt::ControlModifier && ev->key() == Qt::Key_C )
    {
        getSelectedArea();
        cutcopy( true, false );
        return;
    }
    else if( ev->modifiers() == Qt::ControlModifier && ev->key() == Qt::Key_V )
    {
        getSelectedArea();
        paste_clipboard();
        return;
    }
    else if( ev->modifiers() == Qt::ControlModifier && ev->key() == Qt::Key_X )
    {
        getSelectedArea();
        cutcopy( true, true );
        return;
    }
    else if( ev->modifiers() == Qt::NoModifier && ev->key() == Qt::Key_Delete )
    {
        getSelectedArea();
        cutcopy( false, true );
        return;
    }

    if( m_grid->IsEditable() && ev->key() == Qt::Key_Space )
    {
        bool retVal = false;

        if( m_grid->GetSelectionMode() == QT_GRID::SelectRows )
        {
            std::vector<int> rowSel = m_grid->GetSelectedRows();
            for( int rowInd = 0; rowInd < rowSel.size(); rowInd++ )
                retVal |= toggleCell( rowSel[rowInd], 0, true );
        }
        else if( m_grid->GetSelectionMode() == QT_GRID::SelectColumns )
        {
            std::vector<int> colSel = m_grid->GetSelectedCols();
            for( int colInd = 0; colInd < colSel.size(); colInd++ )
                retVal |= toggleCell( 0, colSel[colInd], true );
        }
        else if( m_grid->GetSelectionMode() == QT_GRID::SelectCells )
        {
            std::vector<int> rowSel = m_grid->GetSelectedRows();
            std::vector<int> colSel = m_grid->GetSelectedCols();
            std::vector<std::vector<int>> cellSel = m_grid->GetSelectedCells();
            std::vector<std::vector<int>> topLeft = m_grid->GetSelectionBlockTopLeft();
            std::vector<std::vector<int>> botRight = m_grid->GetSelectionBlockBottomRight();

            for( int cellInd = 0; cellInd < cellSel.size(); cellInd++ )
            {
                retVal |= toggleCell( cellSel[cellInd][0], cellSel[cellInd][1], true );
            }

            for( int colInd = 0; colInd < colSel.size(); colInd++ )
            {
                for( int row = 0; row < m_grid->GetNumberRows(); row++ )
                    retVal |= toggleCell( row, colSel[colInd], true );
            }

            for( int rowInd = 0; rowInd < rowSel.size(); rowInd++ )
            {
                for( int col = 0; col < m_grid->GetNumberCols(); col++ )
                    retVal |= toggleCell( rowSel[rowInd], col, true );
            }

            for( int blockInd = 0; blockInd < topLeft.size(); blockInd++ )
            {
                std::vector<int> start = topLeft[blockInd];
                std::vector<int> end = botRight[blockInd];

                for( int row = start[0]; row <= end[0]; row++ )
                {
                    for( int col = start[1]; col <= end[1]; col++ )
                        retVal |= toggleCell( row, col, true );
                }
            }
        }

        if( retVal )
            return;
    }

#ifdef Q_OS_MAC
    bool ctrl = ev->modifiers() & Qt::MetaModifier;
#else
    bool ctrl = ev->modifiers() & Qt::ControlModifier;
#endif

    if( ctrl && ev->key() == Qt::Key_Tab )
    {
        QWidget* nextWidget = m_grid->nextInFocusChain();
        while( nextWidget && !nextWidget->isWindow() )
        {
            nextWidget->setFocus();
            if( nextWidget->hasFocus() )
                break;
            nextWidget = nextWidget->nextInFocusChain();
        }
        return;
    }

    ev->accept();
}


void GRID_TRICKS::paste_clipboard()
{
    QClipboard* clipboard = QApplication::clipboard();
    if( m_grid->IsEditable() && clipboard->mimeData()->hasText() )
    {
        QString text = clipboard->text();
        
#ifdef Q_OS_MAC
        text.replace( "\n\n", "\n" );
#endif
        
        paste_text( text );
    }
    m_grid->ForceRefresh();
}


void GRID_TRICKS::paste_text( const QString& cb_text )
{
    if( !m_multiCellEditEnabled )
        return;

    const int cur_row = m_grid->GetGridCursorRow();
    const int cur_col = m_grid->GetGridCursorCol();
    int start_row;
    int end_row;
    int start_col;
    int end_col;
    bool is_selection = false;

    if( cur_row < 0 || cur_col < 0 )
    {
        QApplication::beep();
        return;
    }

    if( m_grid->GetSelectionMode() == QT_GRID::SelectRows )
    {
        if( m_sel_row_count > 1 )
            is_selection = true;
    }
    else if( m_sel_col_count > 1 || m_sel_row_count > 1 )
    {
        is_selection = true;
    }

    QStringList rows = cb_text.split( ROW_SEP, Qt::KeepEmptyParts );

    if( is_selection )
    {
        start_row = m_sel_row_start;
        end_row = m_sel_row_start + m_sel_row_count;
        start_col = m_sel_col_start;
        end_col = m_sel_col_start + m_sel_col_count;
    }
    else
    {
        start_row = cur_row;
        end_row = cur_row + rows.size();

        if( end_row > m_grid->GetNumberRows() )
        {
            if( m_addHandler )
            {
                for( int ii = end_row - m_grid->GetNumberRows(); ii > 0; --ii )
                {
                    QAction* dummy = nullptr;
                    m_addHandler( dummy );
                }
            }
            end_row = m_grid->GetNumberRows();
        }

        start_col = cur_col;
        end_col = start_col;
    }

    int rowIndex = 0;
    for( int row = start_row; row < end_row; ++row )
    {
        if( rowIndex >= rows.size() )
            rowIndex = 0;

        QString rowTxt = rows[rowIndex];
        rowIndex++;

        QStringList cols = rowTxt.split( COL_SEP, Qt::KeepEmptyParts );

        if( !is_selection )
            end_col = cur_col + cols.size();

        int colIndex = 0;
        for( int col = start_col; col < end_col && col < m_grid->GetNumberCols(); ++col )
        {
            if( !m_grid->IsColShown( col ) )
            {
                end_col++;
                continue;
            }

            if( colIndex >= cols.size() )
                colIndex = 0;

            QString cellTxt = cols[colIndex];
            colIndex++;

            if( m_grid->CanSetValueAs( row, col, "string" ) && !isReadOnly( row, col ) )
            {
                m_grid->SetCellValue( row, col, cellTxt );
                m_grid->EmitCellChanged( row, col, cellTxt );
            }
            else if( m_grid->CanSetValueAs( row, col, "bool" ) )
            {
                m_grid->SetValueAsBool( row, col, cellTxt == "1" );
                m_grid->EmitCellChanged( row, col, cellTxt );
            }
        }
    }
}


void GRID_TRICKS::cutcopy( bool doCopy, bool doDelete )
{
    if( doCopy )
    {
        QClipboard* clipboard = QApplication::clipboard();
        if( !clipboard )
            return;
    }

    QString txt;

    for( int row = m_sel_row_start; row < m_sel_row_start + m_sel_row_count; ++row )
    {
        if( !txt.isEmpty() )
            txt += ROW_SEP;

        for( int col = m_sel_col_start; col < m_sel_col_start + m_sel_col_count; ++col )
        {
            if( !m_grid->IsColShown( col ) )
                continue;

            txt += m_grid->GetCellValue( row, col );

            if( col < m_sel_col_start + m_sel_col_count - 1 )
                txt += COL_SEP;

            if( doDelete )
            {
                if( isTextEntry( row, col ) && !isReadOnly( row, col ) )
                    m_grid->SetCellValue( row, col, QString() );
            }
        }
    }

    if( doCopy )
    {
        QClipboard* clipboard = QApplication::clipboard();
        clipboard->setText( txt );
    }

    if( doDelete )
        m_grid->ForceRefresh();
}


void GRID_TRICKS::onUpdateUI()
{
    if( m_grid->GetSelectionMode() == QT_GRID::SelectRows )
    {
        int cursorRow = m_grid->GetGridCursorRow();
        bool cursorInSelectedRow = false;

        std::vector<int> selectedRows = m_grid->GetSelectedRows();
        for( int row : selectedRows )
        {
            if( row == cursorRow )
            {
                cursorInSelectedRow = true;
                break;
            }
        }

        if( !cursorInSelectedRow && cursorRow >= 0 )
            m_grid->SelectRow( cursorRow );
    }
}