
#ifndef  EDA_BASE_FRAME_H_
#define  EDA_BASE_FRAME_H_


#include <QVector>
#include <QHash>
#include <QMainWindow>
#include <QWidget>
#include <QMenuBar>
#include <QStatusBar>
#include <QTimer>
#include <QDropEvent>
#include <QString>
#include <QPoint>
#include <QSize>
#include <QKeyEvent>
#include <QMoveEvent>
#include <QResizeEvent>
#include <QCloseEvent>
#include <QTimerEvent>
#include <QFileInfo>
#include <QEvent>
#include <functional>

#include <layer_ids.h>
#include <frame_type.h>
#include <hotkeys_basic.h>
#include <kiway_holder.h>
#include <tool/tools_holder.h>
#include "widgets/ui_common.h"
#include <widgets/qt_infobar.h>
#include <undo_redo_container.h>
#include <units_provider.h>
#include <origin_transforms.h>
#include <ui_events.h>

// Option for main frames
#define KICAD_DEFAULT_DRAWFRAME_STYLE (Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint)

#define VIEWER3D_FRAMENAME "Viewer3DFrameName"
#define QUALIFIED_VIEWER3D_FRAMENAME( parent ) \
                    ( QString( VIEWER3D_FRAMENAME ) + ":" + parent->objectName() )

#define KICAD_MANAGER_FRAME_NAME   "KicadFrame"


class QComboBox;
class QEvent;
class QFileInfo;
class EDA_ITEM;
class EDA_DRAW_PANEL_GAL;
class EDA_MSG_PANEL;
class BASE_SCREEN;
class PAGE_INFO;
class PLOTTER;
class TITLE_BLOCK;
class MSG_PANEL_ITEM;
class TOOL_MANAGER;
class TOOL_DISPATCHER;
class ACTIONS;
class PAGED_DIALOG;
class DIALOG_EDIT_LIBRARY_TABLES;
class PANEL_HOTKEYS_EDITOR;
class FILE_HISTORY;
class SETTINGS_MANAGER;
class SEARCH_STACK;
class APP_SETTINGS_BASE;
class APPEARANCE_CONTROLS_3D;
struct WINDOW_SETTINGS;
struct WINDOW_STATE;
class ACTION_MENU;
class TOOL_INTERACTIVE;

#define DEFAULT_MAX_UNDO_ITEMS 0
#define ABS_MAX_UNDO_ITEMS (INT_MAX / 2)

// This is the handler functor for the update UI events
typedef std::function< void( QEvent& ) > UIUpdateHandler;

// The base frame for deriving all KiCad main window classes.
class EDA_BASE_FRAME : public QMainWindow, public TOOLS_HOLDER, public KIWAY_HOLDER,
                       public UNITS_PROVIDER
{
public:
    enum UNDO_REDO_LIST
    {
        UNDO_LIST,
        REDO_LIST
    };

    EDA_BASE_FRAME( QWidget* aParent, FRAME_T aFrameType, const QString& aTitle,
                    const QPoint& aPos, const QSize& aSize, Qt::WindowFlags aStyle,
                    const QString& aFrameName, KIWAY* aKiway, const EDA_IU_SCALE& aIuScale );

    ~EDA_BASE_FRAME();

    void ChangeUserUnits( EDA_UNITS aUnits );

    virtual void ToggleUserUnits() { }

    ORIGIN_TRANSFORMS& GetOriginTransforms() override
    {
        return m_originTransforms;
    }

    SETTINGS_MANAGER* GetSettingsManager() const { return m_settingsManager; }

    virtual SEVERITY GetSeverity( int aErrorCode ) const { return RPT_SEVERITY_UNDEFINED; }

    bool ProcessEvent( QEvent& aEvent );

    virtual void OnCharHook( QKeyEvent& aKeyEvent );

    void OnMenuEvent( QEvent& event );

    virtual void RegisterUIUpdateHandler( int aID, const ACTION_CONDITIONS& aConditions ) override;

    virtual void UnregisterUIUpdateHandler( int aID ) override;

    static void HandleUpdateUIEvent( QEvent& aEvent, EDA_BASE_FRAME* aFrame,
                                     ACTION_CONDITIONS aCond );

    virtual void OnMove( QMoveEvent& aEvent )
    {
        // Qt equivalent of aEvent.Skip() is accepting the event
        aEvent.accept();
    }

    virtual void OnSize( QResizeEvent& aEvent );

    void OnMaximize( QEvent& aEvent );

    int GetAutoSaveInterval() const;

    bool IsType( FRAME_T aType ) const { return m_ident == aType; }
    FRAME_T GetFrameType() const { return m_ident; }

    virtual const SEARCH_STACK& sys_search();

    virtual QString help_name();

    void OnKicadAbout( QEvent& event );
    void OnPreferences( QEvent& event );

    void ShowPreferences( QString aStartPage, QString aStartParentPage );

    void PrintMsg( const QString& text );

    void CreateInfoBar();

    void FinishLayoutInitialization();

    QT_INFOBAR* GetInfoBar() { return m_infoBar; }

    void ShowInfoBarError( const QString& aErrorMsg, bool aShowCloseButton = false,
                           QT_INFOBAR::MESSAGE_TYPE aType = QT_INFOBAR::MESSAGE_TYPE::GENERIC );

    void ShowInfoBarError( const QString& aErrorMsg, bool aShowCloseButton,
                           std::function<void(void)> aCallback );

    void ShowInfoBarWarning( const QString& aWarningMsg, bool aShowCloseButton = false );

    void ShowInfoBarMsg( const QString& aMsg, bool aShowCloseButton = false );

    virtual APP_SETTINGS_BASE* config() const;

    void LoadWindowState( const QString& aFileName );

    void LoadWindowSettings( const WINDOW_SETTINGS* aCfg );

    void SaveWindowSettings( WINDOW_SETTINGS* aCfg );

    virtual void LoadSettings( APP_SETTINGS_BASE* aCfg );

    virtual void SaveSettings( APP_SETTINGS_BASE* aCfg );

    virtual WINDOW_SETTINGS* GetWindowSettings( APP_SETTINGS_BASE* aCfg );

    virtual void LoadWindowState( const WINDOW_STATE& aState );

    QString ConfigBaseName() override
    {
        QString baseCfgName = m_configName.isEmpty() ? objectName() : m_configName;
        return baseCfgName;
    }

    virtual void SaveProjectLocalSettings() {};

    void ImportHotkeyConfigFromFile( QHash<std::string, TOOL_ACTION*> aActionMap,
                                     const QString& aDefaultShortname );

    QString GetFileFromHistory( int cmdId, const QString& type,
                                FILE_HISTORY* aFileHistory = nullptr );

    void ClearFileHistory( FILE_HISTORY* aFileHistory = nullptr );

    void UpdateFileHistory( const QString& FullFileName, FILE_HISTORY* aFileHistory = nullptr );

    FILE_HISTORY& GetFileHistory()
    {
        return *m_fileHistory;
    }

    void SetMruPath( const QString& aPath ) { m_mruPath = aPath; }

    QString GetMruPath() const { return m_mruPath; }

    virtual QString GetCurrentFileName() const { return QString(); }

    void ReCreateMenuBar();

    void AddStandardHelpMenu( QMenuBar* aMenuBar );

    bool IsWritable( const QFileInfo& aFileName, bool aVerbose = true );

    virtual void CheckForAutoSaveFile( const QFileInfo& aFileName );

    virtual void DeleteAutoSaveFile( const QFileInfo& aFileName );

    virtual void UpdateStatusBar() { }

    void ShowChangedLanguage() override;

    void CommonSettingsChanged( int aFlags ) override;

    virtual void ThemeChanged();

    virtual void ProjectChanged() {}

    const QString& GetAboutTitle() const { return m_aboutTitle; }

    const QString& GetUntranslatedAboutTitle() const { return m_aboutTitle; }

    virtual bool IsContentModified() const;

    QSize GetWindowSize();

    virtual void ClearUndoORRedoList( UNDO_REDO_LIST aList, int aItemCount = -1 )
    { }

    virtual void ClearUndoRedoList();

    virtual void PushCommandToUndoList( PICKED_ITEMS_LIST* aItem );

    virtual void PushCommandToRedoList( PICKED_ITEMS_LIST* aItem );

    virtual PICKED_ITEMS_LIST* PopCommandFromUndoList();

    virtual PICKED_ITEMS_LIST* PopCommandFromRedoList();

    virtual int GetUndoCommandCount() const { return m_undoList.m_CommandsList.size(); }
    virtual int GetRedoCommandCount() const { return m_redoList.m_CommandsList.size(); }

    virtual QString GetUndoActionDescription() const;
    virtual QString GetRedoActionDescription() const;

    int GetMaxUndoItems() const { return m_undoRedoCountMax; }

    virtual void OnModify();

    bool IsClosing() const { return m_isClosing; }

    bool NonUserClose( bool aForce )
    {
        m_isNonUserClose = true;
        return close();
    }

    virtual void HandleSystemColorChange();

    virtual bool CanAcceptApiCommands() { return isEnabled(); }

protected:
    // Default style flags used for Qt toolbars.
    static constexpr int KICAD_QT_TB_STYLE = Qt::ToolButtonTextUnderIcon;

    virtual void doReCreateMenuBar() {}

    void onAutoSaveTimer( QTimerEvent& aEvent );

    virtual void handleIconizeEvent( QEvent& aEvent ) {}
    void         onIconize( QEvent& aEvent );

    virtual bool isAutoSaveRequired() const { return m_autoSaveRequired; }

    virtual bool doAutoSave();

    virtual bool canCloseWindow( QCloseEvent& aCloseEvent ) { return true; }
    virtual void doCloseWindow() { }

    void onSystemColorChange( QEvent& aEvent );

    virtual void unitsChangeRefresh() { }

    virtual void setupUIConditions();

    void initExitKey();

    void ensureWindowIsOnScreen();

    virtual void saveProjectSettings() {}

    virtual void OnDropFiles( QDropEvent& aEvent );

    void AddMenuLanguageList( ACTION_MENU* aMasterMenu, TOOL_INTERACTIVE* aControlTool );

    virtual void            DoWithAcceptedFiles();
    QVector<QFileInfo> m_AcceptedFiles;

private:
    void windowClosing( QCloseEvent& event );

    void commonInit( FRAME_T aFrameType );

    QWidget* findQuasiModalDialog();

    virtual bool IsModal() const { return false; }

#ifdef _WIN32
    bool nativeEvent( const QByteArray& eventType, void* message, long* result ) override;
#endif

 protected:
    FRAME_T         m_ident;                // Id Type (pcb, schematic, library..)
    QPoint          m_framePos;
    QSize           m_frameSize;
    bool            m_maximizeByDefault;
    int             m_displayIndex;

    // These contain the frame size and position for when it is not maximized
    QPoint          m_normalFramePos;
    QSize           m_normalFrameSize;

    QString                 m_aboutTitle;        // Name of program displayed in About.

    // Custom layout management system (replacing wxAuiManager)
    QString                 m_perspective;       // Layout perspective.
    QT_INFOBAR*             m_infoBar;           // Infobar for the frame
    APPEARANCE_CONTROLS_3D* m_appearancePanel;
    QString                 m_configName;        // Prefix used to identify some params (frame
                                                 // size) and to name some config files (legacy
                                                 // hotkey files)
    SETTINGS_MANAGER*       m_settingsManager;

    FILE_HISTORY*           m_fileHistory;       // The frame's recently opened file list
    bool                    m_supportsAutoSave;
    bool                    m_autoSavePending;
    bool                    m_autoSaveRequired;
    QTimer*                 m_autoSaveTimer;

    int                     m_undoRedoCountMax;  // undo/Redo command Max depth

    UNDO_REDO_CONTAINER     m_undoList;          // Objects list for the undo command (old data)
    UNDO_REDO_CONTAINER     m_redoList;          // Objects list for the redo command (old data)

    QString                 m_mruPath;           // Most recently used path.

    ORIGIN_TRANSFORMS       m_originTransforms;  // Default display origin transforms object.

    // Map containing the UI update handlers registered with Qt for each action.
    QHash<int, UIUpdateHandler> m_uiUpdateMap;

    // Set by the close window event handler after frames are asked if they can close.
    // Allows other functions when called to know our state is cleanup.
    bool            m_isClosing;

    // Set by NonUserClose() to indicate that the user did not request the current close.
    bool            m_isNonUserClose;

    // Associate file extensions with action to execute.
    QHash<const QString, TOOL_ACTION*> m_acceptedExts;
};


// Specialization of pane configuration for KiCad panels.
// Replaces wxAuiPaneInfo with Qt-based layout management.
class EDA_PANE
{
public:
    EDA_PANE()
    {
        m_hasGripper = false;
        m_hasCloseButton = false;
        m_hasBorder = false;
    }

    EDA_PANE& HToolbar()
    {
        m_isToolbar = true;
        m_captionVisible = false;
        m_topDockable = true;
        m_bottomDockable = true;
        m_dockFixed = true;
        m_movable = false;
        m_resizable = true;
        return *this;
    }

    EDA_PANE& VToolbar()
    {
        m_isToolbar = true;
        m_captionVisible = false;
        m_leftDockable = true;
        m_rightDockable = true;
        m_dockFixed = true;
        m_movable = false;
        m_resizable = true;
        return *this;
    }

    EDA_PANE& Palette()
    {
        m_captionVisible = true;
        m_hasBorder = true;
        return *this;
    }

    EDA_PANE& Canvas()
    {
        m_captionVisible = false;
        m_layer = 0;
        m_hasBorder = true;
        m_resizable = true;
        return *this;
    }

    EDA_PANE& Messages()
    {
        m_captionVisible = false;
        m_bottomDockable = true;
        m_dockFixed = true;
        m_movable = false;
        m_resizable = true;
        return *this;
    }

    EDA_PANE& InfoBar()
    {
        m_captionVisible = false;
        m_movable = false;
        m_resizable = true;
        m_hasBorder = false;
        m_dockFixed = true;
        return *this;
    }

private:
    bool m_hasGripper = true;
    bool m_hasCloseButton = true;
    bool m_hasBorder = true;
    bool m_isToolbar = false;
    bool m_captionVisible = true;
    bool m_topDockable = false;
    bool m_bottomDockable = false;
    bool m_leftDockable = false;
    bool m_rightDockable = false;
    bool m_dockFixed = false;
    bool m_movable = true;
    bool m_resizable = false;
    int m_layer = 1;
};

#endif  // EDA_BASE_FRAME_H_
