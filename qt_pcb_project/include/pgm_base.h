
#ifndef  PGM_BASE_H_
#define  PGM_BASE_H_

#include <kicommon.h>
#include <singleton.h>
#include <exception>
#include <map>
#include <vector>
#include <memory>
#include <search_stack.h>
#include <settings/environment.h>
#include <QString>
#include <QCoreApplication>
#include <QLocale>
#include <QSplashScreen>

class QCoreApplication;
class QMenu;
class QWidget;
class QSplashScreen;

class BACKGROUND_JOBS_MONITOR;
class NOTIFICATIONS_MANAGER;
class COMMON_SETTINGS;
class SETTINGS_MANAGER;

#ifdef KICAD_IPC_API
class API_PLUGIN_MANAGER;
class KICAD_API_SERVER;
#endif

// A small class to handle the list of existing translations.
struct KICOMMON_API LANGUAGE_DESCR
{
    // Qt locale identifier
    int         m_WX_Lang_Identifier;

    // KiCad identifier used in menu selection
    int         m_KI_Lang_Identifier;

    // Labels used in menus
    QString     m_Lang_Label;

    // Set to true if the m_Lang_Label must not be translated
    bool        m_DoNotTranslate;
};


// An array containing all the languages that KiCad supports.
KICOMMON_API extern LANGUAGE_DESCR LanguagesList[];

// Container for data for KiCad programs.
// The functions are virtual so we can do cross module calls without linking to them.
class KICOMMON_API PGM_BASE
{
public:
    PGM_BASE();
    virtual ~PGM_BASE();

    // Builds the UTF8 based argv variable
    void BuildArgvUtf8();

    BS::thread_pool& GetThreadPool() { return *m_singleton.m_ThreadPool; }

    GL_CONTEXT_MANAGER* GetGLContextManager() { return m_singleton.m_GLContextManager; }

    // Specific to MacOSX (not used under Linux or Windows).
    // MacOSX requires it for file association.
    virtual void MacOpenFile( const QString& aFileName ) = 0;

    virtual SETTINGS_MANAGER& GetSettingsManager() const { return *m_settings_manager; }

    virtual COMMON_SETTINGS*  GetCommonSettings() const;

    virtual BACKGROUND_JOBS_MONITOR& GetBackgroundJobMonitor() const
    {
        return *m_background_jobs_monitor;
    }

    virtual NOTIFICATIONS_MANAGER& GetNotificationsManager() const
    {
        return *m_notifications_manager;
    }

#ifdef KICAD_IPC_API
    virtual API_PLUGIN_MANAGER& GetPluginManager() const { return *m_plugin_manager; }

    KICAD_API_SERVER& GetApiServer() { return *m_api_server; }
#endif

    virtual void SetTextEditor( const QString& aFileName );

    // Return the path to the preferred text editor application.
    // If no editor is currently set and aCanShowFileChooser is true then
    // this method will show a file chooser dialog asking for the editor's executable.
    // Returns the full path of the editor, or an empty string if no editor has been set.
    virtual const QString& GetTextEditor( bool aCanShowFileChooser = true );

    // Show a dialog that instructs the user to select a new preferred editor.
    // aDefaultEditor is the default full path for the default editor this dialog should show by default.
    // Returns the full path of the editor, or an empty string if no editor was chosen.
    virtual const QString AskUserForPreferredEditor( const QString& aDefaultEditor = QString() );

    virtual bool IsKicadEnvVariableDefined() const               { return !m_kicad_env.isEmpty(); }

    virtual const QString& GetKicadEnvVariable() const          { return m_kicad_env; }

    virtual const QString& GetExecutablePath() const;

    virtual QLocale* GetLocale()                                { return m_locale; }

    virtual const QString& GetPdfBrowserName() const            { return m_pdf_browser; }

    virtual void SetPdfBrowserName( const QString& aFileName )  { m_pdf_browser = aFileName; }

    // Return true if the PDF browser is the default (system) PDF browser and false if the
    // PDF browser is the preferred (selected) browser, else returns false if there is no selected browser.
    virtual bool UseSystemPdfBrowser() const
    {
        return m_use_system_pdf_browser || m_pdf_browser.isEmpty();
    }

    // Force the use of system PDF browser, even if a preferred PDF browser is set.
    virtual void ForceSystemPdfBrowser( bool aFlg ) { m_use_system_pdf_browser = aFlg; }

    // Set the dictionary file name for internationalization.
    // The files are in kicad/internat/xx or kicad/internat/xx_XX and are named kicad.mo
    // aErrMsg is the string to return the error message it.
    // first_time must be set to true the first time this function is called, false otherwise.
    // Returns false if there was an error setting the language.
    virtual bool SetLanguage( QString& aErrMsg, bool first_time = false );

    // Set the default language without reference to any preferences.
    // Can be used to set the language for dialogs that show before preferences are loaded.
    // aErrMsg String to return the error message(s) in.
    // Returns false if the language could not be set.
    bool SetDefaultLanguage( QString& aErrMsg );

    // Set in .m_language_id member the Qt language identifier ID from the KiCad
    // menu id (internal menu identifier).
    // menu_id The KiCad menuitem id (returned by Menu Event, when clicking on a menu item)
    virtual void SetLanguageIdentifier( int menu_id );

    // Return the Qt language identifier Id of the language currently selected.
    virtual int GetSelectedLanguageIdentifier() const { return m_language_id; }

    // Return the current selected language in rfc3066 format
    virtual QString GetLanguageTag();

    virtual void SetLanguagePath();

    // Read the PDF browser choice from the common configuration.
    virtual void ReadPdfBrowserInfos();

    // Save the PDF browser choice to the common configuration.
    virtual void WritePdfBrowserInfos();

    // Set the environment variable aName to aValue.
    // This function first checks to see if the environment variable aName is already
    // defined. If it is not defined, then the environment variable aName is set to
    // a value. Otherwise, the environment variable is left unchanged. This allows the user
    // to override environment variables for testing purposes.
    // aName is a QString containing the environment variable name.
    // aValue is a QString containing the environment variable value.
    // Returns true if the environment variable Name was set to aValue.
    virtual bool SetLocalEnvVariable( const QString& aName, const QString& aValue );

    // Update the local environment with the contents of the current ENV_VAR_MAP stored in the
    // COMMON_SETTINGS.
    virtual void SetLocalEnvVariables();

    virtual ENV_VAR_MAP& GetLocalEnvVariables() const;

    // Return a bare naked QCoreApplication which may come from SINGLE_TOP, or kicad.exe.
    // This should return what qApp returns.
    virtual QCoreApplication&   App();

    static const QChar workingDirKey[];

    // Initialize this program.
    // Initialize the process in a KiCad standard way using some generalized techniques:
    //  - Default paths (help, libs, bin) and configuration file names
    //  - Language and locale
    //  - fonts
    // Note: Do not initialize anything relating to DSOs or projects.
    // aHeadless If true, run in headless mode (e.g. for unit tests)
    // aSkipPyInit If true, do not init python stuff.
    // Useful in application that do not use python, to disable python dependency at run time
    // Returns true if success, false if failure and program is to terminate.
    bool InitPgm( bool aHeadless = false, bool aSkipPyInit = false, bool aIsUnitTest = false );

    // The PGM_* classes can have difficulties at termination if they
    // are not destroyed soon enough. Relying on a static destructor can be
    // too late for contained objects.
    void Destroy();

    // Save the program (process) settings subset which are stored .kicad_common.
    void SaveCommonSettings();

#ifdef KICAD_USE_SENTRY
    // Return True if the user agreed to sentry data collection
    bool IsSentryOptedIn();

    // Set the Sentry opt in state, this will also terminate sentry
    // immediately if needed, however it will not init sentry if opted in.
    // aOptIn True/false to agreeing to the use of sentry.
    void SetSentryOptIn( bool aOptIn );

    // Generate and stores a new sentry id at random using the boost uuid generator.
    void ResetSentryId();

    // Get the current id string being used as "user id" in sentry reports.
    const QString& GetSentryId();
#endif

    // A exception handler to be used at the top level if exceptions bubble up that for.
    // The purpose is to have a central place to log a Qt error message and/or sentry report.
    // aPtr Pass the std::current_exception() from within the catch block.
    void HandleException( std::exception_ptr aPtr );

    // A common assert handler to be used between single_top and kicad.
    // This lets us have a common set of assert handling, including triggering sentry reports.
    // aFile the file path of the assert.
    // aLine the line number of the assert.
    // aFunc the function name the assert is within.
    // aCond the condition of the assert.
    // aMsg the attached assert message (can be empty).
    void HandleAssert( const QString& aFile, int aLine, const QString& aFunc,
                       const QString& aCond, const QString& aMsg );

    // Determine if the application is running with a GUI.
    // Returns true if there is a GUI and false otherwise.
    bool IsGUI();

    void ShowSplash();
    void HideSplash();

    // Allow access to test for other running KiCads.
    std::unique_ptr<QObject>& SingleInstance()
    {
        return m_pgm_checker;
    }

    // Qt on MSW tends to crash if you spool up more than one print job at a time.
    bool m_Printing;

    std::vector<void*> m_ModalDialogs;

    bool m_Quitting;

    bool m_PropertyGridInitialized;

protected:
    // Load internal settings from COMMON_SETTINGS.
    void loadCommonSettings();

    // Trap all changes in here, simplifies debugging.
    void setLanguageId( int aId )       { m_language_id = aId; }

#ifdef KICAD_USE_SENTRY
    void     sentryInit();
    void     sentryPrompt();
    QString sentryCreateUid();
#endif

protected:
    std::unique_ptr<SETTINGS_MANAGER> m_settings_manager;
    std::unique_ptr<BACKGROUND_JOBS_MONITOR> m_background_jobs_monitor;
    std::unique_ptr<NOTIFICATIONS_MANAGER> m_notifications_manager;

    // Check if there is another copy of Kicad running at the same time.
    std::unique_ptr<QObject> m_pgm_checker;

#ifdef KICAD_IPC_API
    std::unique_ptr<API_PLUGIN_MANAGER> m_plugin_manager;
    std::unique_ptr<KICAD_API_SERVER> m_api_server;
#endif

    QString         m_kicad_env;              // The KICAD system environment variable.

    QLocale*        m_locale;
    int             m_language_id;

    bool            m_use_system_pdf_browser;
    QString         m_pdf_browser;            // Filename of the app selected for browsing PDFs.

    QString         m_text_editor;

    KICAD_SINGLETON m_singleton;

#ifdef KICAD_USE_SENTRY
    QString         m_sentry_optin_fn;
    QString         m_sentry_uid_fn;
    QString         m_sentryUid;
#endif

    // argv parameters converted to utf8 form because Qt has opinions.
    // This will return argv as either force converted to ASCII in char* or wchar_t only.
    char** m_argvUtf8;

    int m_argcUtf8;

    QSplashScreen* m_splash;
};


// The global program "get" accessor.
// Implemented in:
//    1. common/single_top.cpp
//    2. kicad/kicad.cpp
KICOMMON_API extern PGM_BASE& Pgm();

// Return a reference that can be nullptr when running a shared lib from a script, not from
// a kicad app.
KICOMMON_API extern PGM_BASE* PgmOrNull();

KICOMMON_API extern void SetPgm( PGM_BASE* pgm );


#endif  // PGM_BASE_H_
