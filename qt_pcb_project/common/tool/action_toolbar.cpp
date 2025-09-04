
#include <algorithm>
#include <advanced_config.h>
#include <bitmaps.h>
#include <bitmap_store.h>
#include <eda_draw_frame.h>
#include <functional>
#include <kiplatform/ui.h>
#include <math/util.h>
#include <memory>
#include <pgm_base.h>
#include <settings/common_settings.h>
#include <tool/action_toolbar.h>
#include <tool/actions.h>
#include <tool/tool_action.h>
#include <tool/tool_event.h>
#include <tool/tool_interactive.h>
#include <tool/tool_manager.h>
#include <widgets/bitmap_button.h>
#include <widgets/wx_aui_art_providers.h>
#include <QtWidgets/QWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QBoxLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtGui/QPainter>
#include <QtCore/QTimer>
#include <QtWidgets/QApplication>
#include <QtCore/QHash>
#include <QtCore/QVector>


ACTION_GROUP::ACTION_GROUP( const std::string& aName,
                            const std::vector<const TOOL_ACTION*>& aActions )
{
    Q_ASSERT_X( aActions.size() > 0, "ACTION_GROUP", "Action groups must have at least one action" );

    // The default action is just the first action in the vector
    m_actions       = aActions;
    m_defaultAction = m_actions[0];

    m_name = aName;
    m_id   = ACTION_MANAGER::MakeActionId( m_name );
}


int ACTION_GROUP::GetUIId() const
{
    return m_id + TOOL_ACTION::GetBaseUIId();
}


void ACTION_GROUP::SetDefaultAction( const TOOL_ACTION& aDefault )
{
    bool valid = std::any_of( m_actions.begin(), m_actions.end(),
                              [&]( const TOOL_ACTION* aAction ) -> bool
                              {
                                  // For some reason, we can't compare the actions directly
                                  return aAction->GetId() == aDefault.GetId();
                              } );

    Q_ASSERT_X( valid, "ACTION_GROUP", "Action must be present in a group to be the default" );

    m_defaultAction = &aDefault;
}


#define PALETTE_BORDER 4    // The border around the palette buttons on all sides
#define BUTTON_BORDER  1    // The border on the sides of the buttons that touch other buttons


ACTION_TOOLBAR_PALETTE::ACTION_TOOLBAR_PALETTE( QWidget* aParent, bool aVertical ) :
        QWidget( aParent, Qt::Popup | Qt::FramelessWindowHint ),
        m_group( nullptr ),
        m_isVertical( aVertical ),
        m_panel( nullptr ),
        m_mainSizer( nullptr ),
        m_buttonSizer( nullptr )
{
    m_panel = new QWidget( this );
    m_panel->setAutoFillBackground( true );

    // This layout holds the buttons for the actions
    m_buttonSizer = aVertical ? static_cast<QBoxLayout*>(new QVBoxLayout()) : static_cast<QBoxLayout*>(new QHBoxLayout());

    // This layout holds the other layout, so that a consistent border is present on all sides
    m_mainSizer = aVertical ? static_cast<QBoxLayout*>(new QVBoxLayout()) : static_cast<QBoxLayout*>(new QHBoxLayout());
    m_mainSizer->setContentsMargins( PALETTE_BORDER, PALETTE_BORDER, PALETTE_BORDER, PALETTE_BORDER );
    m_mainSizer->addLayout( m_buttonSizer );

    m_panel->setLayout( m_mainSizer );

    // Qt key event handling will be done through keyPressEvent override
}


void ACTION_TOOLBAR_PALETTE::AddAction( const TOOL_ACTION& aAction )
{
    int            size = Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size;
    QPixmap normalBmp = KiBitmapBundle( aAction.GetIcon(), size ).pixmap();

    int bmpWidth = normalBmp.width();
    int padding = ( m_buttonSize.width() - bmpWidth ) / 2;
    QSize bmSize( size, size );
    bmSize *= KIPLATFORM::UI::GetContentScaleFactor( m_parent );

    BITMAP_BUTTON* button = new BITMAP_BUTTON( m_panel, aAction.GetUIId() );

    button->SetIsToolbarButton();
    button->SetBitmap( normalBmp );
    button->SetDisabledBitmap( KiDisabledBitmapBundle( aAction.GetIcon() ) );
    button->SetPadding( padding );
    button->SetToolTip( aAction.GetButtonTooltip() );
    button->AcceptDragInAsClick();
    button->SetBitmapCentered();

    m_buttons[aAction.GetUIId()] = button;

    if( m_isVertical )
    {
        m_buttonSizer->addWidget( button );
        m_buttonSizer->setContentsMargins( 0, BUTTON_BORDER, 0, BUTTON_BORDER );
    }
    else
    {
        m_buttonSizer->addWidget( button );
        m_buttonSizer->setContentsMargins( BUTTON_BORDER, 0, BUTTON_BORDER, 0 );
    }

    m_buttonSizer->update();
}


void ACTION_TOOLBAR_PALETTE::EnableAction( const TOOL_ACTION& aAction, bool aEnable )
{
    auto it = m_buttons.find( aAction.GetUIId() );

    if( it != m_buttons.end() )
        it->second->Enable( aEnable );
}


void ACTION_TOOLBAR_PALETTE::CheckAction( const TOOL_ACTION& aAction, bool aCheck )
{
    auto it = m_buttons.find( aAction.GetUIId() );

    if( it != m_buttons.end() )
        it->second->Check( aCheck );
}


void ACTION_TOOLBAR_PALETTE::Popup( QWidget* aFocus )
{
    m_panel->adjustSize();
    resize( m_panel->size() );

    show();
    if( aFocus )
        aFocus->setFocus();
}


void ACTION_TOOLBAR_PALETTE::keyPressEvent( QKeyEvent* aEvent )
{
    // Allow the escape key to dismiss this popup
    if( aEvent->key() == Qt::Key_Escape )
        hide();
    else
        QWidget::keyPressEvent( aEvent );
}


ACTION_TOOLBAR::ACTION_TOOLBAR( EDA_BASE_FRAME* parent, int id, const QPoint& pos,
                                const QSize& size, Qt::ToolBarAreas areas ) :
    QToolBar( parent ),
    m_paletteTimer( nullptr ),
    m_auiManager( nullptr ),
    m_toolManager( parent->GetToolManager() ),
    m_palette( nullptr )
{
    m_paletteTimer = new QTimer( this );
    m_paletteTimer->setSingleShot( true );

    // Qt toolbar styling will be handled through stylesheets

    connect( this, &QToolBar::actionTriggered, this, &ACTION_TOOLBAR::onToolEvent );
    connect( m_paletteTimer, &QTimer::timeout, this, &ACTION_TOOLBAR::onTimerDone );
}


ACTION_TOOLBAR::~ACTION_TOOLBAR()
{
    // Qt connections are automatically disconnected in destructor

    // Clear all the maps keeping track of our items on the toolbar
    m_toolMenus.clear();
    m_actionGroups.clear();
    m_toolCancellable.clear();
    m_toolKinds.clear();
    m_toolActions.clear();
}


QAction* ACTION_TOOLBAR::findActionById( int id )
{
    const QList<QAction*> actionList = actions();
    for( QAction* action : actionList )
    {
        if( action->data().toInt() == id )
            return action;
    }
    return nullptr;
}


void ACTION_TOOLBAR::Add( const TOOL_ACTION& aAction, bool aIsToggleEntry, bool aIsCancellable )
{
    Q_ASSERT( parent() );
    Q_ASSERT_X( !( aIsCancellable && !aIsToggleEntry ),
                "ACTION_TOOLBAR", "aIsCancellable requires aIsToggleEntry" );

    int toolId = aAction.GetUIId();

    QAction* qtAction = addAction( KiBitmapBundle( aAction.GetIcon(),
                                                   Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size ).pixmap(),
                                   QString() );
    qtAction->setToolTip( aAction.GetButtonTooltip() );
    qtAction->setCheckable( aIsToggleEntry );
    qtAction->setData( toolId );

    m_toolKinds[ toolId ]       = aIsToggleEntry;
    m_toolActions[ toolId ]     = &aAction;
    m_toolCancellable[ toolId ] = aIsCancellable;
}


void ACTION_TOOLBAR::AddButton( const TOOL_ACTION& aAction )
{
    int toolId = aAction.GetUIId();

    QAction* qtAction = addAction( KiBitmapBundle( aAction.GetIcon(),
                                                   Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size ).pixmap(),
                                   QString() );
    qtAction->setToolTip( aAction.GetButtonTooltip() );
    qtAction->setData( toolId );

    m_toolKinds[ toolId ] = false;
    m_toolActions[ toolId ] = &aAction;
}


void ACTION_TOOLBAR::AddScaledSeparator( QWidget* aWindow )
{
    int scale = KiIconScale( aWindow );

    if( scale > 4 )
    {
        QWidget* spacer = new QWidget();
        spacer->setFixedWidth( 16 * ( scale - 4 ) / 4 );
        addWidget( spacer );
    }

    addSeparator();

    if( scale > 4 )
    {
        QWidget* spacer = new QWidget();
        spacer->setFixedWidth( 16 * ( scale - 4 ) / 4 );
        addWidget( spacer );
    }
}


void ACTION_TOOLBAR::AddToolContextMenu( const TOOL_ACTION& aAction,
                                         std::unique_ptr<ACTION_MENU> aMenu )
{
    int toolId = aAction.GetUIId();

    m_toolMenus[toolId] = std::move( aMenu );
}


void ACTION_TOOLBAR::AddGroup( ACTION_GROUP* aGroup, bool aIsToggleEntry )
{
    int                groupId       = aGroup->GetUIId();
    const TOOL_ACTION* defaultAction = aGroup->GetDefaultAction();

    Q_ASSERT( parent() );
    Q_ASSERT( defaultAction );

    m_toolKinds[ groupId ]    = aIsToggleEntry;
    m_toolActions[ groupId ]  = defaultAction;
    m_actionGroups[ groupId ] = aGroup;

    // Add the main toolbar item representing the group
    QAction* qtAction = addAction( KiBitmapBundle( defaultAction->GetIcon(),
                                                   Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size ).pixmap(),
                                   QString() );
    qtAction->setCheckable( aIsToggleEntry );
    qtAction->setData( groupId );

    // Select the default action
    doSelectAction( aGroup, *defaultAction );
}


void ACTION_TOOLBAR::SelectAction( ACTION_GROUP* aGroup, const TOOL_ACTION& aAction )
{
    bool valid = std::any_of( aGroup->m_actions.begin(), aGroup->m_actions.end(),
                              [&]( const TOOL_ACTION* action2 ) -> bool
                              {
                                  // For some reason, we can't compare the actions directly
                                  return aAction.GetId() == action2->GetId();
                              } );

    if( valid )
        doSelectAction( aGroup, aAction );
}


void ACTION_TOOLBAR::doSelectAction( ACTION_GROUP* aGroup, const TOOL_ACTION& aAction )
{
    Q_ASSERT( parent() );

    int groupId = aGroup->GetUIId();

    QAction* item = findActionById( groupId );

    if( !item )
        return;

    // Update the item information
    if( item )
    {
        item->setToolTip( aAction.GetButtonTooltip() );
        item->setIcon( KiBitmapBundle( aAction.GetIcon(),
                                       Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size ).pixmap() );
    }

    // Register a new handler with the new UI conditions
    if( m_toolManager )
    {
        const ACTION_CONDITIONS* cond = m_toolManager->GetActionManager()->GetCondition( aAction );

        Q_ASSERT_X( cond, "ACTION_TOOLBAR", QString( "Missing UI condition for action %1" )
                                                      .arg( aAction.GetName() ).toLocal8Bit().data() );

        m_toolManager->GetToolHolder()->UnregisterUIUpdateHandler( groupId );
        m_toolManager->GetToolHolder()->RegisterUIUpdateHandler( groupId, *cond );
    }

    // Update the currently selected action
    m_toolActions[ groupId ] = &aAction;

    update();
}


void ACTION_TOOLBAR::UpdateControlWidth( int aID )
{
    QAction* item = findActionById( aID );
    Q_ASSERT_X( item, "ACTION_TOOLBAR", QString( "No toolbar item found for ID %1" )
                                                   .arg( aID ).toLocal8Bit().data() );

    // The control on the toolbar is associated with the action
    QWidget* control = widgetForAction( item );
    Q_ASSERT_X( control, "ACTION_TOOLBAR",
                QString( "No control located in toolbar item with ID %1" )
                    .arg( aID ).toLocal8Bit().data() );

    // Update the size the item has stored using the best size of the control
    control->updateGeometry();
    QSize bestSize = control->sizeHint();
    control->setMinimumSize( bestSize );

    // Update the layout
    if( layout() )
        layout()->update();
    
    updateGeometry();
}


void ACTION_TOOLBAR::ClearToolbar()
{
    // Clear all the maps keeping track of our items on the toolbar
    m_toolMenus.clear();
    m_actionGroups.clear();
    m_toolCancellable.clear();
    m_toolKinds.clear();
    m_toolActions.clear();

    // Remove the actual tools from the toolbar
    clear();
}


const TOOL_ACTION* ACTION_TOOLBAR::GetSelectedAction( const std::string& aGroupName )
{
    for( const auto& [id, group] : m_actionGroups )
    {
        if( group->GetName() == aGroupName )
            return m_toolActions[group->GetUIId()];
    }

    return nullptr;
}


void ACTION_TOOLBAR::SetToolBitmap( const TOOL_ACTION& aAction, const QPixmap& aBitmap )
{
    int toolId = aAction.GetUIId();
    QAction* action = findActionById( toolId );
    
    if( action )
    {
        action->setIcon( QIcon( aBitmap ) );
        // Qt will automatically generate disabled icons
    }
}


void ACTION_TOOLBAR::Toggle( const TOOL_ACTION& aAction, bool aState )
{
    int toolId = aAction.GetUIId();
    QAction* action = findActionById( toolId );
    if( action )
    {
        if( m_toolKinds[ toolId ] )
            action->setChecked( aState );
        else
            action->setEnabled( aState );
    }
}


void ACTION_TOOLBAR::Toggle( const TOOL_ACTION& aAction, bool aEnabled, bool aChecked )
{
    int toolId = aAction.GetUIId();
    QAction* action = findActionById( toolId );
    if( action )
    {
        action->setEnabled( aEnabled );
        action->setChecked( aEnabled && aChecked );
    }
}


void ACTION_TOOLBAR::onToolEvent( QAction* aAction )
{
    int id = aAction->data().toInt();
    OPT_TOOL_EVENT evt;

    bool handled = false;

    if( m_toolManager && id >= TOOL_ACTION::GetBaseUIId() )
    {
        const auto actionIt = m_toolActions.find( id );

        // The toolbar item is toggled before the event is sent, so we check for it not being
        // toggled to see if it was toggled originally
        if( m_toolCancellable[id] && !aAction->isChecked() )
        {
            // Send a cancel event
            m_toolManager->CancelTool();
            handled = true;
        }
        else if( actionIt != m_toolActions.end() )
        {
            // Dispatch a tool event
            evt = actionIt->second->MakeEvent();
            evt->SetHasPosition( false );
            m_toolManager->ProcessEvent( *evt );
            m_toolManager->GetToolHolder()->RefreshCanvas();
            handled = true;
        }
    }
}


void ACTION_TOOLBAR::contextMenuEvent( QContextMenuEvent* aEvent )
{
    QAction* action = actionAt( aEvent->pos() );
    if( !action )
        return;
    
    int toolId = action->data().toInt();

    // This means the event was not on a button
    if( toolId == 0 )
        return;

    // Ensure that the ID used maps to a proper tool ID.
    // If right-clicked on a group item, this is needed to get the ID of the currently selected
    // action, since the event's ID is that of the group.
    const auto actionIt = m_toolActions.find( toolId );

    if( actionIt != m_toolActions.end() )
        toolId = actionIt->second->GetUIId();

    // Find the menu for the action
    const auto menuIt = m_toolMenus.find( toolId );

    if( menuIt == m_toolMenus.end() )
        return;

    // Update and show the menu
    std::unique_ptr<ACTION_MENU>& owningMenu = menuIt->second;

    // Get the actual menu pointer to show it
    ACTION_MENU* menu = owningMenu.get();
    SELECTION    dummySel;

    if( CONDITIONAL_MENU* condMenu = dynamic_cast<CONDITIONAL_MENU*>( menu ) )
        condMenu->Evaluate( dummySel );

    menu->UpdateAll();
    menu->popup( mapToGlobal( aEvent->pos() ) );
}


// The time (in milliseconds) between pressing the left mouse button and opening the palette
#define PALETTE_OPEN_DELAY 500


void ACTION_TOOLBAR::mousePressEvent( QMouseEvent* aEvent )
{
    QAction* item = actionAt( aEvent->pos() );

    if( item )
    {
        // Ensure there is no active palette
        if( m_palette )
        {
            m_palette->hide();
            m_palette->deleteLater();
            m_palette = nullptr;
        }

        // Start the popup conditions if it is a left mouse click and the tool clicked is a group
        if( aEvent->button() == Qt::LeftButton && ( m_actionGroups.find( item->data().toInt() ) != m_actionGroups.end() ) )
            m_paletteTimer->start( PALETTE_OPEN_DELAY );
    }

    QToolBar::mousePressEvent( aEvent );
}

void ACTION_TOOLBAR::mouseReleaseEvent( QMouseEvent* aEvent )
{
    QAction* item = actionAt( aEvent->pos() );
    
    if( item )
    {
        // Clear the popup conditions if it is a left up, because that implies a click happened
        if( aEvent->button() == Qt::LeftButton )
            m_paletteTimer->stop();
    }
    
    QToolBar::mouseReleaseEvent( aEvent );
}


void ACTION_TOOLBAR::mouseMoveEvent( QMouseEvent* aEvent )
{
    // Qt drag handling will be implemented as needed
    QToolBar::mouseMoveEvent( aEvent );
}


void ACTION_TOOLBAR::onTimerDone()
{
    // We need to search for the tool using the client coordinates
    QPoint mousePos = mapFromGlobal( KIPLATFORM::UI::GetMousePosition() );

    QAction* item = actionAt( mousePos );

    if( item )
        popupPalette( item );
}


void ACTION_TOOLBAR::onPaletteEvent( QAction* aAction )
{
    if( !m_palette )
        return;

    OPT_TOOL_EVENT evt;
    ACTION_GROUP*  group = m_palette->GetGroup();

    // Find the action corresponding to the button press
    auto actionIt = std::find_if( group->GetActions().begin(), group->GetActions().end(),
                                  [&]( const TOOL_ACTION* action )
                                  {
                                      return action->GetUIId() == aAction->data().toInt();
                                  } );

    if( actionIt != group->GetActions().end() )
    {
        const TOOL_ACTION* action = *actionIt;

        // Dispatch a tool event
        evt = action->MakeEvent();
        evt->SetHasPosition( false );
        m_toolManager->ProcessEvent( *evt );
        m_toolManager->GetToolHolder()->RefreshCanvas();

        // Update the main toolbar item with the selected action
        doSelectAction( group, *action );
    }

    // Hide the palette
    m_palette->hide();
    m_palette->deleteLater();
    m_palette = nullptr;
}


void ACTION_TOOLBAR::popupPalette( QAction* aItem )
{
    // Clear all popup conditions
    m_paletteTimer->stop();

    QWidget* toolParent = dynamic_cast<QWidget*>( m_toolManager->GetToolHolder() );

    Q_ASSERT( parent() );
    Q_ASSERT( m_auiManager );
    Q_ASSERT( toolParent );

    // Ensure the item we are using for the palette has a group associated with it.
    const auto it = m_actionGroups.find( aItem->data().toInt() );

    if( it == m_actionGroups.end() )
        return;

    ACTION_GROUP* group = it->second;

    // We use the size of the toolbar items for our palette buttons
    QRect toolRect = actionGeometry( aItem );

    // The position for the palette window must be in screen coordinates
    QPoint pos( mapToGlobal( toolRect.topLeft() ) );

    // True for vertical buttons, false for horizontal
    bool    dir        = true;
    size_t  numActions = group->m_actions.size();

    // The size of the palette in the long dimension
    int paletteLongDim =   ( 2 * PALETTE_BORDER )      // The border on all sides of the buttons
                         + ( BUTTON_BORDER )           // The border on the start of the buttons
                         + ( numActions * BUTTON_BORDER )          // The other button borders
                         + ( numActions * toolRect.height() );  // The size of the buttons

    // Determine the position of the top left corner of the palette window
    // For Qt, we'll use a simpler positioning strategy based on toolbar area
    Qt::ToolBarArea area = Qt::TopToolBarArea;
    if( QMainWindow* mainWindow = qobject_cast<QMainWindow*>( parentWidget() ) )
        area = mainWindow->toolBarArea( this );
    switch( area )
    {
        case Qt::TopToolBarArea:
            // Top toolbars need to shift the palette window down
            dir = true;                                 // Buttons are vertical in the palette
            pos = mapToGlobal( toolRect.bottomLeft() );
            pos += QPoint( -PALETTE_BORDER, 0 );       // Shift left to align the button edges
            break;

        case Qt::BottomToolBarArea:
            // Bottom toolbars need to shift the palette window up
            dir = true;                                 // Buttons are vertical in the palette
            pos = mapToGlobal( toolRect.topLeft() );
            pos += QPoint( -PALETTE_BORDER, -paletteLongDim ); // Shift up by the palette height
            break;

        case Qt::LeftToolBarArea:
            // Left toolbars need to shift the palette window right
            dir = false;                               // Buttons are horizontal in the palette
            pos = mapToGlobal( toolRect.topRight() );
            pos += QPoint( 0, -PALETTE_BORDER );       // Shift up to align the button tops
            break;

        case Qt::RightToolBarArea:
            // Right toolbars need to shift the palette window left
            dir = false;                                // Buttons are horizontal in the palette
            pos = mapToGlobal( toolRect.topLeft() );
            pos += QPoint( -paletteLongDim, -PALETTE_BORDER ); // Shift left by palette width
            break;
            
        default:
            dir = true;
            pos = mapToGlobal( toolRect.bottomLeft() );
            break;
    }

    m_palette = new ACTION_TOOLBAR_PALETTE( static_cast<QWidget*>( parent() ), dir );

    // We handle the button events in the toolbar class, so connect the right handler
    m_palette->SetGroup( group );
    m_palette->SetButtonSize( toolRect );
    connect( m_palette, &ACTION_TOOLBAR_PALETTE::actionTriggered,
             this, &ACTION_TOOLBAR::onPaletteEvent );


    // Add the actions in the group to the palette and update their enabled state
    // We purposely don't check items in the palette
    for( const TOOL_ACTION* action : group->m_actions )
    {
        m_palette->AddAction( *action );
        
        // Enable/disable action based on current state
        QAction* qtAction = findActionById( action->GetUIId() );
        if( qtAction )
            m_palette->EnableAction( *action, qtAction->isEnabled() );
    }

    // Release the mouse to ensure the first click will be recognized in the palette
    releaseMouse();

    m_palette->move( pos );
    m_palette->Popup( nullptr );

    // Clear any active states
    clearFocus();
}


void ACTION_TOOLBAR::paintEvent( QPaintEvent* aEvent )
{
    QToolBar::paintEvent( aEvent );
    
    QPainter painter( this );
    
    // Draw custom indicators for action groups
    for( const auto& [id, group] : m_actionGroups )
    {
        QAction* action = findActionById( id );
        if( !action )
            continue;
            
        QRect rect = actionGeometry( action );
        
        // Choose the color to draw the triangle
        QColor clr = action->isEnabled() ? palette().buttonText().color() : palette().mid().color();
        
        painter.setPen( QPen( clr ) );
        painter.setBrush( QBrush( clr ) );
        
        // Make the side length of the triangle approximately 1/5th of the bitmap
        int sideLength = qRound( rect.height() / 5.0 );
        
        // This will create a triangle with its point at the bottom right corner
        QPoint btmRight = rect.bottomRight();
        QPoint topCorner( btmRight.x(), btmRight.y() - sideLength );
        QPoint btmCorner( btmRight.x() - sideLength, btmRight.y() );
        
        QPolygon triangle;
        triangle << btmRight << topCorner << btmCorner;
        
        painter.drawPolygon( triangle );
    }
}


bool ACTION_TOOLBAR::KiRealize()
{
    // Qt toolbars automatically layout their contents
    updateGeometry();
    update();
    return true;
}


void ACTION_TOOLBAR::onThemeChanged()
{
    GetBitmapStore()->ThemeChanged();
    RefreshBitmaps();
}


void ACTION_TOOLBAR::RefreshBitmaps()
{
    for( const std::pair<int, const TOOL_ACTION*> pair : m_toolActions )
    {
        QAction* action = findActionById( pair.first );
        
        if( action )
        {
            action->setIcon( KiBitmapBundle( pair.second->GetIcon(),
                                           Pgm().GetCommonSettings()->m_Appearance.toolbar_icon_size ).pixmap() );
        }
    }

    update();
}
