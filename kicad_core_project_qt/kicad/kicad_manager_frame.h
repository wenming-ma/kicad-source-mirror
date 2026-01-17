
#ifndef KICAD_H
#define KICAD_H

#include <kiway_player.h>
#include <QtWidgets/QWidget>
#include <QtCore/QString>
#include <QtCore/QPoint>
#include <QtCore/QSize>
#include <QtWidgets/QStatusBar>
#include <QtCore/QEvent>
#include <QtWidgets/QTabWidget>
#include <QtCore/QFileInfo>
#include <QtGui/QCloseEvent>
#include <QtGui/QResizeEvent>

class ACTION_TOOLBAR;
class BITMAP_BUTTON;
class EDA_BASE_FRAME;
class KICAD_SETTINGS;
class PANEL_KICAD_LAUNCHER;
class PLUGIN_CONTENT_MANAGER;
class PROJECT_TREE;
class PROJECT_TREE_PANE;
class UPDATE_MANAGER;

/**
 * The main KiCad project manager frame.  It is not a KIWAY_PLAYER.
 */
class KICAD_MANAGER_FRAME : public EDA_BASE_FRAME
{
    Q_OBJECT

public:
    KICAD_MANAGER_FRAME( QWidget* parent, const QString& title,
                         const QPoint& pos, const QSize& size );

    ~KICAD_MANAGER_FRAME();

    void OnIdle( QEvent& event );

    bool canCloseWindow( QCloseEvent& aCloseEvent ) override;
    void doCloseWindow() override;
    void OnSize( QResizeEvent& event ) override;

    void OnArchiveFiles( QEvent& event );
    void OnUnarchiveFiles( QEvent& event );

    void OnOpenFileInTextEditor( QEvent& event );
    void OnBrowseInFileExplorer( QEvent& event );

    void OnFileHistory( QEvent& event );
    void OnClearFileHistory( QEvent& aEvent );
    void OnExit( QEvent& event );

    // Create the status line (like a QStatusBar). This is actually a KISTATUSBAR status bar.
    // the specified number of fields is the extra number of fields, not the full field count.
    // @return a KISTATUSBAR (derived from QStatusBar)
    QStatusBar* OnCreateStatusBar( int number, long style, int id,
                                   const QString& name ) override;

    /**
     * Hides the tabs for Editor notebook if there is only 1 page
     */
    void HideTabsIfNeeded();

    /**
     * (Re)Create the left vertical toolbar
     */
    void RecreateBaseLeftToolbar();

    QString GetCurrentFileName() const override
    {
        return GetProjectFileName();
    }

    // @brief Creates a project and imports a non-KiCad Schematic and PCB
    // @param aWindowTitle to display to the user when opening the files
    // @param aFilesWildcard that includes both PCB and Schematic files (from
    // wildcards_and_files_ext.h)
    // @param aSchFileExtensions e.g. { "sch" } or { "csa" }. Specify { "INPUT" } to copy input file.
    // @param aPcbFileExtensions e.g. { "brd" } or { "cpa" }. Specify { "INPUT" } to copy input file.
    // @param aSchFileType Type of Schematic File to import (from SCH_IO_MGR::SCH_FILE_T)
    // @param aPcbFileType Type of PCB File to import (from IO_MGR::PCB_FILE_T)
    void ImportNonKiCadProject( const QString& aWindowTitle, const QString& aFilesWildcard,
                                const std::vector<std::string>& aSchFileExtensions,
                                const std::vector<std::string>& aPcbFileExtensions,
                                int aSchFileType, int aPcbFileType );

    // Open dialog to import Altium project files.
    void OnImportAltiumProjectFiles( QEvent& event );

    // Open dialog to import CADSTAR Schematic and PCB Archive files.
    void OnImportCadstarArchiveFiles( QEvent& event );

    // Open dialog to import Eagle schematic and board files.
    void OnImportEagleFiles( QEvent& event );

    // Open dialog to import EasyEDA Std schematic and board files.
    void OnImportEasyEdaFiles( QEvent& event );

    // Open dialog to import EasyEDA Pro schematic and board files.
    void OnImportEasyEdaProFiles( QEvent& event );

    // Prints the current working directory name and the project name on the text panel.
    void PrintPrjInfo();

    void RefreshProjectTree();

    // Creates a new project by setting up and initial project, schematic, and board files.
    //
    // The project file is copied from the kicad.pro template file if possible.  Otherwise,
    // a minimal project file is created from an empty project.  A minimal schematic and
    // board file are created to prevent the schematic and board editors from complaining.
    // If any of these files already exist, they are not overwritten.
    //
    // @param aProjectFileName is the absolute path of the project file name.
    // @param aCreateStubFiles specifies if an empty PCB and schematic should be created
    void CreateNewProject( const QFileInfo& aProjectFileName, bool aCreateStubFiles = true );

    // Closes the project, and saves it if aSave is true;
    bool CloseProject( bool aSave );
    void LoadProject( const QFileInfo& aProjectFileName );

    void OpenJobsFile( const QFileInfo& aFileName, bool aCreate = false,
                       bool aResaveProjectPreferences = true );


    void LoadSettings( APP_SETTINGS_BASE* aCfg ) override;

    void SaveSettings( APP_SETTINGS_BASE* aCfg ) override;

    void ShowChangedLanguage() override;
    void CommonSettingsChanged( int aFlags ) override;
    void ProjectChanged() override;

    // Called by sending a event with id = ID_INIT_WATCHED_PATHS
    // rebuild the list of watched paths
    void OnChangeWatchedPaths( QEvent& aEvent );

    const QString GetProjectFileName() const;

    bool IsProjectActive();
    // read only accessors
    const QString SchFileName();
    const QString SchLegacyFileName();
    const QString PcbFileName();
    const QString PcbLegacyFileName();

    void ReCreateTreePrj();

    // @param aIsExplicitUserSave is true to indicate the user ran a Save Project action explicitly
    //        Note that this parameter should currently *always* be false, because there is no
    //        explicit Save Project action in the project manager.  This means that anytime the
    //        project manager saves project local settings, it is an implicit save (and should not
    //        actually save the file if it was migrated)
    void SaveOpenJobSetsToLocalSettings( bool aIsExplicitUserSave = false );

    QWidget* GetToolCanvas() const override;

    std::shared_ptr<PLUGIN_CONTENT_MANAGER> GetPcm() { return m_pcm; };

    void SetPcmButton( BITMAP_BUTTON* aButton );

    void CreatePCM();   // creates the PLUGIN_CONTENT_MANAGER

    // Used only on Windows: stores the info message about file watcher
    QString m_FileWatcherInfo;

protected:
    virtual void setupUIConditions() override;

    void doReCreateMenuBar() override;

    void onToolbarSizeChanged();

    void onNotebookPageCloseRequest( QEvent& evt );

    void onNotebookPageCountChanged( QEvent& evt );

private:
    void setupTools();
    void setupActions();

    void DoWithAcceptedFiles() override;

    APP_SETTINGS_BASE* config() const override;

    KICAD_SETTINGS* kicadSettings() const;

    const SEARCH_STACK& sys_search() override;

    QString help_name() override;

    void language_change( QEvent& event );

    void updatePcmButtonBadge();

    bool m_openSavedWindows;
    int  m_leftWinWidth;
    bool m_active_project;

    PROJECT_TREE_PANE*    m_leftWin;
    QTabWidget*           m_notebook;
    PANEL_KICAD_LAUNCHER* m_launcher;
    ACTION_TOOLBAR*       m_mainToolBar;
    int                   m_lastToolbarIconSize;

    std::shared_ptr<PLUGIN_CONTENT_MANAGER> m_pcm;
    BITMAP_BUTTON*                          m_pcmButton;
    int                                     m_pcmUpdateCount;
    std::unique_ptr<UPDATE_MANAGER>         m_updateManager;
};


// The C++ project manager includes a single PROJECT in its link image.
class PROJECT;
extern PROJECT& Prj();

#endif
