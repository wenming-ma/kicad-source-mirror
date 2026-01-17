
#ifndef DRAW_FRAME_H_
#define DRAW_FRAME_H_

#include <api/api_plugin.h>
#include <eda_base_frame.h>
#include <eda_search_data.h>
#include <kiway_player.h>
#include <gal/gal_display_options.h>
#include <gal_display_options_common.h>
#include <gal/color4d.h>
#include <class_draw_panel_gal.h>
#include <kiid.h>
#include <hotkeys_basic.h>
#include <widgets/lib_tree.h>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtCore/QFileInfo>
#include <QtWidgets/QWidget>
#include <QtWidgets/QComboBox>
#include <QtNetwork/QTcpServer>
#include <QtCore/QEvent>
#include <QtGui/QResizeEvent>

class EDA_ITEM;
class QTcpServer;
class ACTION_TOOLBAR;
class GRID_HELPER;
class COLOR_SETTINGS;
class LOCKFILE;
class TOOL_MENU;
class APP_SETTINGS_BASE;
class QFindReplaceDialog;
// Map wxWidgets event types to Qt equivalents
using QCommandEvent = QEvent;      // For general command events (wxCommandEvent)
using QUpdateUIEvent = QEvent;     // For UI update events (wxUpdateUIEvent)
class QMoveEvent;
// Note: QActivateEvent doesn't exist in Qt, using QEvent for activation events
class SEARCH_PANE;
class HOTKEY_CYCLE_POPUP;
class PROPERTIES_PANEL;
class NET_INSPECTOR_PANEL;
enum class BITMAP_TYPE;

namespace KIGFX
{
    class GAL_DISPLAY_OPTIONS;
    class RENDER_SETTINGS;
}

using KIGFX::COLOR4D;
using KIGFX::RENDER_SETTINGS;

#define LIB_EDIT_FRAME_NAME           QStringLiteral( "LibeditFrame" )
#define LIB_VIEW_FRAME_NAME           QStringLiteral( "ViewlibFrame" )
#define SCH_EDIT_FRAME_NAME           QStringLiteral( "SchematicFrame" )
#define SYMBOL_CHOOSER_FRAME_NAME     QStringLiteral( "SymbolChooserFrame" )
#define PL_EDITOR_FRAME_NAME          QStringLiteral( "PlEditorFrame" )
#define FOOTPRINT_WIZARD_FRAME_NAME   QStringLiteral( "FootprintWizard" )
#define FOOTPRINT_CHOOSER_FRAME_NAME  QStringLiteral( "FootprintChooserFrame" )
#define FOOTPRINT_EDIT_FRAME_NAME     QStringLiteral( "ModEditFrame" )
#define FOOTPRINT_VIEWER_FRAME_NAME   QStringLiteral( "ModViewFrame" )
#define PCB_EDIT_FRAME_NAME           QStringLiteral( "PcbFrame" )


// The base class for create windows for drawing purpose.
// The Eeschema, Pcbnew and GerbView main windows are just a few examples of classes
// derived from EDA_DRAW_FRAME.
class EDA_DRAW_FRAME : public KIWAY_PLAYER
{
public:
    EDA_DRAW_FRAME( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType, const QString& aTitle,
                    const QPoint& aPos, const QSize& aSize, long aStyle,
                    const QString& aFrameName, const EDA_IU_SCALE& aIuScale );

    ~EDA_DRAW_FRAME();

    // Mark a schematic file as being in use.
    // Use ReleaseFile() to undo this.
    bool LockFile( const QString& aFileName );

    // Release the current file marked in use.  See m_file_checker.
    void ReleaseFile();

    // Toggle the scripting console visibility.
    void ScriptingConsoleEnableDisable();

    // Get the current visibility of the scripting console window.
    bool IsScriptingConsoleVisible();

    EDA_SEARCH_DATA& GetFindReplaceData() { return *m_findReplaceData; }
    QStringList& GetFindHistoryList() { return m_findStringHistoryList; }

    virtual void SetPageSettings( const PAGE_INFO& aPageSettings ) = 0;
    virtual const PAGE_INFO& GetPageSettings() const = 0;

    // Works off of GetPageSettings() to return the size of the paper page in
    // the internal units of this particular view.
    virtual const VECTOR2I GetPageSizeIU() const = 0;

    // For those frames that support polar coordinates.
    bool GetShowPolarCoords() const { return m_polarCoords; }
    void SetShowPolarCoords( bool aShow ) { m_polarCoords = aShow; }

    void ToggleUserUnits() override;

    // Get the pair or units in current use.
    // The primary unit is the main unit of the frame, and the secondary unit is the unit
    // of the other system that was used most recently.
    void GetUnitPair( EDA_UNITS& aPrimaryUnit, EDA_UNITS& aSecondaryUnits ) override;

    // Return the absolute coordinates of the origin of the snap grid.
    // This is treated as a relative offset and snapping will occur at multiples of the grid
    // size relative to this point.
    virtual const VECTOR2I& GetGridOrigin() const = 0;
    virtual void            SetGridOrigin( const VECTOR2I& aPosition ) = 0;

    virtual std::unique_ptr<GRID_HELPER> MakeGridHelper();

    // Return the nearest aGridSize location to aPosition.
    VECTOR2I GetNearestGridPosition( const VECTOR2I& aPosition ) const;

    // Return the nearest aGridSize / 2 location to aPosition.
    // This is useful when attempting for keep outer points on grid but
    // not the middle point.
    VECTOR2I GetNearestHalfGridPosition( const VECTOR2I& aPosition ) const;

    virtual const TITLE_BLOCK& GetTitleBlock() const = 0;
    virtual void SetTitleBlock( const TITLE_BLOCK& aTitleBlock ) = 0;

    // the background color of the draw canvas:
    // Virtual because some frames can have a specific way to get/set the bg color
    virtual COLOR4D GetDrawBgColor() const { return m_drawBgColor; }
    virtual void SetDrawBgColor( const COLOR4D& aColor) { m_drawBgColor= aColor ; }

    /// Returns a pointer to the active color theme settings
    virtual COLOR_SETTINGS* GetColorSettings( bool aForceRefresh = false ) const;

    bool LibraryFileBrowser( bool doOpen, QFileInfo& aFilename, const QString& wildcard,
                             const QString& ext, bool isDirectory = false, bool aIsGlobal = false,
                             const QString& aGlobalPath = QString() );

    void CommonSettingsChanged( int aFlags ) override;

    virtual QString GetScreenDesc() const;
    virtual QString GetFullScreenDesc() const;

    // Return a pointer to a BASE_SCREEN or one of its derivatives.
    // It is overloaded by derived classes to return SCH_SCREEN or PCB_SCREEN.
    virtual BASE_SCREEN* GetScreen() const  { return m_currentScreen; }

    void EraseMsgBox();

    virtual void ReCreateHToolbar() { };
    virtual void ReCreateVToolbar() { };
    virtual void ReCreateOptToolbar() { };
    virtual void ReCreateAuxiliaryToolbar() { }

    /**
     * Update the sizes of any controls in the toolbars of the frame.
     */
    virtual void UpdateToolbarControlSizes() { }

    // These 4 functions provide a basic way to show/hide grid and /get/set grid color.
    // These parameters are saved in KiCad config for each main frame.
    bool IsGridVisible() const;
    virtual void SetGridVisibility( bool aVisible );

    bool         IsGridOverridden() const;
    virtual void SetGridOverrides( bool aOverride );

    virtual COLOR4D GetGridColor() { return m_gridColor; }
    virtual void SetGridColor( const COLOR4D& aColor ) { m_gridColor = aColor; }

    // Command event handler for selecting grid sizes.
    // All commands that set the grid size should eventually end up here. This is where the
    // application setting is saved.  If you override this method, make sure you call down
    // to the base class.
    void OnSelectGrid( QCommandEvent& event );

    // Rebuild the grid combobox to respond to any changes in the GUI (units, user
    // grid changes, etc.).
    void UpdateGridSelectBox();

    // Update the checked item in the grid choice.
    void OnUpdateSelectGrid( QUpdateUIEvent& aEvent );

    // Update the checked item in the zoom choice.
    void OnUpdateSelectZoom( QUpdateUIEvent& aEvent );

    // Rebuild the grid combobox to respond to any changes in the GUI (units, user
    // grid changes, etc.)
    void UpdateZoomSelectBox();

    // Return a human readable value for display in dialogs.
    const QString GetZoomLevelIndicator() const;

    // Set the zoom factor when selected by the zoom list box in the main tool bar.
    // List position 0 is fit to page.
    // List position >= 1 = zoom (1 to zoom max).
    // Last list position is custom zoom not in zoom list.
    virtual void OnSelectZoom( QCommandEvent& event );

    // Recalculate the size of toolbars and display panel when the frame size changes.
    virtual void OnSize( QResizeEvent& event ) override;

    void OnMove( QMoveEvent& aEvent ) override;

    // Rebuild the GAL and redraws the screen.  Call when something went wrong.
    virtual void HardRedraw();

    // Redraw the screen with best zoom level and the best centering that shows all the
    // page or the board.
    virtual void Zoom_Automatique( bool aWarpPointer );

    // Useful to focus on a particular location, in find functions.
    // Move the graphic cursor (crosshair cursor) at a given coordinate and reframes the
    // drawing if the requested point is out of view or if center on location is requested.
    void FocusOnLocation( const VECTOR2I& aPos );

    // Construct a "basic" menu for a tool, containing only items that apply to all tools
    // (e.g. zoom and grid).
    void AddStandardSubMenus( TOOL_MENU& aMenu );

    // Print the drawing-sheet (frame and title block).
    void PrintDrawingSheet( const RENDER_SETTINGS* aSettings, BASE_SCREEN* aScreen,
                            const std::map<QString, QString>* aProperties, double aMils2Iu,
                            const QString& aFilename,
                            const QString& aSheetLayer = QString() );

    void DisplayToolMsg( const QString& msg ) override;

    void DisplayConstraintsMsg( const QString& msg );

    // Called when modifying the page settings.
    // In derived classes it can be used to modify parameters like draw area size,
    // and any other local parameter related to the page settings.
    virtual void OnPageSettingsChange() {}

    // Update the status bar information.
    // The EDA_DRAW_FRAME level updates the absolute and relative coordinates and the
    // zoom information.  If you override this virtual method, make sure to call this
    // subclassed method.
    void UpdateStatusBar() override;

    // Display current unit pane in the status bar.
    void DisplayUnitsMsg();

    // Display current grid size in the status bar.
    virtual void DisplayGridMsg();

    void LoadSettings( APP_SETTINGS_BASE* aCfg ) override;
    void SaveSettings( APP_SETTINGS_BASE* aCfg ) override;

    // Append a message to the message panel.
    // This helper method checks to make sure the message panel exists in the frame and
    // appends a message to it using the message panel AppendMessage() method.
    void AppendMsgPanel( const QString& aTextUpper, const QString& aTextLower, int aPadding = 6 );

    // Clear all messages from the message panel.
    virtual void ClearMsgPanel();

    // Clear the message panel and populates it with the contents of aList.
    void SetMsgPanel( const std::vector< MSG_PANEL_ITEM >& aList );
    void SetMsgPanel( EDA_ITEM* aItem );

    // Helper function that erases the msg panel and then appends a single message
    void SetMsgPanel( const QString& aTextUpper, const QString& aTextLower, int aPadding = 6 );

    // Redraw the message panel.
    virtual void UpdateMsgPanel();

    virtual LIB_TREE* GetLibTree() const { return nullptr; }
    virtual LIB_ID GetTargetLibId() const { return LIB_ID(); }

    virtual bool IsLibraryTreeShown() const { return false; }
    virtual void ToggleLibraryTree() {};
    virtual void FocusLibraryTreeInput() {};

    PROPERTIES_PANEL* GetPropertiesPanel() { return m_propertiesPanel; }

    void UpdateProperties();

    virtual void ToggleProperties() {}

    static const QString PropertiesPaneName() { return QStringLiteral( "PropertiesManager" ); }

    static const QString NetInspectorPanelName() { return QStringLiteral( "NetInspector" ); }

    static const QString DesignBlocksPaneName() { return QStringLiteral( "DesignBlocks" ); }

    static const QString AppearancePanelName() { return QStringLiteral( "LayersManager" ); }

    // Fetch an item by KIID.  Frame-type-specific implementation.
    virtual EDA_ITEM* GetItem( const KIID& aId ) const { return nullptr; }

    // Print the page pointed by current screen, set by the calling print function.
    virtual void PrintPage( const RENDER_SETTINGS* aSettings );

    // Use to start up the GAL drawing canvas.
    virtual void ActivateGalCanvas();

    // Change the current rendering backend.
    virtual void SwitchCanvas( EDA_DRAW_PANEL_GAL::GAL_TYPE aCanvasType );

    // Return a pointer to GAL-based canvas of given EDA draw frame.
    virtual EDA_DRAW_PANEL_GAL* GetCanvas() const { return m_canvas; }
    void SetCanvas( EDA_DRAW_PANEL_GAL* aPanel ) { m_canvas = aPanel; }

    QWidget* GetToolCanvas() const override { return GetCanvas(); }

    // Return a reference to the gal rendering options used by GAL for rendering.
    GAL_DISPLAY_OPTIONS_IMPL& GetGalDisplayOptions() { return m_galDisplayOptions; }

    void RefreshCanvas() override
    {
        GetCanvas()->Refresh();
    }

    // Return bounding box of document with option to not include some items.
    // Used most commonly by "Zoom to Fit" and "Zoom to Objects".  In Eeschema for "Zoom to Fit"
    // it's passed "true" to include drawing sheet border, and "false" by "Zoom To Objects" to
    // ignore drawing sheet border.  In Pcbnew, false makes it ignore any items outside the PCB
    // edge such as fabrication notes.
    virtual const BOX2I GetDocumentExtents( bool aIncludeAllVisible = true ) const;

    // Rebuild all toolbars and update the checked state of check tools.
    void RecreateToolbars();

    // Update toolbars if desired toolbar icon changed.
    void OnToolbarSizeChanged();

    // Redraw the menus and what not in current language.
    void ShowChangedLanguage() override;

    HOTKEY_CYCLE_POPUP* GetHotkeyPopup() { return m_hotkeyPopup; }

    virtual void CreateHotkeyPopup();


    // Save the current view as an image file.
    bool SaveCanvasImageToFile( const QString& aFileName, BITMAP_TYPE aBitmapType );

    // Handler for activating an API plugin (via toolbar or menu).
    virtual void OnApiPluginInvoke( QCommandEvent& aEvent );

    virtual PLUGIN_ACTION_SCOPE PluginActionScope() const { return PLUGIN_ACTION_SCOPE::INVALID; }

    static bool IsPluginActionButtonVisible( const PLUGIN_ACTION& aAction,
                                             APP_SETTINGS_BASE* aCfg );

    // Return ordered list of plugin actions for display in the toolbar.
    // Must be static at the moment because this needs to be called from the preferences dialog,
    // which can exist without the frame in question actually being created.
    static std::vector<const PLUGIN_ACTION*> GetOrderedPluginActions( PLUGIN_ACTION_SCOPE aScope,
        APP_SETTINGS_BASE* aCfg );

    Q_OBJECT

protected:
    virtual void SetScreen( BASE_SCREEN* aScreen )  { m_currentScreen = aScreen; }

    void unitsChangeRefresh() override;

    void setupUnits( APP_SETTINGS_BASE* aCfg );

    void updateStatusBarWidths();

    std::vector<QWidget*> findDialogs();

    // Determine the canvas type to load (with prompt if required) and initializes m_canvasType.
    virtual void resolveCanvasType();

    // Return the canvas type stored in the application settings.
    EDA_DRAW_PANEL_GAL::GAL_TYPE loadCanvasTypeSetting( APP_SETTINGS_BASE* aCfg = nullptr );

    // Store the canvas type in the application settings.
    bool saveCanvasTypeSetting( EDA_DRAW_PANEL_GAL::GAL_TYPE aCanvasType );

    // Handle a window activation event.
    virtual void handleActivateEvent( QEvent& aEvent );
    void onActivate( QEvent& aEvent );

    // Append actions from API plugins to the main toolbar
    virtual void addApiPluginTools();


    QTcpServer*             m_socketServer;

    // Prevents opening same file multiple times.
    std::unique_ptr<LOCKFILE> m_file_checker;

    COLOR4D              m_gridColor;         // Grid color
    COLOR4D              m_drawBgColor;       // The background color of the draw canvas; BLACK for
                                              // Pcbnew, BLACK or WHITE for Eeschema
    int                  m_undoRedoCountMax;  // Default Undo/Redo command Max depth, to be handed
                                              // to screens
    bool                 m_polarCoords;       // For those frames that support polar coordinates

    // Show the drawing sheet (border & title block).
    bool                 m_showBorderAndTitleBlock;

    QComboBox*            m_gridSelectBox;
    QComboBox*            m_zoomSelectBox;

    ACTION_TOOLBAR*      m_mainToolBar;
    ACTION_TOOLBAR*      m_auxiliaryToolBar;  // Additional tools under main toolbar
    ACTION_TOOLBAR*      m_drawToolBar;       // Drawing tools (typically on right edge of window)
    ACTION_TOOLBAR*      m_optionsToolBar;    // Options (typically on left edge of window)

    std::unique_ptr<EDA_SEARCH_DATA> m_findReplaceData;
    QStringList        m_findStringHistoryList;
    QStringList        m_replaceStringHistoryList;

    EDA_MSG_PANEL*       m_messagePanel;
    int                  m_msgFrameHeight;

    COLOR_SETTINGS*      m_colorSettings;
    SEARCH_PANE*         m_searchPane;
    PROPERTIES_PANEL*    m_propertiesPanel;
    NET_INSPECTOR_PANEL* m_netInspectorPanel;

    HOTKEY_CYCLE_POPUP* m_hotkeyPopup;

    // The current canvas type.
    EDA_DRAW_PANEL_GAL::GAL_TYPE    m_canvasType;

    static bool m_openGLFailureOccured; // Has any failure occurred when switching to OpenGL in
                                        // any EDA_DRAW_FRAME?

private:
    BASE_SCREEN*                m_currentScreen;      // current used SCREEN
    EDA_DRAW_PANEL_GAL*         m_canvas;

    // This the frame's interface to setting GAL display options.
    GAL_DISPLAY_OPTIONS_IMPL  m_galDisplayOptions;

    int m_lastToolbarIconSize;
};

#endif  // DRAW_FRAME_H_
