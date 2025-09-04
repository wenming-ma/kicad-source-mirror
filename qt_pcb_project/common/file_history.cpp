
#include <file_history.h>
#include <id.h>
#include <settings/app_settings.h>
#include <tool/action_menu.h>
#include <tool/selection_conditions.h>
#include <QMenu>
#include <QString>
#include <QList>

#include <functional>
using namespace std::placeholders;


FILE_HISTORY::FILE_HISTORY( size_t aMaxFiles, int aBaseFileId, int aClearId, QString aClearText )
        : m_maxFiles( std::min( aMaxFiles, (size_t) MAX_FILE_HISTORY_SIZE ) ),
          m_baseFileId( aBaseFileId ),
          m_clearId( aClearId ),
          m_clearText( aClearText )
{
}


void FILE_HISTORY::Load( const APP_SETTINGS_BASE& aSettings )
{
    ClearFileHistory();

    // file_history stores the most recent file first
    for( auto it = aSettings.m_System.file_history.rbegin();
         it != aSettings.m_System.file_history.rend(); ++it )
    {
        AddFileToHistory( *it );
    }
}


void FILE_HISTORY::Load( const std::vector<QString>& aList )
{
    ClearFileHistory();

    for( const auto& file : aList )
        AddFileToHistory( file );
}


void FILE_HISTORY::Save( APP_SETTINGS_BASE& aSettings )
{
    aSettings.m_System.file_history.clear();

    for( const QString& filename : m_fileHistory )
        aSettings.m_System.file_history.emplace_back( filename );
}


void FILE_HISTORY::Save( std::vector<QString>* aList )
{
    aList->clear();

    for( const auto& file : m_fileHistory )
        aList->push_back( file );
}


void FILE_HISTORY::SetMaxFiles( size_t aMaxFiles )
{
    m_fileMaxFiles = std::min( aMaxFiles, (size_t) MAX_FILE_HISTORY_SIZE );

    size_t numFiles = m_fileHistory.size();

    while( numFiles > m_fileMaxFiles )
        RemoveFileFromHistory( --numFiles );
}


void FILE_HISTORY::AddFileToHistory( const QString &aFile )
{
    // Iterate over each menu removing our custom items
    for( QMenu* menu : m_fileMenus )
    {
        doRemoveClearitem( menu );
    }

    // Remove the file from history if it already exists
    m_fileHistory.removeAll( aFile );
    
    // Add the file to the beginning of the list
    m_fileHistory.prepend( aFile );
    
    // Remove excess files if we exceed the maximum
    while( m_fileHistory.size() > (int)m_maxFiles )
        m_fileHistory.removeLast();

    // Add our custom items back
    for( QMenu* menu : m_fileMenus )
    {
        doAddClearItem( menu );
    }
}


void FILE_HISTORY::AddFilesToMenu( QMenu* aMenu )
{
    doRemoveClearitem( aMenu );
    
    // Add file history items
    for( int i = 0; i < m_fileHistory.size(); ++i )
    {
        QString menuText = QString( "&%1 %2" ).arg( i + 1 ).arg( m_fileHistory[i] );
        QAction* action = aMenu->addAction( menuText );
        action->setData( m_baseFileId + i );
    }
    
    doAddClearItem( aMenu );
}


void FILE_HISTORY::doRemoveClearitem( QMenu* aMenu )
{
    QList<QAction*> actions = aMenu->actions();
    
    // Remove clear item and empty placeholder
    for( QAction* action : actions )
    {
        if( action->data().toInt() == m_clearId || 
            action->data().toInt() == ID_FILE_LIST_EMPTY )
        {
            aMenu->removeAction( action );
        }
        // Remove separators before clear item
        else if( action->isSeparator() && 
                 !actions.isEmpty() && 
                 actions.indexOf(action) > 0 )
        {
            int nextIdx = actions.indexOf(action) + 1;
            if( nextIdx < actions.size() )
            {
                QAction* nextAction = actions[nextIdx];
                if( nextAction->data().toInt() == m_clearId )
                {
                    aMenu->removeAction( action );
                }
            }
        }
    }
}


void FILE_HISTORY::doAddClearItem( QMenu* aMenu )
{
    if( GetCount() == 0 )
    {
        // If the history is empty, we create an item to say there are no files
        QAction* action = aMenu->addAction( "No Files" );
        action->setData( ID_FILE_LIST_EMPTY );
        action->setEnabled( false );
    }

    aMenu->addSeparator();
    QAction* clearAction = aMenu->addAction( m_clearText );
    clearAction->setData( m_clearId );
}


void FILE_HISTORY::UpdateClearText( QMenu* aMenu, QString aClearText )
{
    QList<QAction*> actions = aMenu->actions();
    
    for( QAction* action : actions )
    {
        if( action->data().toInt() == m_clearId )
        {
            action->setText( aClearText );
            break;
        }
    }
}


void FILE_HISTORY::ClearFileHistory()
{
    m_fileHistory.clear();
}


size_t FILE_HISTORY::GetCount() const
{
    return m_fileHistory.size();
}


QString FILE_HISTORY::GetHistoryFile( size_t index ) const
{
    if( index < m_fileHistory.size() )
        return m_fileHistory[index];
    return QString();
}


void FILE_HISTORY::RemoveFileFromHistory( size_t index )
{
    if( index < m_fileHistory.size() )
        m_fileHistory.removeAt( index );
}


void FILE_HISTORY::AddFileMenu( QMenu* aMenu )
{
    if( !m_fileMenus.contains( aMenu ) )
        m_fileMenus.append( aMenu );
}


void FILE_HISTORY::RemoveFileMenu( QMenu* aMenu )
{
    m_fileMenus.removeAll( aMenu );
}


SELECTION_CONDITION FILE_HISTORY::FileHistoryNotEmpty( const FILE_HISTORY& aHistory )
{
    return std::bind( &FILE_HISTORY::isHistoryNotEmpty, _1, std::cref( aHistory ) );
}


bool FILE_HISTORY::isHistoryNotEmpty( const SELECTION& aSelection, const FILE_HISTORY& aHistory )
{
    return aHistory.GetCount() != 0;
}
