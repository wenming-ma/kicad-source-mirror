#include <eda_base_frame.h>

#include <advanced_config.h>
#include <bitmaps.h>
#include <bitmap_store.h>
#include <dialog_shim.h>
#include <dialogs/panel_common_settings.h>
#include <dialogs/panel_mouse_settings.h>
#include <eda_dde.h>
#include <file_history.h>
#include <id.h>
#include <kiface_base.h>
#include <hotkeys_basic.h>
#include <panel_hotkeys_editor.h>
#include <paths.h>
#include <confirm.h>
#include <pgm_base.h>
#include <settings/app_settings.h>
#include <settings/common_settings.h>
#include <settings/settings_manager.h>
#include <project/project_local_settings.h>
#include <tool/action_manager.h>
#include <tool/action_menu.h>
#include <tool/action_toolbar.h>
#include <tool/actions.h>
#include <tool/common_control.h>
#include <tool/tool_manager.h>
#include <tool/tool_dispatcher.h>
#include <trace_helpers.h>
#include <widgets/paged_dialog.h>
#include <widgets/wx_busy_indicator.h>
#include <widgets/wx_infobar.h>
#include <widgets/wx_aui_art_providers.h>
#include <widgets/wx_grid.h>
#include <widgets/wx_treebook.h>
#include <QApplication>
#include <QSettings>
#include <QScreen>
#include <QStandardPaths>
#include <QString>
#include <QMessageBox>
#include <QTimer>
#include <QMainWindow>
#include <QWidget>
#include <QKeyEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QFileInfo>
#include <QDir>
#include <QFile>
#include <QShortcut>
#include <QMenuBar>
#include <QDropEvent>
#include <QMimeData>
#include <QUrl>
#include <QLineEdit>
#include <QDebug>
#include <kiplatform/app.h>
#include <kiplatform/io.h>
#include <kiplatform/ui.h>

#include <functional>
#include <kiface_ids.h>

#ifdef KICAD_IPC_API
#include <api/api_server.h>
#endif


// Minimum window size
static const QSize minSizeLookup( FRAME_T aFrameType, QWidget* aWidget )
{
    switch( aFrameType )
    {
    case KICAD_MAIN_FRAME_T:
        return QSize( 406, 354 );

    default:
        return QSize( 500, 400 );
    }
}


static const QSize defaultSize( FRAME_T aFrameType, QWidget* aWidget )
{
    switch( aFrameType )
    {
    case KICAD_MAIN_FRAME_T:
        return QSize( 850, 540 );

    default:
        return QSize( 1280, 720 );
    }
}


// Qt event handling replaces wxWidgets event table


void EDA_BASE_FRAME::commonInit( FRAME_T aFrameType )
{
    m_ident             = aFrameType;
    m_maximizeByDefault = false;
    m_infoBar           = nullptr;
    m_settingsManager   = nullptr;
    m_fileHistory       = nullptr;
    m_supportsAutoSave  = false;
    m_autoSavePending   = false;
    m_undoRedoCountMax  = DEFAULT_MAX_UNDO_ITEMS;
    m_isClosing         = false;
    m_isNonUserClose    = false;
    m_autoSaveTimer     = new QTimer( this );
    m_autoSaveTimer->setSingleShot( true );
    m_autoSaveRequired  = false;
    m_mruPath           = PATHS::GetDefaultUserProjectsPath();
    m_frameSize         = defaultSize( aFrameType, this );
    m_displayIndex      = -1;

    m_settingsManager = &Pgm().GetSettingsManager();

    // Set a reasonable minimal size for the frame
    QSize minSize = minSizeLookup( aFrameType, this );
    setMinimumSize( minSize );

    // Store dimensions of the user area of the main window.
    QSize clientSize = size();
    m_frameSize.x = clientSize.width();
    m_frameSize.y = clientSize.height();

    connect( m_autoSaveTimer, &QTimer::timeout, this, &EDA_BASE_FRAME::onAutoSaveTimer );

    initExitKey();
}


EDA_BASE_FRAME::EDA_BASE_FRAME( QWidget* aParent, FRAME_T aFrameType, const QString& aTitle,
                                const QPoint& aPos, const QSize& aSize, Qt::WindowFlags aFlags,
                                const QString& aFrameName, KIWAY* aKiway,
                                const EDA_IU_SCALE& aIuScale ) :
        QMainWindow( aParent ),
        TOOLS_HOLDER(),
        KIWAY_HOLDER( aKiway, KIWAY_HOLDER::FRAME ),
        UNITS_PROVIDER( aIuScale, EDA_UNITS::MM )
{
    commonInit( aFrameType );
}


QWidget* findQuasiModalDialog( QWidget* aParent )
{
    for( QObject* obj : aParent->children() )
    {
        QWidget* child = qobject_cast<QWidget*>( obj );
        if( !child )
            continue;

        if( DIALOG_SHIM* dlg = dynamic_cast<DIALOG_SHIM*>( child ) )
        {
            if( dlg->IsQuasiModal() )
                return dlg;

            if( QWidget* nestedDlg = findQuasiModalDialog( child ) )
                return nestedDlg;
        }
    }

    return nullptr;
}


QWidget* EDA_BASE_FRAME::findQuasiModalDialog()
{
    if( QWidget* dlg = ::findQuasiModalDialog( this ) )
        return dlg;

    if( m_ident == FRAME_SCH )
    {
        QWidget* cvpcb = QApplication::activeWindow()->findChild<QWidget*>( "CvpcbFrame" );

        if( cvpcb )
            return cvpcb;
    }

    return nullptr;
}


void EDA_BASE_FRAME::windowClosing( QCloseEvent& event )
{
    // Don't allow closing when a quasi-modal is open.
    QWidget* quasiModal = findQuasiModalDialog();

    if( quasiModal )
    {
        // Raise and notify; don't give the user a warning regarding "quasi-modal dialogs"
        // when they have no idea what those are.
        quasiModal->raise();
        QApplication::beep();

        event.ignore();

        return;
    }


    // Qt handles session management differently
    // End session detection would be handled through QGuiApplication::commitDataRequest

    if( canCloseWindow( event ) )
    {
        m_isClosing = true;

        if( m_infoBar )
            m_infoBar->Dismiss();

        APP_SETTINGS_BASE* cfg = config();

        if( cfg )
            SaveSettings( cfg );    // virtual, QMainWindow specific

        doCloseWindow();

        // Qt handles widget destruction automatically
        // In modal mode, the caller manages the dialog lifecycle
        if( !isModal() )
            deleteLater();
    }
    else
    {
        event.ignore();
    }
}


EDA_BASE_FRAME::~EDA_BASE_FRAME()
{
    // Qt automatically handles signal disconnections when objects are destroyed

    delete m_autoSaveTimer;
    delete m_fileHistory;

    ClearUndoRedoList();

    SocketCleanup();

    KIPLATFORM::APP::RemoveShutdownBlockReason( this );
}


bool EDA_BASE_FRAME::ProcessEvent( QEvent& aEvent )
{
#ifdef Q_OS_MACOS
    // macOS specific handling for disabled windows and quasi-modal dialogs
    if( !isEnabled() && isActiveWindow() )
    {
        QWidget* dlg = findQuasiModalDialog();

        if( dlg )
            dlg->raise();
    }
#endif

    if( !QMainWindow::event( &aEvent ) )
        return false;

    if( Pgm().m_Quitting )
        return true;

    if( !m_isClosing && m_supportsAutoSave && isVisible() && isActiveWindow()
            && m_autoSavePending != isAutoSaveRequired()
            && GetAutoSaveInterval() > 0 )
    {
        if( !m_autoSavePending )
        {
            qDebug() << "Starting auto save timer.";
            m_autoSaveTimer->start( GetAutoSaveInterval() * 1000 );
            m_autoSavePending = true;
        }
        else if( m_autoSaveTimer->isActive() )
        {
            qDebug() << "Stopping auto save timer.";
            m_autoSaveTimer->stop();
            m_autoSavePending = false;
        }
    }

    return true;
}


int EDA_BASE_FRAME::GetAutoSaveInterval() const
{
    return Pgm().GetCommonSettings()->m_System.autosave_interval;
}


void EDA_BASE_FRAME::onAutoSaveTimer()
{
    // Qt timer signals are automatically routed correctly

    if( !doAutoSave() )
        m_autoSaveTimer->start( GetAutoSaveInterval() * 1000 );
}


bool EDA_BASE_FRAME::doAutoSave()
{
    Q_ASSERT_X( false, "EDA_BASE_FRAME::doAutoSave", "Auto save timer function not overridden. Bad programmer!" );
    return true;
}


void EDA_BASE_FRAME::OnCharHook( QKeyEvent& aKeyEvent )
{
    qDebug() << "EDA_BASE_FRAME::OnCharHook key=" << aKeyEvent.key();

    // Key events can be filtered here.
    // Currently no filtering is made.
}


void EDA_BASE_FRAME::OnMenuEvent( QEvent& aEvent )
{
    if( m_toolDispatcher )
        m_toolDispatcher->DispatchQtEvent( aEvent );
}


void EDA_BASE_FRAME::RegisterUIUpdateHandler( int aID, const ACTION_CONDITIONS& aConditions )
{
    UIUpdateHandler evtFunc = std::bind( &EDA_BASE_FRAME::HandleUpdateUIEvent,
                                         std::placeholders::_1,
                                         this,
                                         aConditions );

    m_uiUpdateMap[aID] = evtFunc;

    // Qt uses actions and signals instead of UI update events
}


void EDA_BASE_FRAME::UnregisterUIUpdateHandler( int aID )
{
    const auto it = m_uiUpdateMap.find( aID );

    if( it == m_uiUpdateMap.end() )
        return;

    // Qt automatically handles action cleanup
}


void EDA_BASE_FRAME::HandleUpdateUIEvent( QEvent& aEvent, EDA_BASE_FRAME* aFrame,
                                          ACTION_CONDITIONS aCond )
{
    bool       checkRes  = false;
    bool       enableRes = true;
    bool       showRes   = true;
    bool       isCut     = aEvent.GetId() == ACTIONS::cut.GetUIId();
    bool       isCopy    = aEvent.GetId() == ACTIONS::copy.GetUIId();
    bool       isPaste   = aEvent.GetId() == ACTIONS::paste.GetUIId();
    SELECTION& selection = aFrame->GetCurrentSelection();

    try
    {
        checkRes  = aCond.checkCondition( selection );
        enableRes = aCond.enableCondition( selection );
        showRes   = aCond.showCondition( selection );
    }
    catch( std::exception& )
    {
        // Something broke with the conditions, just skip the event.
        return;
    }

    if( showRes && aEvent.GetId() == ACTIONS::undo.GetUIId() )
    {
        QString msg = _( "Undo" );

        if( enableRes )
            msg += " " + aFrame->GetUndoActionDescription();

        // Qt action text update would be handled through QAction
    }
    else if( showRes && aEvent.GetId() == ACTIONS::redo.GetUIId() )
    {
        QString msg = _( "Redo" );

        if( enableRes )
            msg += " " + aFrame->GetRedoActionDescription();

        // Qt action text update would be handled through QAction
    }

    if( isCut || isCopy || isPaste )
    {
        QWidget*    focus = QApplication::focusWidget();
        QLineEdit* textEntry = qobject_cast<QLineEdit*>( focus );

        if( textEntry && isCut && !textEntry->isReadOnly() && textEntry->hasSelectedText() )
            enableRes = true;
        else if( textEntry && isCopy && textEntry->hasSelectedText() )
            enableRes = true;
        else if( textEntry && isPaste && !textEntry->isReadOnly() )
            enableRes = true;
        // Note: WX_GRID reference removed - Qt equivalent would be QTableWidget or QTableView
        // else if( dynamic_cast<QTableWidget*>( focus ) )
        //     enableRes = false;  // Must disable menu in order to get command as CharHook event
    }

    // Qt action state updates would be handled through QAction properties
}


void EDA_BASE_FRAME::setupUIConditions()
{
    // Setup the conditions to check a language menu item
    auto isCurrentLang =
            [] ( const SELECTION& aSel, int aLangIdentifier )
            {
                return Pgm().GetSelectedLanguageIdentifier() == aLangIdentifier;
            };

    for( unsigned ii = 0;  LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
    {
        ACTION_CONDITIONS cond;
        cond.Check( std::bind( isCurrentLang, std::placeholders::_1,
                               LanguagesList[ii].m_KI_Lang_Identifier ) );

        RegisterUIUpdateHandler( LanguagesList[ii].m_KI_Lang_Identifier, cond );
    }
}


void EDA_BASE_FRAME::ReCreateMenuBar()
{
    QTimer::singleShot( 0, this, [this]()
               {
                   if( !m_isClosing )
                       doReCreateMenuBar();
               } );
}


void EDA_BASE_FRAME::AddStandardHelpMenu( QMenuBar* aMenuBar )
{
    COMMON_CONTROL* commonControl = m_toolManager->GetTool<COMMON_CONTROL>();
    ACTION_MENU*    helpMenu = new ACTION_MENU( false, commonControl );

    helpMenu->Add( ACTIONS::help );
    helpMenu->Add( ACTIONS::gettingStarted );
    helpMenu->Add( ACTIONS::listHotKeys );
    helpMenu->Add( ACTIONS::getInvolved );
    helpMenu->Add( ACTIONS::donate );
    helpMenu->Add( ACTIONS::reportBug );

    helpMenu->AppendSeparator();
    helpMenu->Add( ACTIONS::about );

    aMenuBar->addMenu( helpMenu );
    helpMenu->setTitle( _( "&Help" ) );
}


void EDA_BASE_FRAME::ShowChangedLanguage()
{
    TOOLS_HOLDER::ShowChangedLanguage();

    if( menuBar() )
    {
        ReCreateMenuBar();
        menuBar()->update();
    }
}


void EDA_BASE_FRAME::CommonSettingsChanged( int aFlags )
{
    TOOLS_HOLDER::CommonSettingsChanged( aFlags );

    COMMON_SETTINGS* settings = Pgm().GetCommonSettings();

#ifdef KICAD_IPC_API
    bool running = Pgm().GetApiServer().Running();

    if( running && !settings->m_Api.enable_server )
        Pgm().GetApiServer().Stop();
    else if( !running && settings->m_Api.enable_server )
        Pgm().GetApiServer().Start();
#endif

    if( m_fileHistory )
    {
        int historySize = settings->m_System.file_history_size;
        m_fileHistory->SetMaxFiles( (unsigned) std::max( 0, historySize ) );
    }

    GetBitmapStore()->ThemeChanged();
    ThemeChanged();

    if( menuBar() )
    {
        // For icons in menus, icon scaling & hotkeys
        ReCreateMenuBar();
        menuBar()->update();
    }
}


void EDA_BASE_FRAME::ThemeChanged()
{
    ClearScaledBitmapCache();

    // Update all the toolbars to have new icons
    // Qt docking system would be used instead of wxAUI
    // TODO: Implement toolbar refresh for Qt docking system
    // This section needs to be implemented with proper Qt dock widgets and toolbars
}


void EDA_BASE_FRAME::OnSize( QResizeEvent& aEvent )
{
#ifdef Q_OS_MACOS
    int currentDisplay = QApplication::desktop()->screenNumber( this );

    if( m_displayIndex >= 0 && currentDisplay >= 0 && currentDisplay != m_displayIndex )
    {
        qDebug() << "OnSize: current display changed" << m_displayIndex << "to" << currentDisplay;
        m_displayIndex = currentDisplay;
        ensureWindowIsOnScreen();
    }
#endif

    QMainWindow::resizeEvent( aEvent );
}


void EDA_BASE_FRAME::LoadWindowState( const QString& aFileName )
{
    if( !Pgm().GetCommonSettings()->m_Session.remember_open_files )
        return;

    const PROJECT_FILE_STATE* state = Prj().GetLocalSettings().GetFileState( aFileName );

    if( state != nullptr )
    {
        LoadWindowState( state->window );
    }
}


void EDA_BASE_FRAME::LoadWindowState( const WINDOW_STATE& aState )
{
    bool wasDefault = false;

    m_framePos.x  = aState.pos_x;
    m_framePos.y  = aState.pos_y;
    m_frameSize.x = aState.size_x;
    m_frameSize.y = aState.size_y;

    qDebug() << "Config position (" << m_framePos.x() << "," << m_framePos.y() << ") with size (" << m_frameSize.x << "," << m_frameSize.y << ")";

    // Ensure minimum size is set if the stored config was zero-initialized
    QSize minSize = minSizeLookup( m_ident, this );

    if( m_frameSize.x < minSize.width() || m_frameSize.y < minSize.height() )
    {
        m_frameSize = defaultSize( m_ident, this );
        wasDefault  = true;

        qDebug() << "Using minimum size (" << m_frameSize.x << "," << m_frameSize.y << ")";
    }

    qDebug() << "Number of displays:" << QApplication::desktop()->screenCount();

    if( aState.display >= QApplication::desktop()->screenCount() )
    {
        qDebug() << "Previous display not found";

        // Get primary screen geometry
        const unsigned int index = 0;
        QScreen* screen = QApplication::screens().at( index );
        QRect    clientSize = screen->geometry();

        m_framePos = QPoint( -1, -1 ); // Qt equivalent of default position

        // Ensure the window fits on the display, since the other one could have been larger
        if( m_frameSize.x > clientSize.width() )
            m_frameSize.x = clientSize.width();

        if( m_frameSize.y > clientSize.height() )
            m_frameSize.y = clientSize.height();
    }
    else
    {
        QPoint upperRight( m_framePos.x() + m_frameSize.x, m_framePos.y() );
        QPoint upperLeft( m_framePos.x(), m_framePos.y() );

        QScreen* screen = QApplication::screens().at( aState.display );
        QRect clientSize = screen->availableGeometry();

        int yLimTop   = clientSize.y();
        int yLimBottom = clientSize.y() + clientSize.height();
        int xLimLeft  = clientSize.x();
        int xLimRight = clientSize.x() + clientSize.width();

        if( upperLeft.x()  > xLimRight ||  // Upper left corner too close to right edge of screen
            upperRight.x() < xLimLeft  ||  // Upper right corner too close to left edge of screen
            upperLeft.y() < yLimTop ||   // Upper corner too close to the bottom of the screen
            upperLeft.y() > yLimBottom )
        {
            m_framePos = QPoint( -1, -1 ); // Qt equivalent of default position
            qDebug() << "Resetting to default position";
        }
    }

    qDebug() << "Final window position (" << m_framePos.x() << "," << m_framePos.y() << ") with size (" << m_frameSize.x << "," << m_frameSize.y << ")";

    setGeometry( m_framePos.x(), m_framePos.y(), m_frameSize.x, m_frameSize.y );

    // Center the window if we reset to default
    if( m_framePos.x() == -1 )
    {
        qDebug() << "Centering window";
        move( QApplication::desktop()->screen()->rect().center() - rect().center() );
        m_framePos = pos();
    }

    // Record the frame sizes in an un-maximized state
    m_normalFrameSize = m_frameSize;
    m_normalFramePos  = m_framePos;

    // Maximize if we were maximized before
    if( aState.maximized || ( wasDefault && m_maximizeByDefault ) )
    {
        qDebug() << "Maximizing window";
        showMaximized();
    }

    m_displayIndex = QApplication::desktop()->screenNumber( this );
}


void EDA_BASE_FRAME::ensureWindowIsOnScreen()
{
    QScreen* screen = QApplication::screenAt( pos() );
    if( !screen )
        screen = QApplication::primaryScreen();
    QRect    clientSize = screen->availableGeometry();
    QPoint   pos        = this->pos();
    QSize    size       = GetWindowSize();

    qDebug() << "ensureWindowIsOnScreen: clientArea (" << clientSize.x() << "," << clientSize.y() << ") w" << clientSize.width() << "h" << clientSize.height();

    if( pos.y() < clientSize.y() )
    {
        qDebug() << "ensureWindowIsOnScreen: y pos" << pos.y() << "below minimum, setting to" << clientSize.y();
        pos.setY( clientSize.y() );
    }

    if( pos.x() < clientSize.x() )
    {
        qDebug() << "ensureWindowIsOnScreen: x pos" << pos.x() << "is off the client rect, setting to" << clientSize.x();
        pos.setX( clientSize.x() );
    }

    if( pos.x() + size.width() - clientSize.x() > clientSize.width() )
    {
        int newWidth = clientSize.width() - ( pos.x() - clientSize.x() );
        qDebug() << "ensureWindowIsOnScreen: effective width" << (pos.x() + size.width()) << "above available" << clientSize.width() << ", setting to" << newWidth;
        size.setWidth( newWidth );
    }

    if( pos.y() + size.height() - clientSize.y() > clientSize.height() )
    {
        int newHeight = clientSize.height() - ( pos.y() - clientSize.y() );
        qDebug() << "ensureWindowIsOnScreen: effective height" << (pos.y() + size.height()) << "above available" << clientSize.height() << ", setting to" << newHeight;
        size.setHeight( newHeight );
    }

    qDebug() << "Updating window position (" << pos.x() << "," << pos.y() << ") with size (" << size.width() << "," << size.height() << ")";

    setGeometry( pos.x(), pos.y(), size.width(), size.height() );
}


void EDA_BASE_FRAME::LoadWindowSettings( const WINDOW_SETTINGS* aCfg )
{
    LoadWindowState( aCfg->state );

    m_perspective = aCfg->perspective;
    m_mruPath = aCfg->mru_path;

    TOOLS_HOLDER::CommonSettingsChanged();
}


void EDA_BASE_FRAME::SaveWindowSettings( WINDOW_SETTINGS* aCfg )
{
    if( isMinimized() )
        return;

    // If the window is maximized, we use the saved window size from before it was maximized
    if( isMaximized() )
    {
        m_framePos  = m_normalFramePos;
        m_frameSize = m_normalFrameSize;
    }
    else
    {
        m_frameSize = GetWindowSize();
        m_framePos  = pos();
    }

    aCfg->state.pos_x     = m_framePos.x();
    aCfg->state.pos_y     = m_framePos.y();
    aCfg->state.size_x    = m_frameSize.x;
    aCfg->state.size_y    = m_frameSize.y;
    aCfg->state.maximized = isMaximized();
    aCfg->state.display   = QApplication::desktop()->screenNumber( this );

    qDebug() << "Saving window maximized:" << (isMaximized() ? "true" : "false");
    qDebug() << "Saving config position (" << m_framePos.x() << "," << m_framePos.y() << ") with size (" << m_frameSize.x << "," << m_frameSize.y << ")";

    // Qt docking system would replace wxAuiManager
    // Save perspective for Qt dock widgets
    aCfg->perspective = saveState().toStdString();

    aCfg->mru_path = m_mruPath;
}


void EDA_BASE_FRAME::LoadSettings( APP_SETTINGS_BASE* aCfg )
{
    LoadWindowSettings( GetWindowSettings( aCfg ) );

    // Get file history size from common settings
    int fileHistorySize = Pgm().GetCommonSettings()->m_System.file_history_size;

    // Load the recently used files into the history menu
    m_fileHistory = new FILE_HISTORY( (unsigned) std::max( 1, fileHistorySize ),
                                      ID_FILE1, ID_FILE_LIST_CLEAR );
    m_fileHistory->Load( *aCfg );
}


void EDA_BASE_FRAME::SaveSettings( APP_SETTINGS_BASE* aCfg )
{
    Q_ASSERT( config() );

    SaveWindowSettings( GetWindowSettings( aCfg ) );

    bool fileOpen = m_isClosing && m_isNonUserClose;

    QString currentlyOpenedFile = GetCurrentFileName();

    if( Pgm().GetCommonSettings()->m_Session.remember_open_files && !currentlyOpenedFile.isEmpty() )
    {
        QFileInfo rfn( currentlyOpenedFile );
        QString relativePath = QDir( Prj().GetProjectPath() ).relativeFilePath( rfn.absoluteFilePath() );
        Prj().GetLocalSettings().SaveFileState( relativePath, &aCfg->m_Window, fileOpen );
    }

    // Save the recently used files list
    if( m_fileHistory )
    {
        // Save the currently opened file in the file history
        if( !currentlyOpenedFile.isEmpty() )
            UpdateFileHistory( currentlyOpenedFile );

        m_fileHistory->Save( *aCfg );
    }
}


WINDOW_SETTINGS* EDA_BASE_FRAME::GetWindowSettings( APP_SETTINGS_BASE* aCfg )
{
    return &aCfg->m_Window;
}


APP_SETTINGS_BASE* EDA_BASE_FRAME::config() const
{
    // KICAD_MANAGER_FRAME overrides this
    return Kiface().KifaceSettings();
}


const SEARCH_STACK& EDA_BASE_FRAME::sys_search()
{
    return Kiface().KifaceSearch();
}


QString EDA_BASE_FRAME::help_name()
{
    return Kiface().GetHelpFileName();
}


void EDA_BASE_FRAME::PrintMsg( const QString& text )
{
    statusBar()->showMessage( text );
}


void EDA_BASE_FRAME::CreateInfoBar()
{
    // UI component - commented out for minimal build
    // WX_INFOBAR widget implementation removed
    m_infoBar = nullptr;  // Set to null to avoid crashes
}


void EDA_BASE_FRAME::FinishAUIInitialization()
{
#if defined( Q_OS_MACOS )
    m_auimgr.Update();
#else
    // Call Update() to fix all pane default sizes, especially the "InfoBar" pane before
    // hiding it.
    m_auimgr.Update();

    // We don't want the infobar displayed right away
    m_auimgr.GetPane( "InfoBar" ).Hide();
    m_auimgr.Update();
#endif
}


void EDA_BASE_FRAME::ShowInfoBarError( const QString& aErrorMsg, bool aShowCloseButton,
                                       int aType )
{
    // UI component - commented out for minimal build
    // InfoBar widget implementation removed, use status bar instead
    statusBar()->showMessage( aErrorMsg );
}


void EDA_BASE_FRAME::ShowInfoBarError( const QString& aErrorMsg, bool aShowCloseButton,
                                       std::function<void(void)> aCallback )
{
    // UI component - commented out for minimal build
    // InfoBar widget implementation removed, use status bar instead
    statusBar()->showMessage( aErrorMsg );
}


void EDA_BASE_FRAME::ShowInfoBarWarning( const QString& aWarningMsg, bool aShowCloseButton )
{
    // UI component - commented out for minimal build
    // InfoBar widget implementation removed, use status bar instead
    statusBar()->showMessage( aWarningMsg );
}


void EDA_BASE_FRAME::ShowInfoBarMsg( const QString& aMsg, bool aShowCloseButton )
{
    // UI component - commented out for minimal build
    // InfoBar widget implementation removed, use status bar instead
    statusBar()->showMessage( aMsg );
}


void EDA_BASE_FRAME::UpdateFileHistory( const QString& FullFileName, FILE_HISTORY* aFileHistory )
{
    if( !aFileHistory )
        aFileHistory = m_fileHistory;

    Q_ASSERT( aFileHistory );

    aFileHistory->AddFileToHistory( FullFileName );

    // Update the menubar to update the file history menu
    if( !m_isClosing && menuBar() )
    {
        ReCreateMenuBar();
        menuBar()->update();
    }
}


QString EDA_BASE_FRAME::GetFileFromHistory( int cmdId, const QString& type,
                                             FILE_HISTORY* aFileHistory )
{
    if( !aFileHistory )
        aFileHistory = m_fileHistory;

    Q_ASSERT( aFileHistory );

    int baseId = aFileHistory->GetBaseId();

    Q_ASSERT( cmdId >= baseId && cmdId < baseId + (int) aFileHistory->size() );

    unsigned i = cmdId - baseId;

    if( i < aFileHistory->size() )
    {
        QString fn = aFileHistory->GetHistoryFile( i );

        if( QFileInfo::exists( fn ) )
        {
            return fn;
        }
        else
        {
            DisplayErrorMessage( this, QString( _( "File '%1' was not found." ) ).arg( fn ) );
            aFileHistory->RemoveFileFromHistory( i );
        }
    }

    // Update the menubar to update the file history menu
    if( menuBar() )
    {
        ReCreateMenuBar();
        menuBar()->update();
    }

    return QString();
}


void EDA_BASE_FRAME::ClearFileHistory( FILE_HISTORY* aFileHistory )
{
    if( !aFileHistory )
        aFileHistory = m_fileHistory;

    Q_ASSERT( aFileHistory );

    aFileHistory->ClearFileHistory();

    // Update the menubar to update the file history menu
    if( menuBar() )
    {
        ReCreateMenuBar();
        menuBar()->update();
    }
}


void EDA_BASE_FRAME::OnKicadAbout()
{
    // Minimal implementation - just show a simple message
    QMessageBox::information( this, _( "About KiCad" ),
                              _( "KiCad PCB minimal build - About dialog disabled" ) );
}


void EDA_BASE_FRAME::OnPreferences()
{
    ShowPreferences( QString(), QString() );
}


void EDA_BASE_FRAME::ShowPreferences( QString aStartPage, QString aStartParentPage )
{
    // Minimal implementation - just show a simple message
    QMessageBox::information( this, _( "Preferences" ),
                              _( "KiCad PCB minimal build - Preferences dialog disabled.\n"
                                 "UI components have been removed for minimal compilation." ) );
}


void EDA_BASE_FRAME::OnDropFiles( QDropEvent& aEvent )
{
    const QMimeData* mimeData = aEvent.mimeData();
    if( !mimeData->hasUrls() )
        return;

    for( const QUrl& url : mimeData->urls() )
    {
        if( !url.isLocalFile() )
            continue;
            
        const QFileInfo fn( url.toLocalFile() );
        QString         ext = fn.suffix();

        // Alias all gerber files as GerberFileExtension
        if( FILEEXT::IsGerberFileExtension( ext ) )
            ext = FILEEXT::GerberFileExtension;

        if( m_acceptedExts.find( ext.toStdString() ) != m_acceptedExts.end() )
            m_AcceptedFiles.emplace_back( fn.absoluteFilePath() );
    }

    DoWithAcceptedFiles();
    m_AcceptedFiles.clear();
}


void EDA_BASE_FRAME::DoWithAcceptedFiles()
{
    for( const QString& file : m_AcceptedFiles )
    {
        QFileInfo fi( file );
        QString fn = file;
        m_toolManager->RunAction<QString*>( *m_acceptedExts.at( fi.suffix().toStdString() ), &fn );
    }
}


bool EDA_BASE_FRAME::IsWritable( const QFileInfo& aFileName, bool aVerbose )
{
    QString msg;
    QFileInfo fn = aFileName;

    // Check for absence of a file path with a file name.
    // Make the file name absolute if it's relative
    if( fn.isRelative() && !fn.fileName().isEmpty() )
        fn = QFileInfo( QDir::current().absoluteFilePath( fn.filePath() ) );

    Q_ASSERT_X( fn.exists() || !fn.dir().path().isEmpty(), "EDA_BASE_FRAME::IsWritable",
                "File name object is invalid. Bad programmer!" );
    Q_ASSERT_X( !fn.dir().path().isEmpty(), "EDA_BASE_FRAME::IsWritable",
                qPrintable( QString( "File name object path <%1> is not set. Bad programmer!" ).arg( fn.absoluteFilePath() ) ) );

    if( fn.isDir() && !QFileInfo( fn.absoluteFilePath() ).isWritable() )
    {
        msg = QString( _( "Insufficient permissions to folder '%1'." ) ).arg( fn.absolutePath() );
    }
    else if( !fn.exists() && !QFileInfo( fn.absolutePath() ).isWritable() )
    {
        msg = QString( _( "Insufficient permissions to save file '%1'." ) ).arg( fn.absoluteFilePath() );
    }
    else if( fn.exists() && !fn.isWritable() )
    {
        msg = QString( _( "Insufficient permissions to save file '%1'." ) ).arg( fn.absoluteFilePath() );
    }

    if( !msg.isEmpty() )
    {
        if( aVerbose )
            DisplayErrorMessage( this, msg );

        return false;
    }

    return true;
}


void EDA_BASE_FRAME::CheckForAutoSaveFile( const QFileInfo& aFileName )
{
    if( !Pgm().IsGUI() )
        return;

    Q_ASSERT_X( !aFileName.fileName().isEmpty(), "EDA_BASE_FRAME::CheckForAutoSaveFile", "Invalid file name!" );

    QFileInfo autoSaveFileName = aFileName;

    // Check for auto save file.
    QString autoSavePath = aFileName.absolutePath() + "/" + 
                           FILEEXT::AutoSaveFilePrefix + aFileName.baseName() + "." + aFileName.completeSuffix();
    autoSaveFileName = QFileInfo( autoSavePath );

    qDebug() << "Checking for auto save file" << autoSaveFileName.absoluteFilePath();

    if( !autoSaveFileName.exists() )
        return;

    QString msg = QString( _( "Well this is potentially embarrassing!\n"
                              "It appears that the last time you were editing\n"
                              "%1\n"
                              "KiCad exited before saving.\n"
                              "\n"
                              "Do you wish to open the auto-saved file instead?" ) )
                  .arg( aFileName.fileName() );

    int response = QMessageBox::question( this, Pgm().App().GetAppDisplayName(), msg,
                                          QMessageBox::Yes | QMessageBox::No );

    // Make a backup of the current file, delete the file, and rename the auto save file to
    // the file name.
    if( response == QMessageBox::Yes )
    {
        // Preserve the permissions of the current file
        KIPLATFORM::IO::DuplicatePermissions( aFileName.absoluteFilePath(),
                                              autoSaveFileName.absoluteFilePath() );

        if( !QFile::rename( autoSaveFileName.absoluteFilePath(), aFileName.absoluteFilePath() ) )
        {
            QMessageBox::warning( this, Pgm().App().GetAppDisplayName(),
                                   _( "The auto save file could not be renamed to the board file name." ) );
        }
    }
    else
    {
        DeleteAutoSaveFile( aFileName );
    }
}


void EDA_BASE_FRAME::DeleteAutoSaveFile( const QFileInfo& aFileName )
{
    if( !Pgm().IsGUI() )
        return;

    Q_ASSERT_X( !aFileName.fileName().isEmpty(), "EDA_BASE_FRAME::DeleteAutoSaveFile", "Invalid file name!" );

    QString autoSavePath = aFileName.absolutePath() + "/" + 
                           FILEEXT::AutoSaveFilePrefix + aFileName.baseName() + "." + aFileName.completeSuffix();
    QFileInfo autoSaveFn( autoSavePath );

    if( autoSaveFn.exists() )
    {
        qDebug() << "Removing auto save file" << autoSaveFn.absoluteFilePath();
        QFile::remove( autoSaveFn.absoluteFilePath() );
    }
}


bool EDA_BASE_FRAME::IsContentModified() const
{
    // This function should be overridden in child classes
    return false;
}


void EDA_BASE_FRAME::initExitKey()
{
    QShortcut* exitShortcut = new QShortcut( QKeySequence::Quit, this );
    connect( exitShortcut, &QShortcut::activated, this, &QWidget::close );
}


void EDA_BASE_FRAME::ClearUndoRedoList()
{
    ClearUndoORRedoList( UNDO_LIST );
    ClearUndoORRedoList( REDO_LIST );
}


void EDA_BASE_FRAME::PushCommandToUndoList( PICKED_ITEMS_LIST* aNewitem )
{
    m_undoList.PushCommand( aNewitem );

    // Delete the extra items, if count max reached
    if( m_undoRedoCountMax > 0 )
    {
        int extraitems = GetUndoCommandCount() - m_undoRedoCountMax;

        if( extraitems > 0 )
            ClearUndoORRedoList( UNDO_LIST, extraitems );
    }
}


void EDA_BASE_FRAME::PushCommandToRedoList( PICKED_ITEMS_LIST* aNewitem )
{
    m_redoList.PushCommand( aNewitem );

    // Delete the extra items, if count max reached
    if( m_undoRedoCountMax > 0 )
    {
        int extraitems = GetRedoCommandCount() - m_undoRedoCountMax;

        if( extraitems > 0 )
            ClearUndoORRedoList( REDO_LIST, extraitems );
    }
}


PICKED_ITEMS_LIST* EDA_BASE_FRAME::PopCommandFromUndoList( )
{
    return m_undoList.PopCommand();
}


PICKED_ITEMS_LIST* EDA_BASE_FRAME::PopCommandFromRedoList( )
{
    return m_redoList.PopCommand();
}


QString EDA_BASE_FRAME::GetUndoActionDescription() const
{
    if( GetUndoCommandCount() > 0 )
        return m_undoList.m_CommandsList.back()->GetDescription();

    return QString();
}


QString EDA_BASE_FRAME::GetRedoActionDescription() const
{
    if( GetRedoCommandCount() > 0 )
        return m_redoList.m_CommandsList.back()->GetDescription();

    return QString();
}


void EDA_BASE_FRAME::OnModify()
{
    m_autoSaveRequired = true;
}


void EDA_BASE_FRAME::ChangeUserUnits( EDA_UNITS aUnits )
{
    SetUserUnits( aUnits );
    unitsChangeRefresh();

    // Qt signal/slot mechanism would be used instead of wxCommandEvent
    // Signal emission would be implemented once Q_OBJECT is added to header
}


void EDA_BASE_FRAME::OnMaximize( QEvent& aEvent )
{
    // When we maximize the window, we want to save the old information
    // so that we can add it to the settings on next window load.
#ifdef Q_OS_MACOS
    if( !isMaximized() )
#endif
    {
        m_normalFrameSize = GetWindowSize();
        m_normalFramePos  = pos();
        qDebug() << "Maximizing window - Saving position (" << m_normalFramePos.x() << "," << m_normalFramePos.y() << ") with size (" << m_normalFrameSize.x << "," << m_normalFrameSize.y << ")";
    }

    // Qt handles the maximize event automatically
}


QSize EDA_BASE_FRAME::GetWindowSize()
{
    // Qt automatically handles window decorations across platforms
    return size();
}


void EDA_BASE_FRAME::HandleSystemColorChange()
{
    // Update the icon theme when the system theme changes and update the toolbars
    GetBitmapStore()->ThemeChanged();
    ThemeChanged();

    // This isn't handled by ThemeChanged()
    if( menuBar() )
    {
        // For icons in menus, icon scaling & hotkeys
        ReCreateMenuBar();
        menuBar()->update();
    }
}


void EDA_BASE_FRAME::onSystemColorChange()
{
    // Call the handler to update the colors used in the frame
    HandleSystemColorChange();

    // Qt automatically propagates color changes to child widgets
}


void EDA_BASE_FRAME::onIconize( QEvent& aEvent )
{
    // Call the handler
    handleIconizeEvent( aEvent );

    // Qt handles iconize events automatically
}


#ifdef Q_OS_WIN
// Qt handles Windows-specific message processing internally
// Custom message handling would be done through nativeEvent() if needed
#endif


void EDA_BASE_FRAME::AddMenuLanguageList( ACTION_MENU* aMasterMenu, TOOL_INTERACTIVE* aControlTool )
{
    ACTION_MENU* langsMenu = new ACTION_MENU( false, aControlTool );
    langsMenu->SetTitle( _( "Set Language" ) );
    langsMenu->SetIcon( BITMAPS::language );

    QString tooltip;

    for( unsigned ii = 0; LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
    {
        QString label;

        if( LanguagesList[ii].m_DoNotTranslate )
            label = LanguagesList[ii].m_Lang_Label;
        else
            label = QCoreApplication::translate( "LanguagesList", LanguagesList[ii].m_Lang_Label );

        QAction* item = new QAction( label, langsMenu );
        item->setCheckable( true );
        item->setData( LanguagesList[ii].m_KI_Lang_Identifier );

        langsMenu->addAction( item );
    }

    // This must be done after the items are added
    aMasterMenu->Add( langsMenu );
}