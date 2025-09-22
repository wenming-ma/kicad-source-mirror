
#include <bitmaps.h>
#include <eda_base_frame.h>
#include <functional>
#include <id.h>
#include <kiface_base.h>
#include <tool/action_menu.h>
#include <tool/actions.h>
#include <tool/tool_event.h>
#include <tool/tool_interactive.h>
#include <tool/tool_manager.h>
#include <trace_helpers.h>
#include <QMenu>
#include <QAction>
#include <QApplication>
#include <QWidget>
#include <QTextEdit>
#include <QListView>
#include <QTableView>
#include <QKeySequence>
#include <QtGlobal>
#include <QDialog>
#include <widgets/kiui_common.h>

using namespace std::placeholders;

// We need to store the position of the mouse when the menu was opened so it can be passed
// to the command event generated when the menu item is selected.
static VECTOR2D g_menu_open_position;

// Qt doesn't tell us when a menu command was generated from a hotkey or from
// a menu selection.  It's important to us because a hotkey can be an immediate action
// while the menu selection can not (as it has no associated position).
//
// We get around this by storing the last highlighted menuId.  If it matches the command
// id then we know this is a menu selection.  (You might think we could use the menuOpen
// menuClose events, but these are actually generated for hotkeys as well.)
static int g_last_menu_highlighted_id = 0;

// Helper functions for Qt implementation
QAction* ACTION_MENU::findActionById( int aId ) const
{
    for( QAction* action : actions() )
    {
        if( action->data().toInt() == aId )
            return action;
    }
    return nullptr;
}


ACTION_MENU* ACTION_MENU::findMenuContaining( QAction* aAction ) const
{
    for( ACTION_MENU* submenu : m_submenus )
    {
        if( submenu->actions().contains( aAction ) )
            return submenu;
        
        ACTION_MENU* found = submenu->findMenuContaining( aAction );
        if( found )
            return found;
    }
    return nullptr;
}


void ACTION_MENU::OnAboutToShow()
{
    if( m_dirty )
    {
        TOOL_MANAGER* toolMgr = getToolManager();
        if( toolMgr )
            toolMgr->RunAction<ACTION_MENU*>( ACTIONS::updateMenu, this );
    }

    QMenu* parent = dynamic_cast<QMenu*>( parentWidget() );

    // Don't update the position if this menu has a parent
    if( !parent )
    {
        TOOL_MANAGER* toolMgr = getToolManager();
        if( toolMgr )
            g_menu_open_position = toolMgr->GetMousePosition();
    }

    g_last_menu_highlighted_id = 0;
}


void ACTION_MENU::OnAboutToHide()
{
    // Menu closing cleanup
}


ACTION_MENU::ACTION_MENU( bool isContextMenu, TOOL_INTERACTIVE* aTool ) :
    m_isForcedPosition( false ),
    m_dirty( true ),
    m_titleDisplayed( false ),
    m_isContextMenu( isContextMenu ),
    m_icon( BITMAPS::INVALID_BITMAP ),
    m_selected( -1 ),
    m_tool( aTool )
{
    setupEvents();
}


ACTION_MENU::~ACTION_MENU()
{
    // Disconnect Qt signals

    // Set parent to NULL to prevent submenus from unregistering from a nonexistent object
    for( ACTION_MENU* menu : m_submenus )
        menu->setParent( nullptr );

    ACTION_MENU* parent = dynamic_cast<ACTION_MENU*>( parentWidget() );

    if( parent )
        parent->m_submenus.remove( this );
}


void ACTION_MENU::SetIcon( BITMAPS aIcon )
{
    m_icon = aIcon;
}


void ACTION_MENU::setupEvents()
{
    // Connect Qt signals
    connect( this, &QMenu::triggered, this, &ACTION_MENU::OnMenuEvent );
    connect( this, &QMenu::aboutToShow, this, &ACTION_MENU::OnAboutToShow );
    connect( this, &QMenu::aboutToHide, this, &ACTION_MENU::OnAboutToHide );
}


void ACTION_MENU::SetTitle( const QString& aTitle )
{
    m_title = aTitle;

    // Update the menu title
    if( m_titleDisplayed )
        DisplayTitle( true );
}


void ACTION_MENU::DisplayTitle( bool aDisplay )
{
    if( ( !aDisplay || m_title.isEmpty() ) && m_titleDisplayed )
    {
        // Remove the title action
        QAction* titleAction = actions().first();
        removeAction( titleAction );
        delete titleAction;
        
        // Remove separator
        QAction* separatorAction = actions().first();
        removeAction( separatorAction );
        delete separatorAction;
        m_titleDisplayed = false;
    }

    else if( aDisplay && !m_title.isEmpty() )
    {
        if( m_titleDisplayed )
        {
            // Simply update the title
            actions().first()->setText( m_title );
        }
        else
        {
            // Add a separator and a menu action to display the title
            addSeparator();
            QAction* titleAction = new QAction( m_title, this );
            titleAction->setEnabled( false );
            
            if( !!m_icon )
                titleAction->setIcon( KiBitmapBundle( m_icon, 24 ) );
                
            insertAction( actions().isEmpty() ? nullptr : actions().first(), titleAction );
            m_titleDisplayed = true;
        }
    }
}


QAction* ACTION_MENU::Add( const QString& aLabel, int aId, BITMAPS aIcon )
{
    QAction* action = new QAction( aLabel, this );
    action->setData( aId );

    if( !!aIcon )
        action->setIcon( KiBitmapBundle( aIcon, 24 ) );

    addAction( action );
    return action;
}


QAction* ACTION_MENU::Add( const QString& aLabel, const QString& aTooltip, int aId,
                              BITMAPS aIcon, bool aIsCheckmarkEntry )
{
    QAction* action = new QAction( aLabel, this );
    action->setData( aId );
    action->setToolTip( aTooltip );
    action->setCheckable( aIsCheckmarkEntry );

    if( !!aIcon )
        action->setIcon( KiBitmapBundle( aIcon, 24 ) );

    addAction( action );
    return action;
}


QAction* ACTION_MENU::Add( const TOOL_ACTION& aAction, bool aIsCheckmarkEntry,
                              const QString& aOverrideLabel )
{
    // ID numbers for tool actions are assigned above ACTION_BASE_UI_ID inside TOOL_EVENT
    BITMAPS icon = aAction.GetIcon();

    // Allow the label to be overridden at point of use
    QString menuLabel = aOverrideLabel.isEmpty() ? aAction.GetMenuItem() : aOverrideLabel;

    QAction* action = new QAction( menuLabel, this );
    action->setData( aAction.GetUIId() );
    action->setToolTip( aAction.GetTooltip() );
    action->setCheckable( aIsCheckmarkEntry );
    
    if( !!icon )
        action->setIcon( KiBitmapBundle( icon, 24 ) );

    m_toolActions[aAction.GetUIId()] = &aAction;

    addAction( action );
    return action;
}


QAction* ACTION_MENU::Add( ACTION_MENU* aMenu )
{
    m_submenus.push_back( aMenu );

    QAction* submenuAction = addMenu( aMenu );
    submenuAction->setText( aMenu->m_title );
    
    if( !!aMenu->m_icon )
        submenuAction->setIcon( KiBitmapBundle( aMenu->m_icon, 24 ) );

    return submenuAction;
}


void ACTION_MENU::AddClose( const QString& aAppname )
{
#ifdef __WINDOWS__
    Add( _("Close"),
         QString( _("Close %1") ).arg( aAppname ),
         QDialog::Rejected,
         BITMAPS::exit );
#else
    Add( _("Close") + QString("\tCtrl+W"),
         QString( _("Close %1") ).arg( aAppname ),
         QDialog::Rejected,
         BITMAPS::exit );
#endif
}


void ACTION_MENU::AddQuitOrClose( KIFACE_BASE* aKiface, QString aAppname )
{
    if( !aKiface || aKiface->IsSingle() ) // not when under a project mgr
    {
        // Don't use ACTIONS::quit; Qt moves this on OSX and expects to find it via
        // standard IDs
        Add( _("Quit") + QString("\tCtrl+Q"),
             QString( _("Quit %1") ).arg( aAppname ),
             QDialog::Rejected,
             BITMAPS::exit );
    }
    else
    {
        AddClose( aAppname );
    }
}


void ACTION_MENU::AddQuit( const QString& aAppname )
{
    // Don't use ACTIONS::quit; Qt moves this on OSX and expects to find it via
    // standard IDs
    Add( _("Quit") + QString("\tCtrl+Q"),
         QString( _("Quit %1") ).arg( aAppname ),
         QDialog::Rejected,
         BITMAPS::exit );
}


void ACTION_MENU::Clear()
{
    m_titleDisplayed = false;

    clear();

    m_toolActions.clear();
    m_submenus.clear();
}


bool ACTION_MENU::HasEnabledItems() const
{
    for( QAction* action : actions() )
    {
        if( action->isEnabled() && !action->isSeparator() )
            return true;
    }

    return false;
}


void ACTION_MENU::UpdateAll()
{
    try
    {
        update();
    }
    catch( std::exception& )
    {
    }

    if( m_tool )
        updateHotKeys();

    runOnSubmenus( std::bind( &ACTION_MENU::UpdateAll, _1 ) );
}


void ACTION_MENU::ClearDirty()
{
    m_dirty = false;
    runOnSubmenus( std::bind( &ACTION_MENU::ClearDirty, _1 ) );
}


void ACTION_MENU::SetDirty()
{
    m_dirty = true;
    runOnSubmenus( std::bind( &ACTION_MENU::SetDirty, _1 ) );
}


void ACTION_MENU::SetTool( TOOL_INTERACTIVE* aTool )
{
    m_tool = aTool;
    runOnSubmenus( std::bind( &ACTION_MENU::SetTool, _1, aTool ) );
}


ACTION_MENU* ACTION_MENU::Clone() const
{
    ACTION_MENU* clone = create();
    clone->Clear();
    clone->copyFrom( *this );
    return clone;
}


ACTION_MENU* ACTION_MENU::create() const
{
    ACTION_MENU* menu = new ACTION_MENU( false );

    // Type checking for Qt implementation

    return menu;
}


TOOL_MANAGER* ACTION_MENU::getToolManager() const
{
    return m_tool ? m_tool->GetManager() : nullptr;
}


void ACTION_MENU::updateHotKeys()
{
    TOOL_MANAGER* toolMgr = getToolManager();

    Q_ASSERT( toolMgr );

    for( auto it = m_toolActions.begin(); it != m_toolActions.end(); ++it )
    {
        int                id = it->first;
        const TOOL_ACTION& action = *it->second;
        int                key = toolMgr->GetHotKey( action ) & ~MD_MODIFIER_MASK;

        if( key > 0 )
        {
            int mod = toolMgr->GetHotKey( action ) & MD_MODIFIER_MASK;
            int flags = 0;
            QAction* action = findActionById( id );

            if( action )
            {
                Qt::KeyboardModifiers qtMod = Qt::NoModifier;
                qtMod |= ( mod & MD_ALT ) ? Qt::AltModifier : Qt::NoModifier;
                qtMod |= ( mod & MD_CTRL ) ? Qt::ControlModifier : Qt::NoModifier;
                qtMod |= ( mod & MD_SHIFT ) ? Qt::ShiftModifier : Qt::NoModifier;

                QKeySequence shortcut( qtMod | key );
                action->setShortcut( shortcut );
            }
        }
    }
}


void ACTION_MENU::OnIdle()
{
    g_last_menu_highlighted_id = 0;
    g_menu_open_position.x = 0.0;
    g_menu_open_position.y = 0.0;
}


void ACTION_MENU::OnMenuEvent( QAction* aAction )
{
    OPT_TOOL_EVENT evt;
    QString       menuText;
    QWidget*      focus   = QApplication::focusWidget();
    TOOL_MANAGER*  toolMgr = getToolManager();

    // Handle menu opening in OnAboutToShow
    // Menu highlight handling for Qt
    // Handle menu selection
    if( aAction )
    {
        // Handle text editor shortcuts
        if( dynamic_cast<QTextEdit*>( focus )
                || dynamic_cast<QListView*>( focus )
                || dynamic_cast<QTableView*>( focus ) )
        {
            // Handle key events for Qt text widgets
            QKeySequence shortcut = aAction->shortcut();
            if( !shortcut.isEmpty() )
            {
                QKeyEvent keyEvent( QEvent::KeyPress, shortcut[0].key(), 
                                   Qt::KeyboardModifiers( shortcut[0].keyboardModifiers() ) );
                
                if( QTextEdit* textEdit = dynamic_cast<QTextEdit*>( focus ) )
                {
                    QApplication::sendEvent( textEdit, &keyEvent );
                    if( keyEvent.isAccepted() )
                        return;
                }
                else
                {
                    QApplication::sendEvent( focus, &keyEvent );
                    if( keyEvent.isAccepted() )
                        return;
                }
            }
        }

        // Store the selected position, so it can be checked by the tools
        m_selected = aAction->data().toInt();

        ACTION_MENU* parent = dynamic_cast<ACTION_MENU*>( parentWidget() );

        while( parent )
        {
            parent->m_selected = m_selected;
            parent = dynamic_cast<ACTION_MENU*>( parent->parentWidget() );
        }

        // Check if there is a TOOL_ACTION for the given UI ID
        if( toolMgr && toolMgr->GetActionManager()->IsActionUIId( m_selected ) )
            evt = findToolAction( m_selected );

        if( !evt )
        {
#ifdef __WINDOWS__
            if( !evt )
            {
                // Try to find the submenu which holds the selected item
                ACTION_MENU* menu = findMenuContaining( aAction );
                if( menu )
                {
                    evt = menu->eventHandler( aAction );
                }
            }
#else
            if( !evt )
                runEventHandlers( aAction, evt );
#endif

            // Handling non-ACTION menu entries.  Two ranges of ids are supported:
            //   between 0 and ID_CONTEXT_MENU_ID_MAX
            //   between ID_POPUP_MENU_START and ID_POPUP_MENU_END

            #define ID_CONTEXT_MENU_ID_MAX 100  /* should be plenty */

            if( !evt &&
                    ( ( m_selected >= 0 && m_selected < ID_CONTEXT_MENU_ID_MAX ) ||
                      ( m_selected >= ID_POPUP_MENU_START && m_selected <= ID_POPUP_MENU_END ) ) )
            {
                ACTION_MENU* actionMenu = dynamic_cast<ACTION_MENU*>( parentWidget() );

                if( actionMenu && actionMenu->PassHelpTextToHandler() )
                    menuText = aAction->toolTip();
                else
                    menuText = aAction->text();

                evt = TOOL_EVENT( TC_COMMAND, TA_CHOICE_MENU_CHOICE, m_selected, AS_GLOBAL );
                evt->SetParameter( &menuText );
            }
        }
    }

    // forward the action/update event to the TOOL_MANAGER
    // clients that don't supply a tool will have to check GetSelected() themselves
    if( evt && toolMgr )
    {
        // Log trace for Qt implementation

        // WARNING: if you're squeamish, look away.
        // What follows is a series of egregious hacks necessitated by a lack of information
        // on where context-menu-commands and command-key-events originated.

        // If it's a context menu then fetch the mouse position from our context-menu-position
        // hack.
        if( m_isContextMenu )
        {
            evt->SetMousePosition( g_menu_open_position );
        }
        // Check if it is a menubar event
        else if( g_last_menu_highlighted_id == aAction->data().toInt() )
        {
            evt->SetHasPosition( false );
        }
        // Otherwise it's a command-key-event and we need to get the mouse position from the tool
        // manager so that immediate actions work.
        else
        {
            evt->SetMousePosition( toolMgr->GetMousePosition() );
        }

        toolMgr->ProcessEvent( *evt );
    }
    // Qt event handling complete
}


void ACTION_MENU::runEventHandlers( QAction* aAction, OPT_TOOL_EVENT& aToolEvent )
{
    aToolEvent = eventHandler( aAction );

    if( !aToolEvent )
        runOnSubmenus( std::bind( &ACTION_MENU::runEventHandlers, _1, aAction, aToolEvent ) );
}


void ACTION_MENU::runOnSubmenus( std::function<void(ACTION_MENU*)> aFunction )
{
    try
    {
        std::for_each( m_submenus.begin(), m_submenus.end(),
                       [&]( ACTION_MENU* m )
                       {
                           aFunction( m );
                           m->runOnSubmenus( aFunction );
                       } );
    }
    catch( std::exception& )
    {
    }
}


OPT_TOOL_EVENT ACTION_MENU::findToolAction( int aId )
{
    OPT_TOOL_EVENT evt;

    auto findFunc =
            [&]( ACTION_MENU* m )
            {
                if( evt )
                    return;

                const auto it = m->m_toolActions.find( aId );

                if( it != m->m_toolActions.end() )
                    evt = it->second->MakeEvent();
            };

    findFunc( this );

    if( !evt )
        runOnSubmenus( findFunc );

    return evt;
}


void ACTION_MENU::copyFrom( const ACTION_MENU& aMenu )
{
    m_icon = aMenu.m_icon;
    m_title = aMenu.m_title;
    m_titleDisplayed = aMenu.m_titleDisplayed;
    m_selected = -1; // aMenu.m_selected;
    m_tool = aMenu.m_tool;
    m_toolActions = aMenu.m_toolActions;

    // Copy all menu entries
    for( QAction* action : aMenu.actions() )
    {
        appendCopy( action );
    }
}


QAction* ACTION_MENU::appendCopy( const QAction* aSource )
{
    QAction* newAction = new QAction( aSource->text(), this );
    newAction->setData( aSource->data() );
    newAction->setToolTip( aSource->toolTip() );
    newAction->setCheckable( aSource->isCheckable() );
    newAction->setIcon( aSource->icon() );
    newAction->setShortcut( aSource->shortcut() );

    if( aSource->menu() )
    {
        ACTION_MENU* sourceMenu = dynamic_cast<ACTION_MENU*>( aSource->menu() );
        if( sourceMenu )
        {
            ACTION_MENU* menuCopy = sourceMenu->Clone();
            newAction->setMenu( menuCopy );
            m_submenus.push_back( menuCopy );
        }
    }

    addAction( newAction );

    if( aSource->isCheckable() )
        newAction->setChecked( aSource->isChecked() );

    newAction->setEnabled( aSource->isEnabled() );

    return newAction;
}
