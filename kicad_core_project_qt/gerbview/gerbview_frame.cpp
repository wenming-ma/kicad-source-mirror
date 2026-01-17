
#include <kiface_base.h>
#include <base_units.h>
#include <pgm_base.h>
#include <bitmaps.h>
#include <wildcards_and_files_ext.h>
#include <gal/graphics_abstraction_layer.h>
#include <gerbview_frame.h>
#include <gerbview_id.h>
#include <gerber_file_image.h>
#include <gerber_file_image_list.h>
#include <excellon_image.h>
#include <gerbview_draw_panel_gal.h>
#include <gerbview_settings.h>
#include <drawing_sheet/ds_proxy_view_item.h>
#include <lset.h>
#include <settings/settings_manager.h>
#include <tool/tool_manager.h>
#include <tool/action_toolbar.h>
#include <tool/tool_dispatcher.h>
#include <tool/common_control.h>
#include <tool/common_tools.h>
#include <tool/editor_conditions.h>
#include <tool/zoom_tool.h>
#include <tools/gerbview_actions.h>
#include <tools/gerbview_inspection_tool.h>
#include <tools/gerbview_selection.h>
#include <tools/gerbview_selection_tool.h>
#include <tools/gerbview_control.h>
#include <trigo.h>
#include <view/view.h>
#include <view/view_controls.h>
#include <base_screen.h>
#include <gerbview_painter.h>
// Qt Transformation Completed - File converted from wxWidgets to Qt framework
#include <QtWidgets/QWidget>
#include <QtWidgets/QApplication>
#include <QtGui/QIcon>
#include <QtGui/QResizeEvent>
#include <QtCore/QFileInfo>
#include <QtCore/QString>
#include <QtCore/QTimer>

#include "widgets/gbr_layer_box_selector.h"
#include "widgets/gerbview_layer_widget.h"
#include "widgets/dcode_selection_box.h"
#include <dialog_draw_layers_settings.h>

#include <navlib/nl_gerbview_plugin.h>

GERBVIEW_FRAME::GERBVIEW_FRAME( KIWAY* aKiway, QWidget* aParent ) :
        EDA_DRAW_FRAME( aKiway, aParent, FRAME_GERBER, "GerbView", QPoint(),
                        QSize(), KICAD_DEFAULT_DRAWFRAME_STYLE, GERBVIEW_FRAME_NAME,
                        gerbIUScale ),
        m_TextInfo( nullptr ),
        m_zipFileHistory( DEFAULT_FILE_HISTORY_SIZE, ID_GERBVIEW_ZIP_FILE1,
                          ID_GERBVIEW_ZIP_FILE_LIST_CLEAR, "Clear Recent Zip Files" ),
        m_drillFileHistory( DEFAULT_FILE_HISTORY_SIZE, ID_GERBVIEW_DRILL_FILE1,
                            ID_GERBVIEW_DRILL_FILE_LIST_CLEAR, "Clear Recent Drill Files" ),
        m_jobFileHistory( DEFAULT_FILE_HISTORY_SIZE, ID_GERBVIEW_JOB_FILE1,
                          ID_GERBVIEW_JOB_FILE_LIST_CLEAR, "Clear Recent Job Files" ),
        m_activeLayer( 0 )
{
    m_maximizeByDefault = true;
    m_gerberLayout = nullptr;
    m_show_layer_manager_tools = true;
    m_showBorderAndTitleBlock = false;      // true for reference drawings.
    m_SelLayerBox = nullptr;
    m_DCodeSelector = nullptr;
    m_SelComponentBox = nullptr;
    m_SelNetnameBox = nullptr;
    m_SelAperAttributesBox = nullptr;
    m_cmpText = nullptr;
    m_netText = nullptr;
    m_apertText = nullptr;
    m_dcodeText = nullptr;
    m_aboutTitle = "KiCad Gerber Viewer";

    SHAPE_POLY_SET dummy;   // A ugly trick to force the linker to include
                            // some methods in code and avoid link errors

    int fileHistorySize = Pgm().GetCommonSettings()->m_System.file_history_size;
    m_drillFileHistory.SetMaxFiles( fileHistorySize );
    m_zipFileHistory.SetMaxFiles( fileHistorySize );
    m_jobFileHistory.SetMaxFiles( fileHistorySize );

    auto* galCanvas = new GERBVIEW_DRAW_PANEL_GAL( this, -1, QPoint( 0, 0 ), m_frameSize,
                                                   GetGalDisplayOptions(),
                                                   EDA_DRAW_PANEL_GAL::GAL_TYPE_NONE );

    SetCanvas( galCanvas );

    // GerbView requires draw priority for rendering negative objects
    galCanvas->GetView()->UseDrawPriority( true );

    // Give an icon
    QIcon icon;
    icon.addPixmap( KiBitmap( BITMAPS::icon_gerbview, 48 ) );
    icon.addPixmap( KiBitmap( BITMAPS::icon_gerbview, 128 ) );
    icon.addPixmap( KiBitmap( BITMAPS::icon_gerbview, 256 ) );
    icon.addPixmap( KiBitmap( BITMAPS::icon_gerbview_32 ) );
    icon.addPixmap( KiBitmap( BITMAPS::icon_gerbview_16 ) );

    setWindowIcon( icon );

    // Be sure a page info is set. this default value will be overwritten later.
    PAGE_INFO pageInfo( "GERBER" );
    SetLayout( new GBR_LAYOUT() );
    SetPageSettings( pageInfo );

    SetVisibleLayers( LSET::AllLayersMask() );         // All draw layers visible.

    SetScreen( new BASE_SCREEN( GetPageSettings().GetSizeIU( gerbIUScale.IU_PER_MILS ) ) );

    // Create the PCB_LAYER_WIDGET *after* SetLayout():
    m_LayersManager = new GERBER_LAYER_WIDGET( this, GetCanvas() );

    // Update the minimum string length in the layer panel with the length of the last default layer
    QString lyrName = GetImagesList()->GetDisplayName( GetImagesList()->ImagesMaxCount(),
                                                        false, true );
    m_LayersManager->SetSmallestLayerString( lyrName );

    // LoadSettings() *after* creating m_LayersManager, because LoadSettings()
    // initialize parameters in m_LayersManager
    LoadSettings( config() );

    setupTools();
    setupUIConditions();
    ReCreateMenuBar();
    ReCreateHToolbar();
    ReCreateOptToolbar();
    ReCreateAuxiliaryToolbar();

    m_auimgr.SetManagedWindow( this );

    m_auimgr.AddPane( m_mainToolBar, EDA_PANE().HToolbar().Name( "MainToolbar" ).Top().Layer( 6 ) );
    m_auimgr.AddPane( m_auxiliaryToolBar, EDA_PANE().HToolbar().Name( "AuxToolbar" ).Top()
                      .Layer(4) );
    m_auimgr.AddPane( m_messagePanel, EDA_PANE().Messages().Name( "MsgPanel" ).Bottom()
                      .Layer( 6 ) );
    m_auimgr.AddPane( m_optionsToolBar, EDA_PANE().VToolbar().Name( "OptToolbar" ).Left()
                      .Layer( 3 ) );
    m_auimgr.AddPane( m_LayersManager, EDA_PANE().Palette().Name( "LayersManager" ).Right()
                      .Layer( 3 ).Caption( "Layers Manager" ).PaneBorder( false )
                      .MinSize( 80, -1 ).BestSize( m_LayersManager->sizeHint() ) );

    m_auimgr.AddPane( GetCanvas(), EDA_PANE().Canvas().Name( "DrawFrame" ).Center() );

    ReFillLayerWidget();                // this is near end because contents establish size
    m_auimgr.Update();

    SetActiveLayer( 0, true );
    GetToolManager()->PostAction( ACTIONS::zoomFitScreen );

    resolveCanvasType();

    SwitchCanvas( m_canvasType );

    setupUnits( config() );

    // Enable the axes to match legacy draw style
    auto& galOptions = GetGalDisplayOptions();
    galOptions.m_axesEnabled = true;
    galOptions.NotifyChanged();

    m_LayersManager->ReFill();
    m_LayersManager->ReFillRender();    // Update colors in Render after the config is read

    // Drag and drop
    // Note that all gerber files are aliased as GerberFileExtension
    m_acceptedExts.emplace( QString::fromStdString(FILEEXT::GerberFileExtension), &GERBVIEW_ACTIONS::loadGerbFiles );
    m_acceptedExts.emplace( QString::fromStdString(FILEEXT::ArchiveFileExtension), &GERBVIEW_ACTIONS::loadZipFile );
    m_acceptedExts.emplace( QString::fromStdString(FILEEXT::DrillFileExtension), &GERBVIEW_ACTIONS::loadGerbFiles );
    setAcceptDrops( true );

    GetToolManager()->RunAction( ACTIONS::zoomFitScreen );

    // Ensure the window is on top
    raise();
    activateWindow();

    // Register a call to update the toolbar sizes. It can't be done immediately because
    // it seems to require some sizes calculated that aren't yet (at least on GTK).
    QTimer::singleShot(0, this, [this]()
               {
                   // Ensure the controls on the toolbars all are correctly sized
                    UpdateToolbarControlSizes();
               } );
}


GERBVIEW_FRAME::~GERBVIEW_FRAME()
{
    // Ensure m_canvasType is up to date, to save it in config
    m_canvasType = GetCanvas()->GetBackend();

    // Shutdown all running tools
    if( m_toolManager )
        m_toolManager->ShutdownAllTools();

    GetCanvas()->GetView()->Clear();

    GetGerberLayout()->GetImagesList()->DeleteAllImages();
    delete m_gerberLayout;
}


void GERBVIEW_FRAME::doCloseWindow()
{
    // No more vetos
    m_isClosing = true;
    GetCanvas()->StopDrawing();

    if( m_toolManager )
        m_toolManager->DeactivateTool();

    // Be sure any OpenGL event cannot be fired after frame deletion:
    GetCanvas()->setEnabled( false );

    Destroy();
}


bool GERBVIEW_FRAME::OpenProjectFiles( const std::vector<QString>& aFileSet, int aCtl )
{
    // Ensure the frame is shown when opening the file(s), to avoid issues (crash) on GAL
    // when trying to change the view if it is not fully initialized.
    // It happens when starting GerbView with a gerber job file to load
    if( !isVisible() )
        show();

    // The current project path is also a valid command parameter.  Check if a single path
    // rather than a file name was passed to GerbView and use it as the initial MRU path.
    if( aFileSet.size() > 0 )
    {
        QString path = aFileSet[0];

        // For some reason QApplication appears to leave the trailing double quote on quoted
        // parameters which are required for paths with spaces.  Maybe this should be
        // pushed back into PGM_SINGLE_TOP::OnPgmInit() but that may cause other issues.
        // We can't buy a break!
        if( path.right(1) == "\"" )
            path.chop(1);

        if( !QFileInfo::exists( path ) && QFileInfo( path ).isDir() )
        {
            m_mruPath = path;
            return true;
        }

        const unsigned limit = std::min( unsigned( aFileSet.size() ),
                                         unsigned( GERBER_DRAWLAYERS_COUNT ) );

        for( unsigned i = 0; i < limit; ++i )
        {
            QString ext = QFileInfo( aFileSet[i] ).suffix().toLower();

            if( ext == FILEEXT::ArchiveFileExtension )
                LoadZipArchiveFile( aFileSet[i] );
            else if( ext == FILEEXT::GerberJobFileExtension )
                LoadGerberJobFile( aFileSet[i] );
            else
            {
                GERBER_ORDER_ENUM fnameLayer;
                QString          fnameExtensionMatched;

                GERBER_FILE_IMAGE_LIST::GetGerberLayerFromFilename( aFileSet[i], fnameLayer,
                                                                    fnameExtensionMatched );

                switch( fnameLayer )
                {
                case GERBER_ORDER_ENUM::GERBER_DRILL:
                    LoadExcellonFiles( aFileSet[i] );
                    break;
                case GERBER_ORDER_ENUM::GERBER_LAYER_UNKNOWN:
                    LoadAutodetectedFiles( aFileSet[i] );
                    break;
                default:
                    LoadGerberFiles( aFileSet[i] );
                }
            }
        }
    }

    Zoom_Automatique( true );        // Zoom fit in frame

    return true;
}


GERBVIEW_SETTINGS* GERBVIEW_FRAME::gvconfig() const
{
    return dynamic_cast<GERBVIEW_SETTINGS*>( config() );
}


void GERBVIEW_FRAME::LoadSettings( APP_SETTINGS_BASE* aCfg )
{
    EDA_DRAW_FRAME::LoadSettings( aCfg );

    GERBVIEW_SETTINGS* cfg = dynamic_cast<GERBVIEW_SETTINGS*>( aCfg );
    Q_ASSERT( cfg );

    SetElementVisibility( LAYER_GERBVIEW_DRAWINGSHEET, cfg->m_Appearance.show_border_and_titleblock );
    SetElementVisibility( LAYER_GERBVIEW_PAGE_LIMITS, cfg->m_Display.m_DisplayPageLimits );

    PAGE_INFO pageInfo( "GERBER" );
    pageInfo.SetType( cfg->m_Appearance.page_type );
    SetPageSettings( pageInfo );

    SetElementVisibility( LAYER_DCODES, cfg->m_Appearance.show_dcodes );
    SetElementVisibility( LAYER_NEGATIVE_OBJECTS, cfg->m_Appearance.show_negative_objects );

    m_drillFileHistory.Load( cfg->m_DrillFileHistory );
    m_zipFileHistory.Load( cfg->m_ZipFileHistory );
    m_jobFileHistory.Load( cfg->m_JobFileHistory );
}


void GERBVIEW_FRAME::SaveSettings( APP_SETTINGS_BASE* aCfg )
{
    EDA_DRAW_FRAME::SaveSettings( aCfg );

    GERBVIEW_SETTINGS* cfg = dynamic_cast<GERBVIEW_SETTINGS*>( aCfg );
    Q_ASSERT( cfg );

    cfg->m_Appearance.page_type = GetPageSettings().GetType();

    m_drillFileHistory.Save( &cfg->m_DrillFileHistory );
    m_zipFileHistory.Save( &cfg->m_ZipFileHistory );
    m_jobFileHistory.Save( &cfg->m_JobFileHistory );

    COLOR_SETTINGS* cs = Pgm().GetSettingsManager().GetColorSettings();
    Pgm().GetSettingsManager().SaveColorSettings( cs, "gerbview" );
}


COLOR_SETTINGS* GERBVIEW_FRAME::GetColorSettings( bool aForceRefresh ) const
{
    SETTINGS_MANAGER&  mgr = Pgm().GetSettingsManager();
    GERBVIEW_SETTINGS* cfg = mgr.GetAppSettings<GERBVIEW_SETTINGS>( "gerbview" );
    QString currentTheme = cfg->m_ColorTheme;
    return mgr.GetColorSettings( currentTheme );
}


void GERBVIEW_FRAME::ReFillLayerWidget()
{
    m_LayersManager->setUpdatesEnabled( false );

    m_LayersManager->ReFill();
    m_SelLayerBox->Resync();
    ReCreateAuxiliaryToolbar();

    QAuiPaneInfo&  lyrs = m_auimgr.GetPane( m_LayersManager );
    QSize          bestz = m_LayersManager->sizeHint();
    bestz.setWidth( bestz.width() + 5 );   // gives a little margin

    lyrs.MinSize( bestz );
    lyrs.BestSize( bestz );
    lyrs.FloatingSize( bestz );

    if( lyrs.IsDocked() )
        m_auimgr.Update();
    else
        m_LayersManager->resize( bestz );

    m_LayersManager->setUpdatesEnabled( true );

    syncLayerWidget();
}


void GERBVIEW_FRAME::SetElementVisibility( int aLayerID, bool aNewState )
{
    KIGFX::VIEW* view = GetCanvas()->GetView();

    switch( aLayerID )
    {
    case LAYER_DCODES:
        gvconfig()->m_Appearance.show_dcodes = aNewState;

        for( int i = 0; i < GERBER_DRAWLAYERS_COUNT; i++ )
        {
            int layer = GERBER_DRAW_LAYER( i );
            int dcode_layer = GERBER_DCODE_LAYER( layer );
            view->SetLayerVisible( dcode_layer, aNewState && view->IsLayerVisible( layer ) );
        }

        break;

    case LAYER_NEGATIVE_OBJECTS:
    {
        gvconfig()->m_Appearance.show_negative_objects = aNewState;

        view->UpdateAllItemsConditionally( KIGFX::REPAINT,
                []( KIGFX::VIEW_ITEM* aItem )
                {
                    GERBER_DRAW_ITEM* item = dynamic_cast<GERBER_DRAW_ITEM*>( aItem );

                    // GetLayerPolarity() returns true for negative items
                    return ( item && item->GetLayerPolarity() );
                } );

        break;
    }

    case LAYER_GERBVIEW_DRAWINGSHEET:
        gvconfig()->m_Appearance.show_border_and_titleblock = aNewState;

        m_showBorderAndTitleBlock = gvconfig()->m_Appearance.show_border_and_titleblock;

        // NOTE: LAYER_DRAWINGSHEET always used for visibility, but the layer manager passes
        // LAYER_GERBVIEW_DRAWINGSHEET because of independent color control
        GetCanvas()->GetView()->SetLayerVisible( LAYER_DRAWINGSHEET, aNewState );
        break;

    case LAYER_GERBVIEW_GRID:
        SetGridVisibility( aNewState );
        break;

    case LAYER_GERBVIEW_PAGE_LIMITS:
        gvconfig()->m_Display.m_DisplayPageLimits = aNewState;
        SetPageSettings( GetPageSettings() );
        break;

    default:
        Q_ASSERT_X( false, "GERBVIEW_FRAME::SetElementVisibility",
                   QString::asprintf( "bad arg %d", aLayerID ).toStdString().c_str() );
    }

    ApplyDisplaySettingsToGAL();
    m_LayersManager->SetRenderState( aLayerID, aNewState );
}


void GERBVIEW_FRAME::ApplyDisplaySettingsToGAL()
{
    auto painter = static_cast<KIGFX::GERBVIEW_PAINTER*>( GetCanvas()->GetView()->GetPainter() );
    KIGFX::GERBVIEW_RENDER_SETTINGS* settings = painter->GetSettings();
    settings->SetHighContrast( gvconfig()->m_Display.m_HighContrastMode );
    settings->LoadColors( GetColorSettings() );

    GetCanvas()->GetView()->MarkTargetDirty( KIGFX::TARGET_NONCACHED );
}


int GERBVIEW_FRAME::getNextAvailableLayer() const
{
    for( int i = 0; i < (int) ImagesMaxCount(); ++i )
    {
        const GERBER_FILE_IMAGE* gerber = GetGbrImage( i );

        if( gerber == nullptr )    // this graphic layer is available: use it
            return i;
    }

    return NO_AVAILABLE_LAYERS;
}


void GERBVIEW_FRAME::syncLayerWidget()
{
    m_LayersManager->SelectLayer( GetActiveLayer() );
}


void GERBVIEW_FRAME::syncLayerBox( bool aRebuildLayerBox )
{
    if( aRebuildLayerBox )
        m_SelLayerBox->Resync();

    m_SelLayerBox->SetLayerSelection( GetActiveLayer() );

    int dcodeSelected = -1;
    GERBER_FILE_IMAGE*   gerber = GetGbrImage( GetActiveLayer() );

    if( gerber )
        dcodeSelected = gerber->m_Selected_Tool;

    if( m_DCodeSelector )
    {
        updateDCodeSelectBox();
        m_DCodeSelector->SetDCodeSelection( dcodeSelected );
        m_DCodeSelector->setEnabled( gerber != nullptr );
    }
}


void GERBVIEW_FRAME::SortLayersByFileExtension()
{
    RemapLayers( GetImagesList()->SortImagesByFileExtension() );
}


void GERBVIEW_FRAME::SortLayersByX2Attributes()
{
    RemapLayers( GetImagesList()->SortImagesByZOrder() );
}


void GERBVIEW_FRAME::RemapLayers( const std::unordered_map<int, int>& remapping )
{
    // Save the visibility of each existing gerber layer, in order to be able
    // to restore this visibility after layer reorder.
    // Note: the visibility of other objects (D_CODE, negative objects ... )
    // must be not modified
    for( int currlayer = GERBER_DRAWLAYERS_COUNT-1; currlayer >= 0; --currlayer )
    {
        GERBER_FILE_IMAGE* gerber = GetImagesList()->GetGbrImage( currlayer );

        if( gerber )
        {
            if( IsLayerVisible( currlayer ) )
                gerber->SetFlags( CANDIDATE );
            else
                gerber->ClearFlags( CANDIDATE );
        }

    }

    std::unordered_map<int, int> view_remapping;

    for( const std::pair<const int, int>& entry : remapping )
    {
        view_remapping[ GERBER_DRAW_LAYER( entry.first ) ] = GERBER_DRAW_LAYER( entry.second );
        view_remapping[ GERBER_DCODE_LAYER( entry.first ) ] = GERBER_DCODE_LAYER( entry.second );
    }

    GetCanvas()->GetView()->ReorderLayerData( view_remapping );

    // Restore visibility of gerber layers
    LSET newVisibility = GetVisibleLayers();

    for( int currlayer = GERBER_DRAWLAYERS_COUNT-1; currlayer >= 0; --currlayer )
    {
        GERBER_FILE_IMAGE* gerber = GetImagesList()->GetGbrImage( currlayer );

        if( gerber )
        {
            if( gerber->HasFlag( CANDIDATE ) )
                newVisibility.set( currlayer );
            else
                newVisibility.set( currlayer, false );

            gerber->ClearFlags( CANDIDATE );
        }
    }

    SetVisibleLayers( newVisibility );

    ReFillLayerWidget();
    syncLayerBox( true );

    // Reordering draw layers need updating the view items
    GetCanvas()->GetView()->RecacheAllItems();
    GetCanvas()->GetView()->MarkDirty();
    GetCanvas()->GetView()->UpdateAllItems( KIGFX::ALL );

    GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::SetLayerDrawPrms()
{
    // Adjust draw params: draw offset and draw rotation for a gerber file image
    GERBER_FILE_IMAGE* gerber = GetGbrImage( GetActiveLayer() );

    if( !gerber )
        return;

    DIALOG_DRAW_LAYERS_SETTINGS dlg( this );

    if( dlg.exec() != QDialog::Accepted )
        return;

    KIGFX::VIEW* view = GetCanvas()->GetView();

    view->RecacheAllItems();
    view->MarkDirty();
    view->UpdateAllItems( KIGFX::ALL );

    GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::UpdateXORLayers()
{
    auto target = GetCanvas()->GetBackend() == GERBVIEW_DRAW_PANEL_GAL::GAL_TYPE_OPENGL
                          ? KIGFX::TARGET_CACHED
                          : KIGFX::TARGET_NONCACHED;
    KIGFX::VIEW* view = GetCanvas()->GetView();

    int lastVisibleLayer = -1;

    for( int i = 0; i < GERBER_DRAWLAYERS_COUNT; i++ )
    {
        view->SetLayerDiff( GERBER_DRAW_LAYER( i ), gvconfig()->m_Display.m_XORMode );

        // Caching doesn't work with layered rendering of XOR'd layers
        if( gvconfig()->m_Display.m_XORMode )
            view->SetLayerTarget( GERBER_DRAW_LAYER( i ), KIGFX::TARGET_NONCACHED );
        else
            view->SetLayerTarget( GERBER_DRAW_LAYER( i ), target );

        // We want the last visible layer, but deprioritize the active layer unless it's the
        // only layer
        if( ( lastVisibleLayer == -1 )
            || ( view->IsLayerVisible( GERBER_DRAW_LAYER( i ) ) && i != GetActiveLayer() ) )
        {
            lastVisibleLayer = i;
        }
    }

    //We don't want to diff the last visible layer onto the background, etc.
    if( lastVisibleLayer != -1 )
    {
        view->SetLayerTarget( GERBER_DRAW_LAYER( lastVisibleLayer ), target );
        view->SetLayerDiff( GERBER_DRAW_LAYER( lastVisibleLayer ), false );
    }

    view->RecacheAllItems();
    view->MarkDirty();
    view->UpdateAllItems( KIGFX::ALL );
}


void GERBVIEW_FRAME::UpdateTitleAndInfo()
{
    GERBER_FILE_IMAGE* gerber = GetGbrImage( GetActiveLayer() );

    // Display the gerber filename
    if( gerber == nullptr )
    {
        setWindowTitle( "Gerber Viewer" );

        statusBar()->showMessage( QString() );

        QString info = "Drawing layer not in use";
        m_TextInfo->setText( info );

        if( KIUI::EnsureTextCtrlWidth( m_TextInfo, &info ) ) // Resized
           m_auimgr.Update();

        ClearMsgPanel();
        return;
    }
    else
    {
        QString   title;
        QFileInfo filename( gerber->m_FileName );

        title = filename.fileName();

        if( gerber->m_IsX2_file )
            title += " " + QString( "(with X2 attributes)" );

        title += " \u2014 " + QString( "Gerber Viewer" );
        setWindowTitle( title );

        gerber->DisplayImageInfo( this );

        // Display Image Name and Layer Name (from the current gerber data):
        QString status;
        status = QString::asprintf( "Image name: \"%s\"  Layer name: \"%s\"",
                                   gerber->m_ImageName.toStdString().c_str(),
                                   gerber->GetLayerParams().m_LayerName.toStdString().c_str() );
        statusBar()->showMessage( status );

        // Display data format like fmt in X3.4Y3.4 no LZ or fmt mm X2.3 Y3.5 no TZ in main toolbar
        QString info;
        info = QString::asprintf( "fmt: %s X%d.%d Y%d.%d no %cZ",
                                 gerber->m_GerbMetric ? "mm" : "in",
                                 gerber->m_FmtLen.x() - gerber->m_FmtScale.x(),
                                 gerber->m_FmtScale.x(),
                                 gerber->m_FmtLen.y() - gerber->m_FmtScale.y(),
                                 gerber->m_FmtScale.y(),
                                 gerber->m_NoTrailingZeros ? 'T' : 'L' );

        if( gerber->m_IsX2_file )
            info += " " + QString( "X2 attr" );

        m_TextInfo->setText( info );

        if( KIUI::EnsureTextCtrlWidth( m_TextInfo, &info ) ) // Resized
            m_auimgr.Update();
    }
}


bool GERBVIEW_FRAME::IsElementVisible( int aLayerID ) const
{
    switch( aLayerID )
    {
    case LAYER_DCODES:                return gvconfig()->m_Appearance.show_dcodes;
    case LAYER_NEGATIVE_OBJECTS:      return gvconfig()->m_Appearance.show_negative_objects;
    case LAYER_GERBVIEW_GRID:         return IsGridVisible();
    case LAYER_GERBVIEW_DRAWINGSHEET: return gvconfig()->m_Appearance.show_border_and_titleblock;
    case LAYER_GERBVIEW_PAGE_LIMITS:  return gvconfig()->m_Display.m_DisplayPageLimits;
    case LAYER_GERBVIEW_BACKGROUND:   return true;

    default:
        Q_ASSERT_X( false, "GERBVIEW_FRAME::IsElementVisible",
                   QString::asprintf( "bad arg %d", aLayerID ).toStdString().c_str() );
    }

    return true;
}


LSET GERBVIEW_FRAME::GetVisibleLayers() const
{
    LSET visible = LSET::AllLayersMask();

    if( GetCanvas() )
    {
        for( int i = 0; i < GERBER_DRAWLAYERS_COUNT; i++ )
            visible[i] = GetCanvas()->GetView()->IsLayerVisible( GERBER_DRAW_LAYER( i ) );
    }

    return visible;
}


void GERBVIEW_FRAME::SetVisibleLayers( const LSET& aLayerMask )
{
    if( GetCanvas() )
    {
        for( int i = 0; i < GERBER_DRAWLAYERS_COUNT; i++ )
        {
            bool v = aLayerMask[i];
            int layer = GERBER_DRAW_LAYER( i );
            GetCanvas()->GetView()->SetLayerVisible( layer, v );
            GetCanvas()->GetView()->SetLayerVisible( GERBER_DCODE_LAYER( layer ),
                                                     gvconfig()->m_Appearance.show_dcodes && v );
        }
    }
}


bool GERBVIEW_FRAME::IsLayerVisible( int aLayer ) const
{
    return m_LayersManager->IsLayerVisible( aLayer );
}


COLOR4D GERBVIEW_FRAME::GetVisibleElementColor( int aLayerID )
{
    COLOR4D color = COLOR4D::UNSPECIFIED;
    COLOR_SETTINGS* settings = GetColorSettings();

    switch( aLayerID )
    {
    case LAYER_NEGATIVE_OBJECTS:
    case LAYER_DCODES:
    case LAYER_GERBVIEW_DRAWINGSHEET:
    case LAYER_GERBVIEW_PAGE_LIMITS:
    case LAYER_GERBVIEW_BACKGROUND:
        color = settings->GetColor( aLayerID );
        break;

    case LAYER_GERBVIEW_GRID:
        color = GetGridColor();
        break;

    default:
        Q_ASSERT_X( false, "GERBVIEW_FRAME::GetVisibleElementColor",
                   QString::asprintf( "bad arg %d", aLayerID ).toStdString().c_str() );
    }

    return color;
}


void GERBVIEW_FRAME::SetGridVisibility( bool aVisible )
{
    EDA_DRAW_FRAME::SetGridVisibility( aVisible );
    m_LayersManager->SetRenderState( LAYER_GERBVIEW_GRID, aVisible );
}


void GERBVIEW_FRAME::SetVisibleElementColor( int aLayerID, const COLOR4D& aColor )
{
    COLOR_SETTINGS* settings = GetColorSettings();

    settings->SetColor( aLayerID, aColor );

    switch( aLayerID )
    {
    case LAYER_GERBVIEW_DRAWINGSHEET:
    case LAYER_GERBVIEW_PAGE_LIMITS:
        SetPageSettings( GetPageSettings() );
        break;

    case LAYER_GERBVIEW_GRID:
        SetGridColor( aColor );
        break;

    case LAYER_GERBVIEW_BACKGROUND:
        SetDrawBgColor( aColor );
        break;

    default:
        break;
    }
}


COLOR4D GERBVIEW_FRAME::GetLayerColor( int aLayer ) const
{
    return GetColorSettings()->GetColor( aLayer );
}


void GERBVIEW_FRAME::SetLayerColor( int aLayer, const COLOR4D& aColor )
{
    GetColorSettings()->SetColor( aLayer, aColor );
    ApplyDisplaySettingsToGAL();
}


void GERBVIEW_FRAME::SetActiveLayer( int aLayer, bool doLayerWidgetUpdate )
{
    m_activeLayer = aLayer;

    if( gvconfig()->m_Display.m_XORMode )
        UpdateXORLayers();

    if( doLayerWidgetUpdate )
    {
        m_LayersManager->SelectLayer( aLayer );
        m_LayersManager->OnLayerSelected();
    }

    UpdateTitleAndInfo();

    m_toolManager->PostAction( GERBVIEW_ACTIONS::layerChanged );       // notify other tools
    GetCanvas()->setFocus();                 // otherwise hotkeys are stuck somewhere

    GetCanvas()->SetHighContrastLayer( GERBER_DRAW_LAYER( aLayer ) );
    GetCanvas()->Refresh();
}


void GERBVIEW_FRAME::SetPageSettings( const PAGE_INFO& aPageSettings )
{
    m_paper = aPageSettings;

    if( GetScreen() )
        GetScreen()->InitDataPoints( aPageSettings.GetSizeIU( gerbIUScale.IU_PER_MILS ) );

    GERBVIEW_DRAW_PANEL_GAL* drawPanel = static_cast<GERBVIEW_DRAW_PANEL_GAL*>( GetCanvas() );

    // Prepare drawing-sheet template
    DS_PROXY_VIEW_ITEM* drawingSheet = new DS_PROXY_VIEW_ITEM( gerbIUScale, &GetPageSettings(),
                                                               &Prj(), &GetTitleBlock(), nullptr );

    if( GetScreen() )
    {
        drawingSheet->SetPageNumber( "1" );
        drawingSheet->SetSheetCount( 1 );
    }

    drawingSheet->SetColorLayer( LAYER_GERBVIEW_DRAWINGSHEET );
    drawingSheet->SetPageBorderColorLayer( LAYER_GERBVIEW_PAGE_LIMITS );

    // Draw panel takes ownership of the drawing-sheet
    drawPanel->SetDrawingSheet( drawingSheet );
}


const PAGE_INFO& GERBVIEW_FRAME::GetPageSettings() const
{
    return m_paper;
}


const VECTOR2I GERBVIEW_FRAME::GetPageSizeIU() const
{
    // this function is only needed because EDA_DRAW_FRAME is not compiled
    // with either -DPCBNEW or -DEESCHEMA, so the virtual is used to route
    // into an application specific source file.
    return GetPageSettings().GetSizeIU( gerbIUScale.IU_PER_MILS );
}


const TITLE_BLOCK& GERBVIEW_FRAME::GetTitleBlock() const
{
    Q_ASSERT( m_gerberLayout );
    return m_gerberLayout->GetTitleBlock();
}


void GERBVIEW_FRAME::SetTitleBlock( const TITLE_BLOCK& aTitleBlock )
{
    Q_ASSERT( m_gerberLayout );
    m_gerberLayout->SetTitleBlock( aTitleBlock );
}


COLOR4D GERBVIEW_FRAME::GetGridColor()
{
    return GetColorSettings()->GetColor( LAYER_GERBVIEW_GRID );
}


void GERBVIEW_FRAME::SetGridColor( const COLOR4D& aColor )
{
    GetColorSettings()->SetColor( LAYER_GERBVIEW_GRID, aColor );
    GetCanvas()->GetGAL()->SetGridColor( aColor );
    m_gridColor = aColor;
}


void GERBVIEW_FRAME::DisplayGridMsg()
{
    VECTOR2D gridSize = GetCanvas()->GetGAL()->GetGridSize();
    QString line;

    line = QString::asprintf( "grid X %s  Y %s",
                             MessageTextFromValue( gridSize.x, false ).toStdString().c_str(),
                             MessageTextFromValue( gridSize.y, false ).toStdString().c_str() );

    statusBar()->showMessage( line );
}


void GERBVIEW_FRAME::UpdateStatusBar()
{
    EDA_DRAW_FRAME::UpdateStatusBar();

    if( !GetScreen() )
        return;

    QString line;
    VECTOR2D cursorPos = GetCanvas()->GetViewControls()->GetCursorPosition();

    if( GetShowPolarCoords() )  // display relative polar coordinates
    {
        VECTOR2D  v = cursorPos - GetScreen()->m_LocalOrigin;
        EDA_ANGLE theta( VECTOR2D( v.x, -v.y ) );
        double    ro = hypot( v.x, v.y );

        line = QString::asprintf( "r %s  theta %s",
                                 MessageTextFromValue( ro, false ).toStdString().c_str(),
                                 MessageTextFromValue( theta, false ).toStdString().c_str() );

        statusBar()->showMessage( line );
    }

    // Display absolute coordinates:
    line = QString::asprintf( "X %s  Y %s",
                             MessageTextFromValue( cursorPos.x, false ).toStdString().c_str(),
                             MessageTextFromValue( cursorPos.y, false ).toStdString().c_str() );
    statusBar()->showMessage( line );

    if( !GetShowPolarCoords() )
    {
        // Display relative cartesian coordinates:
        double dXpos = cursorPos.x - GetScreen()->m_LocalOrigin.x;
        double dYpos = cursorPos.y - GetScreen()->m_LocalOrigin.y;

        line = QString::asprintf( "dx %s  dy %s  dist %s",
                                 MessageTextFromValue( dXpos, false ).toStdString().c_str(),
                                 MessageTextFromValue( dYpos, false ).toStdString().c_str(),
                                 MessageTextFromValue( hypot( dXpos, dYpos ), false ).toStdString().c_str() );
        statusBar()->showMessage( line );
    }

    DisplayGridMsg();
}


GERBER_FILE_IMAGE* GERBVIEW_FRAME::GetGbrImage( int aIdx ) const
{
    return m_gerberLayout->GetImagesList()->GetGbrImage( aIdx );
}


unsigned GERBVIEW_FRAME::ImagesMaxCount() const
{
    return m_gerberLayout->GetImagesList()->ImagesMaxCount();
}


void GERBVIEW_FRAME::unitsChangeRefresh()
{
    // Called on units change (see EDA_DRAW_FRAME)
    EDA_DRAW_FRAME::unitsChangeRefresh();
    updateDCodeSelectBox();
    UpdateGridSelectBox();
}


void GERBVIEW_FRAME::ActivateGalCanvas()
{
    EDA_DRAW_FRAME::ActivateGalCanvas();

    EDA_DRAW_PANEL_GAL* galCanvas = GetCanvas();

    if( m_toolManager )
    {
        m_toolManager->SetEnvironment( m_gerberLayout, GetCanvas()->GetView(),
                                       GetCanvas()->GetViewControls(), config(), this );
        m_toolManager->ResetTools( TOOL_BASE::GAL_SWITCH );
    }

    galCanvas->GetGAL()->SetGridColor( GetLayerColor( LAYER_GERBVIEW_GRID ) );

    SetPageSettings( GetPageSettings() );

    galCanvas->GetView()->RecacheAllItems();
    galCanvas->SetEventDispatcher( m_toolDispatcher );
    galCanvas->StartDrawing();

    m_LayersManager->ReFill();
    m_LayersManager->ReFillRender();

    ReCreateOptToolbar();
    ReCreateMenuBar();

    try
    {
        if( !m_spaceMouse )
            m_spaceMouse = std::make_unique<NL_GERBVIEW_PLUGIN>();

        m_spaceMouse->SetCanvas( galCanvas );
    }
    catch( const std::system_error& e )
    {
        // Log trace message for navigation library error
    }
}


void GERBVIEW_FRAME::setupTools()
{
    // Create the manager and dispatcher & route draw panel events to the dispatcher
    m_toolManager = new TOOL_MANAGER;
    m_toolManager->SetEnvironment( m_gerberLayout, GetCanvas()->GetView(),
                                   GetCanvas()->GetViewControls(), config(), this );
    m_actions = new GERBVIEW_ACTIONS();
    m_toolDispatcher = new TOOL_DISPATCHER( m_toolManager );

    // Register tools
    m_toolManager->RegisterTool( new COMMON_CONTROL );
    m_toolManager->RegisterTool( new COMMON_TOOLS );
    m_toolManager->RegisterTool( new GERBVIEW_SELECTION_TOOL );
    m_toolManager->RegisterTool( new GERBVIEW_CONTROL );
    m_toolManager->RegisterTool( new GERBVIEW_INSPECTION_TOOL );
    m_toolManager->RegisterTool( new ZOOM_TOOL );
    m_toolManager->InitTools();

    // Run the selection tool, it is supposed to be always active
    m_toolManager->InvokeTool( "gerbview.InteractiveSelection" );
}


void GERBVIEW_FRAME::setupUIConditions()
{
    EDA_DRAW_FRAME::setupUIConditions();

    ACTION_MANAGER*   mgr = m_toolManager->GetActionManager();
    EDITOR_CONDITIONS cond( this );

    Q_ASSERT( mgr );

#define ENABLE( x ) ACTION_CONDITIONS().Enable( x )
#define CHECK( x )  ACTION_CONDITIONS().Check( x )

    mgr->SetConditions( ACTIONS::zoomTool,      CHECK( cond.CurrentTool( ACTIONS::zoomTool ) ) );
    mgr->SetConditions( ACTIONS::selectionTool, CHECK( cond.CurrentTool( ACTIONS::selectionTool ) ) );
    mgr->SetConditions( ACTIONS::measureTool,   CHECK( cond.CurrentTool( ACTIONS::measureTool ) ) );

    mgr->SetConditions( ACTIONS::toggleGrid,        CHECK( cond.GridVisible() ) );
    mgr->SetConditions( ACTIONS::togglePolarCoords, CHECK( cond.PolarCoordinates() ) );
    mgr->SetConditions( ACTIONS::toggleCursorStyle, CHECK( cond.FullscreenCursor() ) );

    mgr->SetConditions( ACTIONS::millimetersUnits,  CHECK( cond.Units( EDA_UNITS::MM ) ) );
    mgr->SetConditions( ACTIONS::inchesUnits,       CHECK( cond.Units( EDA_UNITS::INCH ) ) );
    mgr->SetConditions( ACTIONS::milsUnits,         CHECK( cond.Units( EDA_UNITS::MILS ) ) );

    auto flashedDisplayOutlinesCond =
        [this] ( const SELECTION& )
        {
            return !gvconfig()->m_Display.m_DisplayFlashedItemsFill;
        };

    auto linesFillCond =
        [this] ( const SELECTION& )
        {
            return !gvconfig()->m_Display.m_DisplayLinesFill;
        };

    auto polygonsFilledCond =
        [this] ( const SELECTION& )
        {
            return !gvconfig()->m_Display.m_DisplayPolygonsFill;
        };

    auto negativeObjectsCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Appearance.show_negative_objects;
        };

    auto dcodeCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Appearance.show_dcodes;
        };

    auto forceOpacityModeCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Display.m_ForceOpacityMode;
        };

    auto xorModeCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Display.m_XORMode;
        };

    auto highContrastModeCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Display.m_HighContrastMode;
        };

    auto flipGerberCond =
        [this] ( const SELECTION& )
        {
            return gvconfig()->m_Display.m_FlipGerberView;
        };

    auto layersManagerShownCondition =
        [this] ( const SELECTION& aSel )
        {
            return m_show_layer_manager_tools;
        };

    mgr->SetConditions( GERBVIEW_ACTIONS::flashedDisplayOutlines,  CHECK( flashedDisplayOutlinesCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::linesDisplayOutlines,    CHECK( linesFillCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::polygonsDisplayOutlines, CHECK( polygonsFilledCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::negativeObjectDisplay,   CHECK( negativeObjectsCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::dcodeDisplay,            CHECK( dcodeCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::toggleForceOpacityMode,  CHECK( forceOpacityModeCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::toggleXORMode,           CHECK( xorModeCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::flipGerberView,          CHECK( flipGerberCond ) );
    mgr->SetConditions( ACTIONS::highContrastMode,                 CHECK( highContrastModeCond ) );
    mgr->SetConditions( GERBVIEW_ACTIONS::toggleLayerManager,      CHECK( layersManagerShownCondition ) );

#undef CHECK
#undef ENABLE
}


void GERBVIEW_FRAME::CommonSettingsChanged( int aFlags )
{
    EDA_DRAW_FRAME::CommonSettingsChanged( aFlags );

    // Update gal display options like cursor shape, grid options:
    SETTINGS_MANAGER&  mgr = Pgm().GetSettingsManager();
    GERBVIEW_SETTINGS* cfg = mgr.GetAppSettings<GERBVIEW_SETTINGS>( "gerbview" );

    GetGalDisplayOptions().ReadWindowSettings( cfg->m_Window );

    SetPageSettings( PAGE_INFO( gvconfig()->m_Appearance.page_type ) );

    UpdateXORLayers();

    SetElementVisibility( LAYER_DCODES, gvconfig()->m_Appearance.show_dcodes );

    GetCanvas()->GetView()->MarkTargetDirty( KIGFX::TARGET_NONCACHED );
    GetCanvas()->GetView()->UpdateAllItems( KIGFX::REPAINT );
    GetCanvas()->ForceRefresh();

    RecreateToolbars();
    ReFillLayerWidget();                // Update the layers list
    m_LayersManager->ReFillRender();    // Update colors in Render after the config is read

    updateGeometry();
    // Send resize event to update widget sizes
    QResizeEvent resizeEvent( size(), size() );
    QApplication::sendEvent( this, &resizeEvent );
}


SELECTION& GERBVIEW_FRAME::GetCurrentSelection()
{
    return m_toolManager->GetTool<GERBVIEW_SELECTION_TOOL>()->GetSelection();
}


void GERBVIEW_FRAME::ToggleLayerManager()
{
    m_show_layer_manager_tools = !m_show_layer_manager_tools;

    // show/hide auxiliary Vertical layers and visibility manager toolbar
    m_auimgr.GetPane( "LayersManager" ).Show( m_show_layer_manager_tools );
    m_auimgr.Update();
}

void GERBVIEW_FRAME::handleActivateEvent( QEvent& aEvent )
{
    EDA_DRAW_FRAME::handleActivateEvent(aEvent);

    if( m_spaceMouse )
        m_spaceMouse->SetFocus( true );
}

void GERBVIEW_FRAME::handleIconizeEvent( QEvent& aEvent )
{
    EDA_DRAW_FRAME::handleIconizeEvent(aEvent);

    if( m_spaceMouse )
        m_spaceMouse->SetFocus( false );
}
