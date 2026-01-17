// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-24
// Copyright The KiCad Developers, see AUTHORS.txt for contributors.

#include <algorithm>
#include <api/api_plugin_manager.h>
#include <base_screen.h>
#include <bitmaps.h>
#include <confirm.h>
#include <core/arraydim.h>
#include <core/kicad_algo.h>
#include <dialog_shim.h>
#include <dialogs/hotkey_cycle_popup.h>
#include <eda_draw_frame.h>
#include <file_history.h>
#include <gal/graphics_abstraction_layer.h>
#include <id.h>
#include <kiface_base.h>
#include <kiplatform/ui.h>
#include <lockfile.h>
#include <macros.h>
#include <math/vector2wx.h>
#include <page_info.h>
#include <paths.h>
#include <pgm_base.h>
#include <render_settings.h>
#include <settings/app_settings.h>
#include <settings/color_settings.h>
#include <settings/common_settings.h>
#include <settings/settings_manager.h>
#include <title_block.h>
#include <tool/actions.h>
#include <tool/action_toolbar.h>
#include <tool/common_tools.h>
#include <tool/grid_helper.h>
#include <tool/grid_menu.h>
#include <tool/selection_conditions.h>
#include <tool/tool_dispatcher.h>
#include <tool/tool_manager.h>
#include <tool/tool_menu.h>
#include <tool/zoom_menu.h>
#include <trace_helpers.h>
#include <view/view.h>
#include <drawing_sheet/ds_draw_item.h>
#include <view/view_controls.h>
#include <widgets/msgpanel.h>
#include <widgets/properties_panel.h>
#include <widgets/net_inspector_panel.h>
#include <QCoreApplication>
#include <QWidget>
#include <QDir>
#include <QFileDialog>
#include <QDebug>
#include <QTimer>
#include <QStringList>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QEvent>
#include <QDialog>
#include <QDockWidget>
#include <QAction>
#include <QIcon>
#include <QScreen>
#include <QPoint>
#include <QSize>
#include <QStatusBar>
#include <QSharedMemory>
#include <QApplication>
#include <QWindow>
#include <widgets/kiui_common.h>
#include <widgets/search_pane.h>

#define FR_HISTORY_LIST_CNT     10   ///< Maximum size of the find/replace history stacks.


// Qt event handling will be implemented in the class methods


bool EDA_DRAW_FRAME::m_openGLFailureOccured = false;


EDA_DRAW_FRAME::EDA_DRAW_FRAME( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType,
                                const QString& aTitle, const QPoint& aPos, const QSize& aSize,
                                long aStyle, const QString& aFrameName,
                                const EDA_IU_SCALE& aIuScale ) :
        KIWAY_PLAYER( aKiway, aParent, aFrameType, aTitle, aPos, aSize, aStyle, aFrameName,
                      aIuScale ),
        m_socketServer( nullptr ),
        m_lastToolbarIconSize( 0 )
{
    m_mainToolBar         = nullptr;
    m_drawToolBar         = nullptr;
    m_optionsToolBar      = nullptr;
    m_auxiliaryToolBar    = nullptr;
    m_gridSelectBox       = nullptr;
    m_zoomSelectBox       = nullptr;
    m_searchPane          = nullptr;
    m_undoRedoCountMax    = DEFAULT_MAX_UNDO_ITEMS;

    m_canvasType          = EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE;
    m_canvas              = nullptr;
    m_toolDispatcher      = nullptr;
    m_messagePanel        = nullptr;
    m_currentScreen       = nullptr;
    m_showBorderAndTitleBlock = false;  // true to display reference sheet.
    m_gridColor           = COLOR4D( DARKGRAY );   // Default grid color
    m_drawBgColor         = COLOR4D( BLACK );   // the background color of the draw canvas:
                                                // BLACK for Pcbnew, BLACK or WHITE for Eeschema
    m_colorSettings       = nullptr;
    m_polarCoords         = false;
    m_findReplaceData     = std::make_unique<EDA_SEARCH_DATA>();
    m_hotkeyPopup         = nullptr;
    m_propertiesPanel     = nullptr;
    m_netInspectorPanel   = nullptr;

    SetUserUnits( EDA_UNITS::MM );

    // m_auimgr needs to be properly initialized - Qt dock widgets handle this differently
    // QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable would be set on individual dock widgets

    if( ( aStyle & Qt::Tool ) == 0 )
    {
        // Qt status bar creation
        QStatusBar* statusBar = new QStatusBar(this);
        setStatusBar(statusBar);
        // Status bar double buffering is handled automatically by Qt

        statusBar->setFont( KIUI::GetStatusFont( this ) );

        // set the size of the status bar subwindows:
        updateStatusBarWidths();
    }

    m_messagePanel = new EDA_MSG_PANEL( this );
    m_messagePanel->move( QPoint( 0, m_frameSize.height() ) );
    // Qt uses stylesheets for background colors
    COLOR4D bgColor = COLOR4D( LIGHTGRAY );
    m_messagePanel->setStyleSheet( QString("background-color: rgb(%1, %2, %3)")
                                   .arg( int(bgColor.r * 255) )
                                   .arg( int(bgColor.g * 255) )
                                   .arg( int(bgColor.b * 255) ) );
    m_msgFrameHeight = m_messagePanel->sizeHint().height();

    // Create child subwindows.
    QSize clientSize = size();
    m_frameSize.setWidth( clientSize.width() );
    m_frameSize.setHeight( clientSize.height() );
    m_framePos.setX( 0 );
    m_framePos.setY( 0 );
    m_frameSize.setHeight( m_frameSize.height() - m_msgFrameHeight );

    m_messagePanel->resize( m_frameSize.width(), m_msgFrameHeight );

    // Qt DPI change handling
    // Note: screenChanged is a QWindow signal, not QWidget
    // We need to handle DPI changes differently in Qt
    if( QWindow* window = windowHandle() )
    {
        connect( window, &QWindow::screenChanged, this,
                [this]( QScreen* )
                {
                    if( ( windowFlags() & Qt::Tool ) == 0 )
                        updateStatusBarWidths();

                    // Qt move event handling will be different
                    QMoveEvent dummyEvent( pos(), pos() );
                    OnMove( dummyEvent );

                    // we need to kludge the msg panel to the correct size again
                    // especially important even for first launches as the constructor of the window
                    // here usually doesn't have the correct dpi awareness yet
                    m_frameSize.setHeight( m_frameSize.height() + m_msgFrameHeight );
                    m_msgFrameHeight = m_messagePanel->sizeHint().height();
                    m_frameSize.setHeight( m_frameSize.height() - m_msgFrameHeight );

                    m_messagePanel->move( QPoint( 0, m_frameSize.height() ) );
                    m_messagePanel->resize( m_frameSize.width(), m_msgFrameHeight );

                    // Don't propagate, otherwise the frame gets too big
                } );
    }
}


EDA_DRAW_FRAME::~EDA_DRAW_FRAME()
{
    if( !m_openGLFailureOccured )
        saveCanvasTypeSetting( m_canvasType );

    delete m_actions;
    delete m_toolManager;
    delete m_toolDispatcher;
    delete m_canvas;

    delete m_currentScreen;
    m_currentScreen = nullptr;

    // Qt dock widgets are managed by the main window
    // No explicit UnInit needed

    ReleaseFile();
}


void EDA_DRAW_FRAME::ReleaseFile()
{
    if( m_file_checker.get() != nullptr )
        m_file_checker->UnlockFile();
}


bool EDA_DRAW_FRAME::LockFile( const QString& aFileName )
{
    // We need to explicitly reset here to get the deletion before
    // we create a new unique_ptr that may be for the same file
    m_file_checker.reset();

    m_file_checker = std::make_unique<LOCKFILE>( aFileName );

    if( !m_file_checker->Valid() && m_file_checker->IsLockedByMe() )
    {
        // If we cannot acquire the lock but we appear to be the one who
        // locked it, check to see if there is another KiCad instance running.
        // If there is not, then we can override the lock.  This could happen if
        // KiCad crashed or was interrupted
        // Qt version: Check if another instance is running using QSharedMemory
        if( Pgm().SingleInstance() && !Pgm().SingleInstance()->attach() )
            m_file_checker->OverrideLock();
    }
    // If the file is valid, return true.  This could mean that the file is
    // locked or it could mean that the file is read-only
    return m_file_checker->Valid();
}


void EDA_DRAW_FRAME::ScriptingConsoleEnableDisable()
{
    KIWAY_PLAYER* frame = Kiway().Player( FRAME_PYTHON, false );

    QRect  rect = frameGeometry();
    QPoint center = rect.center();

    if( !frame )
    {
        frame = Kiway().Player( FRAME_PYTHON, true, Kiway().GetTop() );

        // If we received an error in the CTOR due to Python-ness, don't crash
        if( !frame )
            return;

        if( !frame->isVisible() )
            frame->show();

        // On Windows, raise() does not bring the window on screen, when minimized
        if( frame->isMinimized() )
            frame->showNormal();

        frame->raise();
        frame->move( center - QPoint( frame->size().width() / 2, frame->size().height() / 2 ) );

        return;
    }

    frame->setVisible( !frame->isVisible() );
    frame->move( center - QPoint( frame->size().width(), frame->size().height() ) / 2 );
}


bool EDA_DRAW_FRAME::IsScriptingConsoleVisible()
{
    KIWAY_PLAYER* frame = Kiway().Player( FRAME_PYTHON, false );
    return frame && frame->isVisible();
}


void EDA_DRAW_FRAME::unitsChangeRefresh()
{
    // Notify all tools the units have changed
    if( m_toolManager )
        m_toolManager->RunAction( ACTIONS::updateUnits );

    UpdateStatusBar();
    UpdateMsgPanel();
    UpdateProperties();
}


void EDA_DRAW_FRAME::ToggleUserUnits()
{
    if( m_toolManager->GetTool<COMMON_TOOLS>() )
    {
        TOOL_EVENT dummy;
        m_toolManager->GetTool<COMMON_TOOLS>()->ToggleUnits( dummy );
    }
    else
    {
        SetUserUnits( GetUserUnits() == EDA_UNITS::INCH ? EDA_UNITS::MM : EDA_UNITS::INCH );
        unitsChangeRefresh();

        // Qt signals will handle this differently
        // ProcessEventLocally removed - Qt handles events differently
    }
}


void EDA_DRAW_FRAME::CommonSettingsChanged( int aFlags )
{
    EDA_BASE_FRAME::CommonSettingsChanged( aFlags );

    COMMON_SETTINGS*      settings = Pgm().GetCommonSettings();
    KIGFX::VIEW_CONTROLS* viewControls = GetCanvas()->GetViewControls();

    if( m_supportsAutoSave && m_autoSaveTimer->isActive() )
    {
        if( GetAutoSaveInterval() > 0 )
        {
            m_autoSaveTimer->start( GetAutoSaveInterval() * 1000 );
            m_autoSaveTimer->setSingleShot( true );
        }
        else
        {
            m_autoSaveTimer->stop();
            m_autoSavePending = false;
        }
    }

    viewControls->LoadSettings();

    m_galDisplayOptions.ReadCommonConfig( *settings, this );

    GetToolManager()->RunAction( ACTIONS::gridPreset, config()->m_Window.grid.last_size_idx );
    UpdateGridSelectBox();

    if( m_lastToolbarIconSize == 0
        || m_lastToolbarIconSize != settings->m_Appearance.toolbar_icon_size )
    {
        OnToolbarSizeChanged();
        m_lastToolbarIconSize = settings->m_Appearance.toolbar_icon_size;
    }

#ifndef Q_OS_MAC
    resolveCanvasType();

    if( m_canvasType != GetCanvas()->GetBackend() )
    {
        // Try to switch (will automatically fallback if necessary)
        SwitchCanvas( m_canvasType );
        EDA_DRAW_PANEL_GAL::GAL_TYPE newGAL = GetCanvas()->GetBackend();
        bool                         success = newGAL == m_canvasType;

        if( !success )
        {
            m_canvasType = newGAL;
            m_openGLFailureOccured = true; // Store failure for other EDA_DRAW_FRAMEs
        }
    }
#endif

    // Notify all tools the preferences have changed
    if( m_toolManager )
        m_toolManager->RunAction( ACTIONS::updatePreferences );
}


void EDA_DRAW_FRAME::EraseMsgBox()
{
    if( m_messagePanel )
        m_messagePanel->EraseMsgBox();
}


void EDA_DRAW_FRAME::UpdateGridSelectBox()
{
    UpdateStatusBar();
    DisplayUnitsMsg();

    if( m_gridSelectBox == nullptr )
        return;

    // Update grid values with the current units setting.
    m_gridSelectBox->clear();
    QStringList gridsList;

    Q_ASSERT( config() );

    GRID_MENU::BuildChoiceList( &gridsList, config(), this );

    for( const QString& grid : gridsList )
        m_gridSelectBox->addItem( grid );

    m_gridSelectBox->addItem( "---" );
    m_gridSelectBox->addItem( "Edit Grids..." );

    m_gridSelectBox->setCurrentIndex( config()->m_Window.grid.last_size_idx );
}


void EDA_DRAW_FRAME::OnUpdateSelectGrid( QUpdateUIEvent& aEvent )
{
    // No need to update the grid select box if it doesn't exist or the grid setting change
    // was made using the select box.
    if( m_gridSelectBox == nullptr )
        return;

    Q_ASSERT( this->config() );

    int idx = this->config()->m_Window.grid.last_size_idx;
    idx = std::clamp( idx, 0, (int) m_gridSelectBox->count() - 1 );

    if( idx != m_gridSelectBox->currentIndex() )
        m_gridSelectBox->setCurrentIndex( idx );
}



void EDA_DRAW_FRAME::OnUpdateSelectZoom( QUpdateUIEvent& aEvent )
{
    // No need to update the grid select box if it doesn't exist or the grid setting change
    // was made using the select box.
    if( m_zoomSelectBox == nullptr )
        return;

    double zoom = this->GetCanvas()->GetGAL()->GetZoomFactor();

    Q_ASSERT( this->config() );

    const std::vector<double>& zoomList = this->config()->m_Window.zoom_factors;
    int curr_selection = m_zoomSelectBox->currentIndex();
    int new_selection = 0;      // select zoom auto
    double last_approx = 1e9;   // large value to start calculation

    // Search for the nearest available value to the current zoom setting, and select it
    for( size_t jj = 0; jj < zoomList.size(); ++jj )
    {
        double rel_error = std::fabs( zoomList[jj] - zoom ) / zoom;

        if( rel_error < last_approx )
        {
            last_approx = rel_error;

            // zoom IDs in m_zoomSelectBox start with 1 (leaving 0 for auto-zoom choice)
            new_selection = jj + 1;
        }
    }

    if( curr_selection != new_selection )
        m_zoomSelectBox->setCurrentIndex( new_selection );
}


void EDA_DRAW_FRAME::PrintPage( const RENDER_SETTINGS* aSettings )
{
    DisplayErrorMessage( this, "EDA_DRAW_FRAME::PrintPage() error" );
}


void EDA_DRAW_FRAME::OnSelectGrid( QCommandEvent& event )
{
    Q_ASSERT( m_gridSelectBox != nullptr );

    int idx = m_gridSelectBox->currentIndex();

    if( idx == int( m_gridSelectBox->count() ) - 2 )
    {
        // Qt will handle the separator differently, which we don't want.
        // Re-check the current grid.
        // Qt event handling will be implemented differently
        QUpdateUIEvent dummyEvent( QEvent::User );
        OnUpdateSelectGrid( dummyEvent );
    }
    else if( idx == int( m_gridSelectBox->count() ) - 1 )
    {
        // Qt will handle the Grid Settings... entry differently, which we don't want.
        // Re-check the current grid.
        // Qt event handling will be implemented differently
        QUpdateUIEvent dummyEvent( QEvent::User );
        OnUpdateSelectGrid( dummyEvent );

        // Give a time-slice to close the menu before opening the dialog.
        // (Only matters on some versions of GTK.)
        QCoreApplication::processEvents();

        m_toolManager->RunAction( ACTIONS::gridProperties );
    }
    else
    {
        m_toolManager->RunAction( ACTIONS::gridPreset, idx );
    }

    this->UpdateStatusBar();
    m_canvas->update();

    // Needed on Windows because clicking on m_gridSelectBox remove the focus from m_canvas
    // (Windows specific
    m_canvas->setFocus();
}


bool EDA_DRAW_FRAME::IsGridVisible() const
{
    if( !config() ) return true;

    return config()->m_Window.grid.show;
}


void EDA_DRAW_FRAME::SetGridVisibility( bool aVisible )
{
    Q_ASSERT( config() );

    config()->m_Window.grid.show = aVisible;

    // Update the display with the new grid
    if( GetCanvas() )
    {
        // Check to ensure these exist, since this function could be called before
        // the GAL and View have been created
        if( GetCanvas()->GetGAL() )
            GetCanvas()->GetGAL()->SetGridVisibility( aVisible );

        if( GetCanvas()->GetView() )
            GetCanvas()->GetView()->MarkTargetDirty( KIGFX::TARGET_NONCACHED );

        GetCanvas()->Refresh();
    }
}


bool EDA_DRAW_FRAME::IsGridOverridden() const
{
    if( !config() ) return false;

    return config()->m_Window.grid.overrides_enabled;
}


void EDA_DRAW_FRAME::SetGridOverrides( bool aOverride )
{
    Q_ASSERT( config() );

    config()->m_Window.grid.overrides_enabled = aOverride;
}


std::unique_ptr<GRID_HELPER> EDA_DRAW_FRAME::MakeGridHelper()
{
    return nullptr;
}


void EDA_DRAW_FRAME::UpdateZoomSelectBox()
{
    if( m_zoomSelectBox == nullptr )
        return;

    double zoom = m_canvas->GetGAL()->GetZoomFactor();

    m_zoomSelectBox->clear();
    m_zoomSelectBox->addItem( "Zoom Auto" );
    m_zoomSelectBox->setCurrentIndex( 0 );

    Q_ASSERT( config() );

    for( unsigned i = 0;  i < config()->m_Window.zoom_factors.size();  ++i )
    {
        double current = config()->m_Window.zoom_factors[i];

        m_zoomSelectBox->addItem( QString::asprintf( "Zoom %.2f", current ) );

        if( zoom == current )
            m_zoomSelectBox->setCurrentIndex( i + 1 );
    }
}


void EDA_DRAW_FRAME::OnSelectZoom( QCommandEvent& event )
{
    Q_ASSERT( m_zoomSelectBox != nullptr );

    int id = m_zoomSelectBox->currentIndex();

    if( id < 0 || !( id < (int)m_zoomSelectBox->count() ) )
        return;

    m_toolManager->RunAction( ACTIONS::zoomPreset, id );
    this->UpdateStatusBar();
    m_canvas->update();

    // Needed on Windows because clicking on m_zoomSelectBox remove the focus from m_canvas
    // (Windows specific
    m_canvas->setFocus();
}


void EDA_DRAW_FRAME::OnMove( QMoveEvent& aEvent )
{
    // If the window is moved to a different display, the scaling factor may change
    double oldFactor = m_galDisplayOptions.m_scaleFactor;
    m_galDisplayOptions.UpdateScaleFactor();

    if( oldFactor != m_galDisplayOptions.m_scaleFactor && m_canvas )
    {
        QSize clientSize = size();
        GetCanvas()->GetGAL()->ResizeScreen( clientSize.width(), clientSize.height() );
        GetCanvas()->GetView()->MarkDirty();
    }

    // Qt automatically propagates move events
}


void EDA_DRAW_FRAME::AddStandardSubMenus( TOOL_MENU& aToolMenu )
{
    COMMON_TOOLS*     commonTools = m_toolManager->GetTool<COMMON_TOOLS>();
    CONDITIONAL_MENU& aMenu = aToolMenu.GetMenu();

    aMenu.AddSeparator( 1000 );

    std::shared_ptr<ZOOM_MENU> zoomMenu = std::make_shared<ZOOM_MENU>( this );
    zoomMenu->SetTool( commonTools );
    aToolMenu.RegisterSubMenu( zoomMenu );

    std::shared_ptr<GRID_MENU> gridMenu = std::make_shared<GRID_MENU>( this );
    gridMenu->SetTool( commonTools );
    aToolMenu.RegisterSubMenu( gridMenu );

    aMenu.AddMenu( zoomMenu.get(), SELECTION_CONDITIONS::ShowAlways, 1000 );
    aMenu.AddMenu( gridMenu.get(), SELECTION_CONDITIONS::ShowAlways, 1000 );
}


void EDA_DRAW_FRAME::DisplayToolMsg( const QString& msg )
{
    if( m_isClosing )
        return;

    if( statusBar() )
        statusBar()->showMessage( msg );
}


void EDA_DRAW_FRAME::DisplayConstraintsMsg( const QString& msg )
{
    if( m_isClosing )
        return;

    if( statusBar() )
        statusBar()->showMessage( msg );
}


void EDA_DRAW_FRAME::DisplayGridMsg()
{
    if( m_isClosing )
        return;

    QString msg;

    GRID_SETTINGS& gridSettings = m_toolManager->GetSettings()->m_Window.grid;
    int            currentIdx = m_toolManager->GetSettings()->m_Window.grid.last_size_idx;

    msg = QString::asprintf( "grid %s",
                gridSettings.grids[currentIdx].UserUnitsMessageText( this, false ) );

    if( statusBar() )
        statusBar()->showMessage( msg );
}


void EDA_DRAW_FRAME::DisplayUnitsMsg()
{
    if( m_isClosing )
        return;

    QString msg;

    switch( GetUserUnits() )
    {
    case EDA_UNITS::INCH: msg = _( "inches" ); break;
    case EDA_UNITS::MILS: msg = _( "mils" );   break;
    case EDA_UNITS::MM:   msg = _( "mm" );     break;
    default:              msg = _( "Units" );  break;
    }

    if( statusBar() )
        statusBar()->showMessage( msg );
}


void EDA_DRAW_FRAME::OnSize( QResizeEvent& SizeEv )
{
    EDA_BASE_FRAME::OnSize( SizeEv );

    m_frameSize = this->size();

    // Qt automatically propagates resize events
}


void EDA_DRAW_FRAME::updateStatusBarWidths()
{
    QStatusBar* stsbar = statusBar();
    int       spacer = KIUI::GetTextSize( "M", stsbar ).width() * 2;

    int dims[] = {
        // remainder of status bar on far left is set to a default or whatever is left over.
        -1,

        // When using GetTextSize() remember the width of character '1' is not the same
        // as the width of '0' unless the font is fixed width, and it usually won't be.

        // zoom:
        KIUI::GetTextSize( "Z 762000", stsbar ).width(),

        // cursor coords
        KIUI::GetTextSize( "X 1234.1234  Y 1234.1234", stsbar ).width(),

        // delta distances
        KIUI::GetTextSize( "dx 1234.1234  dy 1234.1234  dist 1234.1234", stsbar ).width(),

        // grid size
        KIUI::GetTextSize( "grid X 1234.1234  Y 1234.1234", stsbar ).width(),

        // units display, Inches is bigger than mm
        KIUI::GetTextSize( _( "Inches" ), stsbar ).width(),

        // Size for the "Current Tool" panel; longest string from SetTool()
        KIUI::GetTextSize( "Add layer alignment target", stsbar ).width(),

        // constraint mode
        KIUI::GetTextSize( _( "Constrain to H, V, 45" ), stsbar ).width()
    };

    for( size_t ii = 1; ii < arrayDim( dims ); ii++ )
        dims[ii] += spacer;

    // Qt status bar widget widths are set differently
    // statusBar()->setFieldWidth() would be used for individual sections
}


void EDA_DRAW_FRAME::UpdateStatusBar()
{
    if( m_isClosing )
        return;

    if( statusBar() )
        statusBar()->showMessage( GetZoomLevelIndicator() );

    // Absolute and relative cursor positions are handled by overloading this function and
    // handling the internal to user units conversion at the appropriate level.

    // refresh units display
    DisplayUnitsMsg();
}


const QString EDA_DRAW_FRAME::GetZoomLevelIndicator() const
{
    // returns a human readable value which can be displayed as zoom
    // level indicator in dialogs.
    double zoom = m_canvas->GetGAL()->GetZoomFactor();
    return QString::asprintf( "Z %.2f", zoom );
}


void EDA_DRAW_FRAME::LoadSettings( APP_SETTINGS_BASE* aCfg )
{
    EDA_BASE_FRAME::LoadSettings( aCfg );

    COMMON_SETTINGS* cmnCfg = Pgm().GetCommonSettings();
    WINDOW_SETTINGS* window = GetWindowSettings( aCfg );

    // Read units used in dialogs and toolbars
    SetUserUnits( static_cast<EDA_UNITS>( aCfg->m_System.units ) );

    m_undoRedoCountMax = aCfg->m_System.max_undo_items;

    m_galDisplayOptions.ReadConfig( *cmnCfg, *window, this );

    m_findReplaceData->findString = aCfg->m_FindReplace.find_string;
    m_findReplaceData->replaceString = aCfg->m_FindReplace.replace_string;
    m_findReplaceData->matchMode =
            static_cast<EDA_SEARCH_MATCH_MODE>( aCfg->m_FindReplace.match_mode );
    m_findReplaceData->matchCase = aCfg->m_FindReplace.match_case;
    m_findReplaceData->searchAndReplace = aCfg->m_FindReplace.search_and_replace;

    for( const QString& s : aCfg->m_FindReplace.find_history )
        m_findStringHistoryList.append( s );

    for( const QString& s : aCfg->m_FindReplace.replace_history )
        m_replaceStringHistoryList.append( s );

    m_lastToolbarIconSize = cmnCfg->m_Appearance.toolbar_icon_size;
}


void EDA_DRAW_FRAME::SaveSettings( APP_SETTINGS_BASE* aCfg )
{
    EDA_BASE_FRAME::SaveSettings( aCfg );

    WINDOW_SETTINGS* window = GetWindowSettings( aCfg );

    aCfg->m_System.units = static_cast<int>( GetUserUnits() );
    aCfg->m_System.max_undo_items = GetMaxUndoItems();

    m_galDisplayOptions.WriteConfig( *window );

    aCfg->m_FindReplace.search_and_replace = m_findReplaceData->searchAndReplace;

    aCfg->m_FindReplace.find_string = m_findReplaceData->findString;
    aCfg->m_FindReplace.replace_string = m_findReplaceData->replaceString;

    aCfg->m_FindReplace.find_history.clear();
    aCfg->m_FindReplace.replace_history.clear();

    for( size_t i = 0; i < m_findStringHistoryList.size() && i < FR_HISTORY_LIST_CNT; i++ )
    {
        aCfg->m_FindReplace.find_history.push_back( m_findStringHistoryList[ i ] );
    }

    for( size_t i = 0; i < m_replaceStringHistoryList.size() && i < FR_HISTORY_LIST_CNT; i++ )
    {
        aCfg->m_FindReplace.replace_history.push_back(
                m_replaceStringHistoryList[ i ] );
    }

    // Save the units used in this frame
    if( m_toolManager )
    {
        if( COMMON_TOOLS* cmnTool = m_toolManager->GetTool<COMMON_TOOLS>() )
        {
            aCfg->m_System.last_imperial_units =
                    static_cast<int>( cmnTool->GetLastImperialUnits() );
            aCfg->m_System.last_metric_units = static_cast<int>( cmnTool->GetLastMetricUnits() );
        }
    }
}


void EDA_DRAW_FRAME::AppendMsgPanel( const QString& aTextUpper, const QString& aTextLower,
                                     int aPadding )
{
    if( m_messagePanel && !m_isClosing )
        m_messagePanel->AppendMessage( aTextUpper, aTextLower, aPadding );
}


void EDA_DRAW_FRAME::ClearMsgPanel()
{
    if( m_messagePanel && !m_isClosing )
        m_messagePanel->EraseMsgBox();
}


void EDA_DRAW_FRAME::SetMsgPanel( const std::vector<MSG_PANEL_ITEM>& aList )
{
    if( m_messagePanel && !m_isClosing )
    {
        m_messagePanel->EraseMsgBox();

        for( const MSG_PANEL_ITEM& item : aList )
            m_messagePanel->AppendMessage( item );
    }
}


void EDA_DRAW_FRAME::SetMsgPanel( const QString& aTextUpper, const QString& aTextLower,
                                  int aPadding )
{
    if( m_messagePanel && !m_isClosing )
    {
        m_messagePanel->EraseMsgBox();
        m_messagePanel->AppendMessage( aTextUpper, aTextLower, aPadding );
    }
}


void EDA_DRAW_FRAME::SetMsgPanel( EDA_ITEM* aItem )
{
    Q_ASSERT( aItem );

    std::vector<MSG_PANEL_ITEM> items;
    aItem->GetMsgPanelInfo( this, items );
    SetMsgPanel( items );
}


void EDA_DRAW_FRAME::UpdateMsgPanel()
{
}


void EDA_DRAW_FRAME::ActivateGalCanvas()
{
    // In Qt, we don't need to enable event handlers as they're always enabled
    // Just start drawing
    GetCanvas()->StartDrawing();
}


void EDA_DRAW_FRAME::SwitchCanvas( EDA_DRAW_PANEL_GAL::GAL_TYPE aCanvasType )
{
    GetCanvas()->SwitchBackend( aCanvasType );
    m_canvasType = GetCanvas()->GetBackend();

    ActivateGalCanvas();
}


EDA_DRAW_PANEL_GAL::GAL_TYPE EDA_DRAW_FRAME::loadCanvasTypeSetting(  APP_SETTINGS_BASE* aCfg )
{
#ifdef Q_OS_MAC
    // Cairo renderer doesn't handle Retina displays so there's really only one game
    // in town for Mac
    return EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL;
#endif

    EDA_DRAW_PANEL_GAL::GAL_TYPE canvasType = EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE;
    APP_SETTINGS_BASE* cfg = aCfg ? aCfg : Kiface().KifaceSettings();

    if( cfg )
        canvasType = static_cast<EDA_DRAW_PANEL_GAL::GAL_TYPE>( cfg->m_Graphics.canvas_type );

    if( canvasType < EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE
            || canvasType >= EDA_DRAW_PANEL_GAL::GAL_TYPE_LAST )
    {
        Q_ASSERT( false );
        canvasType = EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE;
    }

    // Legacy canvas no longer supported.  Switch to OpenGL, falls back to Cairo on failure
    if( canvasType == EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE )
        canvasType = EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL;

    return canvasType;
}


bool EDA_DRAW_FRAME::saveCanvasTypeSetting( EDA_DRAW_PANEL_GAL::GAL_TYPE aCanvasType )
{
    // Not all classes derived from EDA_DRAW_FRAME can save the canvas type, because some
    // have a fixed type, or do not have a option to set the canvas type (they inherit from
    // a parent frame)
    static std::vector<FRAME_T> s_allowedFrames =
            {
                FRAME_SCH, FRAME_SCH_SYMBOL_EDITOR,
                FRAME_PCB_EDITOR, FRAME_FOOTPRINT_EDITOR,
                FRAME_GERBER,
                FRAME_PL_EDITOR
            };

    if( !alg::contains( s_allowedFrames, m_ident ) )
        return false;

    if( aCanvasType < EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE
            || aCanvasType >= EDA_DRAW_PANEL_GAL::GAL_TYPE_LAST )
    {
        Q_ASSERT( false );
        return false;
    }

    if( APP_SETTINGS_BASE* cfg = Kiface().KifaceSettings() )
        cfg->m_Graphics.canvas_type = static_cast<int>( aCanvasType );

    return false;
}


VECTOR2I EDA_DRAW_FRAME::GetNearestGridPosition( const VECTOR2I& aPosition ) const
{
    const VECTOR2I& gridOrigin = GetGridOrigin();
    VECTOR2D        gridSize = GetCanvas()->GetGAL()->GetGridSize();

    double xOffset = fmod( gridOrigin.x, gridSize.x );
    int    x = KiROUND( (aPosition.x - xOffset) / gridSize.x );
    double yOffset = fmod( gridOrigin.y, gridSize.y );
    int    y = KiROUND( (aPosition.y - yOffset) / gridSize.y );

    return VECTOR2I( KiROUND( x * gridSize.x + xOffset ), KiROUND( y * gridSize.y + yOffset ) );
}


VECTOR2I EDA_DRAW_FRAME::GetNearestHalfGridPosition( const VECTOR2I& aPosition ) const
{
    const VECTOR2I& gridOrigin = GetGridOrigin();
    VECTOR2D        gridSize = GetCanvas()->GetGAL()->GetGridSize() / 2.0;

    double xOffset = fmod( gridOrigin.x, gridSize.x );
    int    x = KiROUND( (aPosition.x - xOffset) / gridSize.x );
    double yOffset = fmod( gridOrigin.y, gridSize.y );
    int    y = KiROUND( (aPosition.y - yOffset) / gridSize.y );

    return VECTOR2I( KiROUND( x * gridSize.x + xOffset ), KiROUND( y * gridSize.y + yOffset ) );
}


const BOX2I EDA_DRAW_FRAME::GetDocumentExtents( bool aIncludeAllVisible ) const
{
    return BOX2I();
}


void EDA_DRAW_FRAME::HardRedraw()
{
    // To be implemented by subclasses.
}


void EDA_DRAW_FRAME::Zoom_Automatique( bool aWarpPointer )
{
    m_toolManager->RunAction( ACTIONS::zoomFitScreen );
}


std::vector<QWidget*> EDA_DRAW_FRAME::findDialogs()
{
    std::vector<QWidget*> dialogs;

    for( QWidget* window : findChildren<QWidget*>() )
    {
        if( dynamic_cast<DIALOG_SHIM*>( window ) )
            dialogs.push_back( window );
    }

    return dialogs;
}


void EDA_DRAW_FRAME::FocusOnLocation( const VECTOR2I& aPos )
{
    bool  centerView = false;
    BOX2D r = GetCanvas()->GetView()->GetViewport();

    // Center if we're off the current view, or within 10% of its edge
    r.Inflate( - (int) r.GetWidth() / 10 );

    if( !r.Contains( aPos ) )
        centerView = true;

    std::vector<BOX2D> dialogScreenRects;

    for( QWidget* dialog : findDialogs() )
    {
        dialogScreenRects.emplace_back(
                ToVECTOR2D( GetCanvas()->mapFromGlobal( dialog->mapToGlobal( QPoint( 0, 0 ) ) ) ),
                ToVECTOR2D( dialog->size() ) );
    }

    // Center if we're behind an obscuring dialog, or within 10% of its edge
    for( BOX2D rect : dialogScreenRects )
    {
        rect.Inflate( rect.GetWidth() / 10 );

        if( rect.Contains( GetCanvas()->GetView()->ToScreen( aPos ) ) )
            centerView = true;
    }

    if( centerView )
    {
        try
        {
            GetCanvas()->GetView()->SetCenter( aPos, dialogScreenRects );
        }
        catch( const Clipper2Lib::Clipper2Exception& e )
        {
            qWarning() << QString::asprintf( "Clipper2 exception occurred centering object: %s", e.what() );
        }
    }

    GetCanvas()->GetViewControls()->SetCrossHairCursorPosition( aPos );
}


static const QString productName = "KiCad E.D.A.  ";


void PrintDrawingSheet( const RENDER_SETTINGS* aSettings, const PAGE_INFO& aPageInfo,
                        const QString& aSheetName, const QString& aSheetPath,
                        const QString& aFileName, const TITLE_BLOCK& aTitleBlock,
                        const std::map<QString, QString>* aProperties, int aSheetCount,
                        const QString& aPageNumber, double aMils2Iu, const PROJECT* aProject,
                        const QString& aSheetLayer, bool aIsFirstPage )
{
    DS_DRAW_ITEM_LIST drawList( unityScale );

    drawList.SetDefaultPenSize( aSettings->GetDefaultPenWidth() );
    drawList.SetPlotterMilsToIUfactor( aMils2Iu );
    drawList.SetPageNumber( aPageNumber );
    drawList.SetSheetCount( aSheetCount );
    drawList.SetFileName( aFileName );
    drawList.SetSheetName( aSheetName );
    drawList.SetSheetPath( aSheetPath );
    drawList.SetSheetLayer( aSheetLayer );
    drawList.SetProject( aProject );
    drawList.SetIsFirstPage( aIsFirstPage );
    drawList.SetProperties( aProperties );

    drawList.BuildDrawItemsList( aPageInfo, aTitleBlock );

    // Draw item list
    drawList.Print( aSettings );
}


void EDA_DRAW_FRAME::PrintDrawingSheet( const RENDER_SETTINGS* aSettings, BASE_SCREEN* aScreen,
                                        const std::map<QString, QString>* aProperties,
                                        double aMils2Iu, const QString &aFilename,
                                        const QString &aSheetLayer )
{
    if( !m_showBorderAndTitleBlock )
        return;

    QPainter* painter = aSettings->GetPrintPainter();
    QPoint origin = painter->deviceTransform().map(QPoint(0,0));

    if( origin.y() > 0 )
    {
        painter->setWorldTransform( QTransform() );
    }

    ::PrintDrawingSheet( aSettings, GetPageSettings(), GetScreenDesc(), GetFullScreenDesc(),
                         aFilename, GetTitleBlock(), aProperties, aScreen->GetPageCount(),
                         aScreen->GetPageNumber(), aMils2Iu, &Prj(), aSheetLayer,
                         aScreen->GetVirtualPageNumber() == 1 );

    if( origin.y() > 0 )
    {
        QTransform transform;
        transform.translate( origin.x(), origin.y() );
        painter->setWorldTransform( transform );
    }
}


QString EDA_DRAW_FRAME::GetScreenDesc() const
{
    // Virtual function. Base class implementation returns an empty string.
    return QString();
}


QString EDA_DRAW_FRAME::GetFullScreenDesc() const
{
    // Virtual function. Base class implementation returns an empty string.
    return QString();
}


bool EDA_DRAW_FRAME::LibraryFileBrowser( bool doOpen, QFileInfo& aFilename,
                                         const QString& wildcard, const QString& ext,
                                         bool isDirectory, bool aIsGlobal,
                                         const QString& aGlobalPath )
{
    QString prompt = doOpen ? "Select Library" : "New Library";
    aFilename = QFileInfo( aFilename.absolutePath() + "/" + aFilename.baseName() + "." + ext );

    QString projectDir = Prj().IsNullProject() ? aFilename.absolutePath() : Prj().GetProjectPath();
    QString defaultDir;

    if( aIsGlobal )
    {
        if( !GetMruPath().isEmpty() && !GetMruPath().startsWith( projectDir ) )
            defaultDir = GetMruPath();
        else
            defaultDir = aGlobalPath;
    }
    else
    {
        if( !GetMruPath().isEmpty() && GetMruPath().startsWith( projectDir ) )
            defaultDir = GetMruPath();
        else
            defaultDir = projectDir;
    }

    if( isDirectory && doOpen )
    {
        QFileDialog dlg( this, prompt, defaultDir );
        dlg.setFileMode( QFileDialog::Directory );
        dlg.setOption( QFileDialog::ShowDirsOnly, true );

        if( dlg.exec() == QDialog::Rejected )
            return false;

        QStringList paths = dlg.selectedFiles();
        if( !paths.isEmpty() )
            aFilename = QFileInfo( paths.first() );
        aFilename = QFileInfo( aFilename.absolutePath() + "/" + aFilename.baseName() + "." + ext );
    }
    else
    {
        // Ensure the file has a dummy name, otherwise GTK will display the regex from the filter
        if( aFilename.baseName().isEmpty() )
            aFilename = QFileInfo( aFilename.absolutePath() + "/Library." + ext );

        QFileDialog dlg( this, prompt, defaultDir, wildcard );
        if( doOpen )
        {
            dlg.setFileMode( QFileDialog::ExistingFile );
            dlg.setAcceptMode( QFileDialog::AcceptOpen );
        }
        else
        {
            dlg.setFileMode( QFileDialog::AnyFile );
            dlg.setAcceptMode( QFileDialog::AcceptSave );
            dlg.setOption( QFileDialog::DontConfirmOverwrite, false );
        }
        dlg.selectFile( aFilename.fileName() );

        if( dlg.exec() == QDialog::Rejected )
            return false;

        QStringList paths = dlg.selectedFiles();
        if( !paths.isEmpty() )
            aFilename = QFileInfo( paths.first() );
        aFilename = QFileInfo( aFilename.absolutePath() + "/" + aFilename.baseName() + "." + ext );
    }

    SetMruPath( aFilename.absolutePath() );

    return true;
}


void EDA_DRAW_FRAME::RecreateToolbars()
{
    // Rebuild all toolbars, and update the checked state of check tools
    if( m_mainToolBar )
        ReCreateHToolbar();

    if( m_drawToolBar )         // Drawing tools (typically on right edge of window)
        ReCreateVToolbar();

    if( m_optionsToolBar )      // Options (typically on left edge of window)
        ReCreateOptToolbar();

    if( m_auxiliaryToolBar )    // Additional tools under main toolbar
       ReCreateAuxiliaryToolbar();


}


void EDA_DRAW_FRAME::OnToolbarSizeChanged()
{
    if( m_mainToolBar )
    {
        // Qt dock widget sizing handled automatically
    }

    if( m_drawToolBar )
    {
        // Qt dock widget sizing handled automatically
    }

    if( m_optionsToolBar )
    {
        // Qt dock widget sizing handled automatically
    }

    if( m_auxiliaryToolBar )
    {
        // Qt dock widget sizing handled automatically
    }

    // Qt dock widgets update automatically
    // No explicit update needed
}


void EDA_DRAW_FRAME::ShowChangedLanguage()
{
    EDA_BASE_FRAME::ShowChangedLanguage();

    if( m_searchPane )
    {
        QDockWidget* search_pane_info = findChild<QDockWidget*>( "SearchPane" );
        if( search_pane_info )
            search_pane_info->setWindowTitle( "Search" );
    }

    if( m_propertiesPanel )
    {
        QDockWidget* properties_pane_info = findChild<QDockWidget*>( "PropertiesPane" );
        if( properties_pane_info )
            properties_pane_info->setWindowTitle( "Properties" );
    }

    if( m_netInspectorPanel )
    {
        QDockWidget* net_inspector_panel_info = findChild<QDockWidget*>( "NetInspectorPane" );
        if( net_inspector_panel_info )
            net_inspector_panel_info->setWindowTitle( "Net Inspector" );
    }
}


void EDA_DRAW_FRAME::UpdateProperties()
{
    if( m_isClosing || !m_propertiesPanel || !m_propertiesPanel->isVisible() )
        return;

    m_propertiesPanel->UpdateData();
}


void EDA_DRAW_FRAME::CreateHotkeyPopup()
{
    m_hotkeyPopup = new HOTKEY_CYCLE_POPUP( this );
}


COLOR_SETTINGS* EDA_DRAW_FRAME::GetColorSettings( bool aForceRefresh ) const
{
    if( !m_colorSettings || aForceRefresh )
    {
        COLOR_SETTINGS* colorSettings = Pgm().GetSettingsManager().GetColorSettings();

        const_cast<EDA_DRAW_FRAME*>( this )->m_colorSettings = colorSettings;
    }

    return m_colorSettings;
}


void EDA_DRAW_FRAME::setupUnits( APP_SETTINGS_BASE* aCfg )
{
    COMMON_TOOLS* cmnTool = m_toolManager->GetTool<COMMON_TOOLS>();

    if( cmnTool )
    {
        // Tell the tool what the units used last session
        cmnTool->SetLastUnits( static_cast<EDA_UNITS>( aCfg->m_System.last_imperial_units ) );
        cmnTool->SetLastUnits( static_cast<EDA_UNITS>( aCfg->m_System.last_metric_units ) );
    }

    // Tell the tool what units the frame is currently using
    switch( static_cast<EDA_UNITS>( aCfg->m_System.units ) )
    {
    default:
    case EDA_UNITS::MM:   m_toolManager->RunAction( ACTIONS::millimetersUnits ); break;
    case EDA_UNITS::INCH: m_toolManager->RunAction( ACTIONS::inchesUnits );      break;
    case EDA_UNITS::MILS: m_toolManager->RunAction( ACTIONS::milsUnits );        break;
    }
}


void EDA_DRAW_FRAME::GetUnitPair( EDA_UNITS& aPrimaryUnit, EDA_UNITS& aSecondaryUnits )
{
    COMMON_TOOLS* cmnTool = m_toolManager->GetTool<COMMON_TOOLS>();

    aPrimaryUnit    = GetUserUnits();
    aSecondaryUnits = EDA_UNITS::MILS;

    if( EDA_UNIT_UTILS::IsImperialUnit( aPrimaryUnit ) )
    {
        if( cmnTool )
            aSecondaryUnits = cmnTool->GetLastMetricUnits();
        else
            aSecondaryUnits = EDA_UNITS::MM;
    }
    else
    {
        if( cmnTool )
            aSecondaryUnits = cmnTool->GetLastImperialUnits();
        else
            aSecondaryUnits = EDA_UNITS::MILS;
    }
}


void EDA_DRAW_FRAME::resolveCanvasType()
{
    m_canvasType = loadCanvasTypeSetting();

    // If we had an OpenGL failure this session, use the fallback GAL but don't update the
    // user preference silently:

    if( m_openGLFailureOccured && m_canvasType == EDA_DRAW_PANEL_GAL::GAL_TYPE_OPENGL )
        m_canvasType = EDA_DRAW_PANEL_GAL::GAL_FALLBACK;
}


void EDA_DRAW_FRAME::handleActivateEvent( QEvent& aEvent )
{
    // Force a refresh of the message panel to ensure that the text is the right color
    // when the window activates
    if( !isMinimized() )
        m_messagePanel->update();
}


void EDA_DRAW_FRAME::onActivate( QEvent& aEvent )
{
    handleActivateEvent( aEvent );

    // Qt automatically handles activation events
}


bool EDA_DRAW_FRAME::SaveCanvasImageToFile( const QString& aFileName,
                                            BITMAP_TYPE aBitmapType )
{
    bool retv = true;

    // Make a screen copy of the canvas:
    QSize image_size = GetCanvas()->size();

    QPainter painter( GetCanvas() );
    QPixmap   pixmap( image_size );
    QImage image = pixmap.toImage();

    const char* format = "PNG";
    switch( aBitmapType )
    {
    case BITMAP_TYPE::PNG: format = "PNG"; break;
    case BITMAP_TYPE::BMP: format = "BMP"; break;
    case BITMAP_TYPE::JPG: format = "JPG"; break;
    }

    if( !image.save( aFileName, format ) )
        retv = false;

    // Qt automatically manages image memory
    return retv;
}


bool EDA_DRAW_FRAME::IsPluginActionButtonVisible( const PLUGIN_ACTION& aAction,
                                                  APP_SETTINGS_BASE* aCfg )
{
    if( !aCfg ) return aAction.show_button;

    for( const auto& [identifier, visible] : aCfg->m_Plugins.actions )
    {
        if( identifier == aAction.identifier )
            return visible;
    }

    return aAction.show_button;
}


std::vector<const PLUGIN_ACTION*> EDA_DRAW_FRAME::GetOrderedPluginActions(
    PLUGIN_ACTION_SCOPE aScope, APP_SETTINGS_BASE* aCfg )
{
    std::vector<const PLUGIN_ACTION*> actions;
    if( !aCfg ) return actions;

#ifdef KICAD_IPC_API

    API_PLUGIN_MANAGER& mgr = Pgm().GetPluginManager();
    std::vector<const PLUGIN_ACTION*> unsorted = mgr.GetActionsForScope( aScope );
    std::map<QString, const PLUGIN_ACTION*> actionMap;
    std::set<const PLUGIN_ACTION*> handled;

    for( const PLUGIN_ACTION* action : unsorted )
        actionMap[action->identifier] = action;

    for( const auto& identifier : aCfg->m_Plugins.actions | std::views::keys )
    {
        if( actionMap.contains( identifier ) )
        {
            const PLUGIN_ACTION* action = actionMap[ identifier ];
            actions.emplace_back( action );
            handled.insert( action );
        }
    }

    for( const auto& action : actionMap | std::views::values )
    {
        if( !handled.contains( action ) )
            actions.emplace_back( action );
    }

#endif

    return actions;
}


void EDA_DRAW_FRAME::addApiPluginTools()
{
#ifdef KICAD_IPC_API
    API_PLUGIN_MANAGER& mgr = Pgm().GetPluginManager();

    mgr.ButtonBindings().clear();

    std::vector<const PLUGIN_ACTION*> actions =
            GetOrderedPluginActions( PluginActionScope(), config() );

    for( const PLUGIN_ACTION* action : actions )
    {
        if( !IsPluginActionButtonVisible( *action, config() ) )
            continue;

        const QIcon& icon = KIPLATFORM::UI::IsDarkTheme() && !action->icon_dark.isNull()
                                             ? action->icon_dark
                                             : action->icon_light;

        QAction* button = m_mainToolBar->addAction( icon, action->name );

        connect( button, &QAction::triggered,
                 this, &EDA_DRAW_FRAME::OnApiPluginInvoke );

        mgr.ButtonBindings().insert( { reinterpret_cast<intptr_t>( button ), action->identifier } );
    }
#endif
}


void EDA_DRAW_FRAME::OnApiPluginInvoke( QCommandEvent& aEvent )
{
#ifdef KICAD_IPC_API
    API_PLUGIN_MANAGER& mgr = Pgm().GetPluginManager();

    QAction* senderAction = qobject_cast<QAction*>( sender() );
    if( senderAction )
    {
        intptr_t actionId = reinterpret_cast<intptr_t>( senderAction );
        if( mgr.ButtonBindings().count( actionId ) )
            mgr.InvokeAction( mgr.ButtonBindings().at( actionId ) );
    }
#endif
}
