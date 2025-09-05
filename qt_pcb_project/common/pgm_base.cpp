#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QStandardPaths>
#include <QFileDialog>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include <QToolTip>

#include <advanced_config.h>
#include <background_jobs_monitor.h>
#include <bitmaps.h>
#include <build_version.h>
#include <common.h>
#include <confirm.h>
#include <core/arraydim.h>
#include <id.h>
#include <kicad_curl/kicad_curl.h>
#include <kiplatform/policy.h>
#include <macros.h>
#include <notifications_manager.h>
#include <paths.h>
#include <pgm_base.h>
#include <policy_keys.h>
#include <settings/common_settings.h>
#include <settings/settings_manager.h>
#include <string_utils.h>
#include <systemdirsappend.h>
#include <thread_pool.h>
#include <trace_helpers.h>

#include <widgets/wx_splash.h>
#include <i18n_utility.h>

#ifdef KICAD_USE_SENTRY
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <sentry.h>
#include <build_version.h>
#endif

#ifdef KICAD_IPC_API
#include <api/api_plugin_manager.h>
#include <api/api_server.h>
#include <python_manager.h>
#endif

// For static initialization, temporarily disable translation
#undef _
#define _(s) s
LANGUAGE_DESCR LanguagesList[] =
{
    { QLocale::AnyLanguage,    ID_LANGUAGE_DEFAULT,    _( "Default" ),    false },
    { QLocale::Czech,      ID_LANGUAGE_CZECH,      "Čeština",  true },
    { QLocale::German,     ID_LANGUAGE_GERMAN,     "Deutsch",  true },
    { QLocale::Greek,      ID_LANGUAGE_GREEK,      "Ελληνικά", true },
    { QLocale::English,    ID_LANGUAGE_ENGLISH,    "English",  true },
    { QLocale::Spanish,    ID_LANGUAGE_SPANISH,    "Español",  true },
    { QLocale::Spanish, ID_LANGUAGE_SPANISH_MEXICAN,
      "Español (Latinoamericano)",  true },
    { QLocale::French,     ID_LANGUAGE_FRENCH,     "Français", true },
    { QLocale::Korean,     ID_LANGUAGE_KOREAN,     "한국어",       true },
    { QLocale::Italian,    ID_LANGUAGE_ITALIAN,    "Italiano", true },
    { QLocale::Dutch,      ID_LANGUAGE_DUTCH,      "Nederlands", true },
    { QLocale::Japanese,   ID_LANGUAGE_JAPANESE,   "日本語",    true },
    { QLocale::Thai,       ID_LANGUAGE_THAI,       "ภาษาไทย",    true },
    { QLocale::Polish,     ID_LANGUAGE_POLISH,     "Polski",   true },
    { QLocale::Portuguese, ID_LANGUAGE_PORTUGUESE, "Português",true },
    { QLocale::Portuguese, ID_LANGUAGE_PORTUGUESE_BRAZILIAN,
      "Português (Brasil)", true },
    { QLocale::Russian,    ID_LANGUAGE_RUSSIAN,    "Русский",  true },
    { QLocale::Finnish,    ID_LANGUAGE_FINNISH,    "Suomi",    true },
    { QLocale::Swedish,    ID_LANGUAGE_SWEDISH,    "Svenska",  true },
    { QLocale::Ukrainian,  ID_LANGUAGE_UKRANIAN,   "Українська",   true },
    { QLocale::Chinese, ID_LANGUAGE_CHINESE_SIMPLIFIED,
            "简体中文", true },
    { QLocale::Chinese, ID_LANGUAGE_CHINESE_TRADITIONAL,
            "繁體中文", true },
    { QLocale::AnyLanguage, 0, "", false }         // Sentinel
};
// Re-enable translation after static initialization
#undef _


PGM_BASE::PGM_BASE()
{
    m_locale = QLocale::AnyLanguage;
    m_translator = nullptr;
    m_Printing = false;
    m_Quitting = false;
    m_argcUtf8 = 0;
    m_argvUtf8 = nullptr;
    m_splash = nullptr;
    m_PropertyGridInitialized = false;

    setLanguageId( QLocale::AnyLanguage );

    ForceSystemPdfBrowser( false );
}


PGM_BASE::~PGM_BASE()
{
    HideSplash();
    Destroy();

    for( int n = 0; n < m_argcUtf8; n++ )
    {
        free( m_argvUtf8[n] );
    }

    delete[] m_argvUtf8;

    delete m_translator;
    m_translator = nullptr;
}


void PGM_BASE::Destroy()
{
    KICAD_CURL::Cleanup();

#ifdef KICAD_USE_SENTRY
    sentry_close();
#endif

    m_pgm_checker.reset();
}


QApplication& PGM_BASE::App()
{
    Q_ASSERT( qApp );
    return *qApp;
}


void PGM_BASE::SetTextEditor( const QString& aFileName )
{
    m_text_editor = aFileName;
    GetCommonSettings()->m_System.text_editor = aFileName;
}


const QString& PGM_BASE::GetTextEditor( bool aCanShowFileChooser )
{
    QString editorname = m_text_editor;

    if( editorname.isEmpty() )
    {
        editorname = qEnvironmentVariable( "EDITOR" );
        if( editorname.isEmpty() )
        {
#ifdef __APPLE__
            editorname = "/usr/bin/open -e";
#elif defined(__linux__)
            editorname =  "/usr/bin/xdg-open";
#endif
        }
    }

    if( editorname.isEmpty() && aCanShowFileChooser )
    {
        DisplayInfoMessage( nullptr, _( "No default editor found, you must choose one." ) );

        editorname = AskUserForPreferredEditor();
    }

    if( !editorname.isEmpty() )
        SetTextEditor( editorname );

    return m_text_editor;
}


const QString PGM_BASE::AskUserForPreferredEditor( const QString& aDefaultEditor )
{
#ifdef _WIN32
    QString mask( _( "Executable file" ) + " (*.exe)" );
#else
    QString mask( _( "Executable file" ) + " (*)" );
#endif

    QFileInfo info( aDefaultEditor );
    QString path = info.absolutePath();
    QString name = info.baseName();
    QString ext = info.completeSuffix();

    return QFileDialog::getOpenFileName( nullptr, _( "Select Preferred Editor" ), 
                                        path + "/" + name + "." + ext,
                                        mask );
}


#ifdef KICAD_USE_SENTRY
bool PGM_BASE::IsSentryOptedIn()
{
    KIPLATFORM::POLICY::PBOOL policyState =
            KIPLATFORM::POLICY::GetPolicyBool( POLICY_KEY_DATACOLLECTION );
    if( policyState != KIPLATFORM::POLICY::PBOOL::NOT_CONFIGURED )
    {
        return policyState == KIPLATFORM::POLICY::PBOOL::ENABLED;
    }

    return m_sentry_optin_fn.exists();
}


void PGM_BASE::SetSentryOptIn( bool aOptIn )
{
    if( aOptIn )
    {
        if( !m_sentry_uid_fn.exists() )
        {
            sentryCreateUid();
        }

        if( !m_sentry_optin_fn.exists() )
        {
            QFile sentryInitFile( m_sentry_optin_fn.absoluteFilePath() );
            if( sentryInitFile.open( QIODevice::WriteOnly ) )
            {
                QTextStream stream( &sentryInitFile );
                stream << "";
                sentryInitFile.close();
            }
        }
    }
    else
    {
        if( m_sentry_optin_fn.exists() )
        {
            QFile::remove( m_sentry_optin_fn.absoluteFilePath() );
            sentry_close();
        }
    }
}


QString PGM_BASE::sentryCreateUid()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    QString userGuid = QString::fromStdString( boost::uuids::to_string( uuid ) );

    QFile sentryInitFile( m_sentry_uid_fn.absoluteFilePath() );
    if( sentryInitFile.open( QIODevice::WriteOnly ) )
    {
        QTextStream stream( &sentryInitFile );
        stream << userGuid;
        sentryInitFile.close();
    }

    return userGuid;
}


void PGM_BASE::ResetSentryId()
{
    m_sentryUid = sentryCreateUid();
}


const QString& PGM_BASE::GetSentryId()
{
    return m_sentryUid;
}


void PGM_BASE::sentryInit()
{
    m_sentry_optin_fn = QFileInfo( PATHS::GetUserCachePath() + "/sentry-opt-in" );
    m_sentry_uid_fn = QFileInfo( PATHS::GetUserCachePath() + "/sentry-uid" );

    if( IsSentryOptedIn() )
    {
        QFile sentryInitFile( m_sentry_uid_fn.absoluteFilePath() );
        if( sentryInitFile.open( QIODevice::ReadOnly ) )
        {
            QTextStream stream( &sentryInitFile );
            m_sentryUid = stream.readAll();
            sentryInitFile.close();
        }

        if( m_sentryUid.isEmpty() || m_sentryUid.length() != 36 )
        {
            m_sentryUid = sentryCreateUid();
        }

        sentry_options_t* options = sentry_options_new();

#ifndef KICAD_SENTRY_DSN
#   error "Project configuration error, missing KICAD_SENTRY_DSN"
#endif

        sentry_options_set_dsn( options, KICAD_SENTRY_DSN );

        QDir tmp( PATHS::GetUserCachePath() );
        if( !tmp.exists( "sentry" ) )
            tmp.mkpath( "sentry" );
        tmp.cd( "sentry" );

#ifdef _WIN32
        sentry_options_set_database_pathw( options, reinterpret_cast<const wchar_t*>(tmp.absolutePath().utf16()) );
#else
        sentry_options_set_database_path( options, tmp.absolutePath().toUtf8().constData() );
#endif
        sentry_options_set_symbolize_stacktraces( options, true );
        sentry_options_set_auto_session_tracking( options, false );

        sentry_options_set_release( options, GetCommitHash().toStdString().c_str() );

        sentry_options_set_environment( options, GetMajorMinorVersion().c_str() );

        sentry_init( options );

        sentry_value_t user = sentry_value_new_object();
        sentry_value_set_by_key( user, "id", sentry_value_new_string( m_sentryUid.toUtf8().constData() ) );
        sentry_set_user( user );

        sentry_set_tag( "kicad.version", GetBuildVersion().toStdString().c_str() );
    }
}


void PGM_BASE::sentryPrompt()
{
    if( !IsGUI() )
        return;

    KIPLATFORM::POLICY::PBOOL policyState = KIPLATFORM::POLICY::GetPolicyBool( POLICY_KEY_DATACOLLECTION );

    if( policyState == KIPLATFORM::POLICY::PBOOL::NOT_CONFIGURED
            && !m_settings_manager->GetCommonSettings()->m_DoNotShowAgain.data_collection_prompt )
    {
        QMessageBox optIn;
        optIn.setWindowTitle( _( "Data Collection Opt In" ) );
        optIn.setText( _( "KiCad can anonymously report crashes and special event data to developers in order to "
                   "aid identifying critical bugs and help profile functionality to guide improvements. \n"
                   "If you choose to voluntarily participate, KiCad will automatically send said reports "
                   "when crashes or events occur. \n"
                   "Your design files such as schematic and PCB are not shared in this process." ) );
        optIn.setStandardButtons( QMessageBox::Yes | QMessageBox::No );
        optIn.setButtonText( QMessageBox::Yes, _( "Opt In" ) );
        optIn.setButtonText( QMessageBox::No, _( "Decline" ) );
        int result = optIn.exec();

        if( result == QMessageBox::Yes )
        {
            SetSentryOptIn( true );
            sentryInit();
        }
        else
        {
            SetSentryOptIn( false );
        }

        m_settings_manager->GetCommonSettings()->m_DoNotShowAgain.data_collection_prompt = true;
    }
}
#endif


void PGM_BASE::BuildArgvUtf8()
{
    QStringList argList = QCoreApplication::arguments();
    m_argcUtf8 = argList.size();

    m_argvUtf8 = new char*[m_argcUtf8 + 1];
    for( int n = 0; n < m_argcUtf8; n++ )
    {
        QByteArray utf8 = argList[n].toUtf8();
        m_argvUtf8[n] = strdup( utf8.constData() );
    }

    m_argvUtf8[m_argcUtf8] = NULL;
}


void PGM_BASE::ShowSplash()
{
    if( m_splash )
        return;

    m_splash = new WX_SPLASH( KiBitmap( BITMAPS::splash ), Qt::SplashScreen, 0, nullptr );
    QCoreApplication::processEvents();
}


void PGM_BASE::HideSplash()
{
    if( !m_splash )
        return;

    m_splash->close();
    delete m_splash;
    m_splash = nullptr;
}


bool PGM_BASE::InitPgm( bool aHeadless, bool aSkipPyInit, bool aIsUnitTest )
{
#if defined( __APPLE__ )
    QLocale::setDefault( QLocale::system() );
#endif

    PATHS::EnsureUserPathsExist();

    KICAD_CURL::Init();

#ifdef KICAD_USE_SENTRY
    sentryInit();
#endif

    m_singleton.Init();

    QString pgm_name;

    if( QCoreApplication::arguments().isEmpty() )
        pgm_name = "kicad";
    else
        pgm_name = QFileInfo( QCoreApplication::arguments()[0] ).baseName().toLower();

#ifdef KICAD_USE_SENTRY
    sentry_set_tag( "kicad.app", pgm_name.toUtf8().constData() );
#endif

#ifndef _WIN32
    if( qEnvironmentVariable( "HOME" ).isEmpty() )
    {
        DisplayErrorMessage( nullptr, _( "Environment variable HOME is empty.  "
                                         "Unable to continue." ) );
        return false;
    }
#endif

    QString instanceCheckerDir = PATHS::GetInstanceCheckerPath();
    PATHS::EnsurePathExists( instanceCheckerDir );
    QFile::setPermissions( instanceCheckerDir,
                          QFile::ReadUser | QFile::WriteUser | QFile::ExeUser |
                          QFile::ReadGroup | QFile::WriteGroup | QFile::ExeGroup |
                          QFile::ReadOther | QFile::WriteOther | QFile::ExeOther );

    QString instanceCheckerName = QString( "%1-%2" ).arg( pgm_name )
                                                    .arg( GetMajorMinorVersion() );

    m_pgm_checker = std::make_unique<QSharedMemory>( instanceCheckerName );
    if( !m_pgm_checker->create( 1 ) )
    {
        if( m_pgm_checker->error() == QSharedMemory::AlreadyExists )
        {
            m_pgm_checker->attach();
        }
    }

    QString kicadEnv = qEnvironmentVariable( "KICAD" );
    bool isDefined = !kicadEnv.isEmpty();

    if( isDefined )
    {
        m_kicad_env = kicadEnv;
        m_kicad_env.replace( WIN_STRING_DIR_SEP, UNIX_STRING_DIR_SEP );

        if( !m_kicad_env.isEmpty() && !m_kicad_env.endsWith( '/' ) )
            m_kicad_env += UNIX_STRING_DIR_SEP;
    }

    QCoreApplication::setOrganizationName( "KiCad" );
    QCoreApplication::setApplicationName( pgm_name );

    QString tmp;
    SetLanguagePath();
    SetDefaultLanguage( tmp );

#ifdef _MSC_VER
    if( qEnvironmentVariable( "FONTCONFIG_PATH" ).isEmpty() )
    {
        qputenv( "FONTCONFIG_PATH", PATHS::GetWindowsFontConfigDir().toUtf8() );
    }
#endif

    m_settings_manager = std::make_unique<SETTINGS_MANAGER>( aHeadless );
    m_background_jobs_monitor = nullptr;
    m_notifications_manager = nullptr;

#ifdef KICAD_IPC_API
    m_plugin_manager = std::make_unique<API_PLUGIN_MANAGER>( &App() );
#endif

    if( aIsUnitTest )
        return false;

    if( !m_settings_manager->IsOK() )
        return false;

    COMMON_SETTINGS* commonSettings = GetCommonSettings();
    commonSettings->InitializeEnvironment();

    m_settings_manager->ReloadColorSettings();

    GetSettingsManager().Load( commonSettings );

#ifdef KICAD_IPC_API
    if( commonSettings->m_Api.python_interpreter.isEmpty() )
        commonSettings->m_Api.python_interpreter = PYTHON_MANAGER::FindPythonInterpreter();
#endif

    SetLanguage( tmp, true );

    WarnUserIfOperatingSystemUnsupported();

    loadCommonSettings();

#ifdef KICAD_USE_SENTRY
    sentryPrompt();
#endif

    ReadPdfBrowserInfos();

    GetSettingsManager().LoadProject( "" );

#ifdef KICAD_IPC_API
    if( commonSettings->m_Api.enable_server )
        m_plugin_manager->ReloadPlugins();
#endif

    if( !aHeadless )
    {
        QToolTip::hideText();
    }

    if( ADVANCED_CFG::GetCfg().m_UpdateUIEventInterval != 0 )
    {
        // Qt equivalent would be setting timer intervals for specific widgets
    }

    if( !aHeadless )
        ShowSplash();

    return true;
}


void PGM_BASE::loadCommonSettings()
{
    m_text_editor = GetCommonSettings()->m_System.text_editor;

    for( const std::pair<QString, ENV_VAR_ITEM> it : GetCommonSettings()->m_Env.vars )
    {
        qDebug() << QString( "PGM_BASE::loadSettings: Found entry %1 = %2" )
                    .arg( it.first, it.second.GetValue() );

        if( it.first == PROJECT_VAR_NAME )
            continue;

        if( it.first.isEmpty() )
            continue;

        if( it.second.GetDefinedExternally() )
            continue;

        SetLocalEnvVariable( it.first, it.second.GetValue() );
    }
}


void PGM_BASE::SaveCommonSettings()
{
    if( GetCommonSettings() )
        GetCommonSettings()->m_System.working_dir = QDir::currentPath();
}


COMMON_SETTINGS* PGM_BASE::GetCommonSettings() const
{
    return m_settings_manager ? m_settings_manager->GetCommonSettings() : nullptr;
}


bool PGM_BASE::SetLanguage( QString& aErrMsg, bool first_time )
{
    if( first_time )
    {
        setLanguageId( QLocale::AnyLanguage );

        QString languageSel = GetCommonSettings()->m_System.language;

        for( unsigned ii = 0; LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
        {
            if( LanguagesList[ii].m_Lang_Label == languageSel )
            {
                setLanguageId( LanguagesList[ii].m_WX_Lang_Identifier );
                break;
            }
        }
    }

    QString dictionaryName( "kicad" );

    delete m_translator;
    m_translator = new QTranslator;

    QLocale locale( m_language_id );
    if( !m_translator->load( locale, dictionaryName, "_", GetLanguagePath() ) )
    {
        qDebug() << "This language is not supported by the system.";

        setLanguageId( QLocale::AnyLanguage );
        delete m_translator;

        m_translator = new QTranslator;
        QLocale::setDefault( QLocale::system() );

        aErrMsg = _( "This language is not supported by the operating system." );
        return false;
    }
    else if( !first_time )
    {
        qDebug() << QString( "Search for dictionary %1.qm in %2" )
                    .arg( dictionaryName, GetLanguagePath() );
    }

    if( !first_time )
    {
        QString languageSel;

        for( unsigned ii = 0;  LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
        {
            if( LanguagesList[ii].m_WX_Lang_Identifier == m_language_id )
            {
                languageSel = LanguagesList[ii].m_Lang_Label;
                break;
            }
        }

        COMMON_SETTINGS* cfg = GetCommonSettings();
        cfg->m_System.language = languageSel;
        cfg->SaveToFile( GetSettingsManager().GetPathForSettingsFile( cfg ) );
    }

    QCoreApplication::installTranslator( m_translator );

    return true;
}


bool PGM_BASE::SetDefaultLanguage( QString& aErrMsg )
{
    setLanguageId( QLocale::AnyLanguage );

    QString dictionaryName( "kicad" );

    delete m_translator;
    m_translator = new QTranslator;
    QLocale::setDefault( QLocale::system() );

    if( !m_translator->load( QLocale::system(), dictionaryName, "_", GetLanguagePath() ) 
        && m_language_id != QLocale::English )
    {
        qDebug() << QString( "Unable to load dictionary %1.qm in %2" )
                    .arg( dictionaryName, GetLanguagePath() );

        setLanguageId( QLocale::AnyLanguage );
        delete m_translator;

        m_translator = new QTranslator;
        QLocale::setDefault( QLocale::system() );

        aErrMsg = _( "The KiCad language file for this language is not installed." );
        return false;
    }

    QCoreApplication::installTranslator( m_translator );
    return true;
}


void PGM_BASE::SetLanguageIdentifier( int menu_id )
{
    qDebug() << QString( "Select language ID %1 from %2 possible languages." )
                .arg( menu_id ).arg( (int)arrayDim( LanguagesList )-1 );

    for( unsigned ii = 0;  LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
    {
        if( menu_id == LanguagesList[ii].m_KI_Lang_Identifier )
        {
            setLanguageId( LanguagesList[ii].m_WX_Lang_Identifier );
            break;
        }
    }
}


QString PGM_BASE::GetLanguageTag()
{
    QLocale locale( m_language_id );
    QString str = locale.name();
    str.replace( "_", "-" );
    return str;
}


void PGM_BASE::SetLanguagePath()
{
#ifdef _MSC_VER
    QDir::addSearchPath( "translations", PATHS::GetWindowsBaseSharePath() + "/locale" );
#endif
    QDir::addSearchPath( "translations", PATHS::GetLocaleDataPath() );

    if( !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
    {
        QFileInfo fi( Pgm().GetExecutablePath() );
        QDir dir = fi.dir();
        dir.cdUp();
        dir.cd( "translation" );
        QDir::addSearchPath( "translations", dir.absolutePath() );
    }
}


QString PGM_BASE::GetLanguagePath()
{
#ifdef _MSC_VER
    return PATHS::GetWindowsBaseSharePath() + "/locale";
#endif
    return PATHS::GetLocaleDataPath();
}


bool PGM_BASE::SetLocalEnvVariable( const QString& aName, const QString& aValue )
{
    QString env;

    if( aName.isEmpty() )
    {
        qDebug() << QString( "PGM_BASE::SetLocalEnvVariable: Attempt to set empty variable to "
                         "value %1" ).arg( aValue );
        return false;
    }

    env = qEnvironmentVariable( aName.toUtf8() );
    if( !env.isNull() )
    {
        qDebug() << QString( "PGM_BASE::SetLocalEnvVariable: Environment variable %1 already set "
                         "to %2" ).arg( aName, env );
        return env == aValue;
    }

    qDebug() << QString( "PGM_BASE::SetLocalEnvVariable: Setting local environment variable %1 to %2" )
                .arg( aName, aValue );

    return qputenv( aName.toUtf8(), aValue.toUtf8() );
}


void PGM_BASE::SetLocalEnvVariables()
{
    for( const std::pair<QString, ENV_VAR_ITEM> m_local_env_var : GetCommonSettings()->m_Env.vars )
    {
        qDebug() << QString( "PGM_BASE::SetLocalEnvVariables: Setting local environment variable %1 "
                         "to %2" ).arg( m_local_env_var.first,
                                       m_local_env_var.second.GetValue() );
        qputenv( m_local_env_var.first.toUtf8(), m_local_env_var.second.GetValue().toUtf8() );
    }
}


ENV_VAR_MAP& PGM_BASE::GetLocalEnvVariables() const
{
    return GetCommonSettings()->m_Env.vars;
}


bool PGM_BASE::IsGUI()
{
    if( !qApp )
        return false;

    return qobject_cast<QApplication*>( qApp ) != nullptr;
}


void PGM_BASE::HandleException( std::exception_ptr aPtr )
{
    try
    {
        if( aPtr )
            std::rethrow_exception( aPtr );
    }
    catch( const IO_ERROR& ioe )
    {
        qCritical() << ioe.What();
    }
    catch( const std::exception& e )
    {
#ifdef KICAD_USE_SENTRY
        if( IsSentryOptedIn() )
        {
            sentry_value_t exc = sentry_value_new_exception( "exception", e.what() );
            sentry_value_set_stacktrace( exc, NULL, 0 );

            sentry_value_t sentryEvent = sentry_value_new_event();
            sentry_event_add_exception( sentryEvent, exc );
            sentry_capture_event( sentryEvent );
        }
#endif

        qCritical() << QString( "Unhandled exception class: %1  what: %2" )
                       .arg( typeid( e ).name(), e.what() );
    }
    catch( ... )
    {
        qCritical() << "Unhandled exception of unknown type";
    }
}


#ifdef KICAD_USE_SENTRY
struct SENTRY_ASSERT_CACHE_KEY
{
    QString file;
    int      line;
    QString func;
    QString cond;
    QString msg;
};


bool operator<( const SENTRY_ASSERT_CACHE_KEY& aKey1, const SENTRY_ASSERT_CACHE_KEY& aKey2 )
{
    return aKey1.file < aKey2.file ||
        aKey1.line < aKey2.line ||
        aKey1.func < aKey2.func ||
        aKey1.cond < aKey2.cond ||
        aKey1.msg < aKey2.msg;
}
#endif


void PGM_BASE::HandleAssert( const QString& aFile, int aLine, const QString& aFunc,
                             const QString& aCond, const QString& aMsg )
{
    QString assertStr;

    if( !aMsg.isEmpty() )
    {
        assertStr = QString( "Assertion failed at %1:%2 in %3: %4 - %5" )
                    .arg( aFile ).arg( aLine ).arg( aFunc, aCond, aMsg );
    }
    else
    {
        assertStr = QString( "Assertion failed at %1:%2 in %3: %4" )
                    .arg( aFile ).arg( aLine ).arg( aFunc, aCond );
    }

#ifndef NDEBUG
    qCritical() << assertStr;
#endif

#ifdef KICAD_USE_SENTRY
    if( IsSentryOptedIn() )
    {
        static std::set<SENTRY_ASSERT_CACHE_KEY> assertCache;

        SENTRY_ASSERT_CACHE_KEY key = { aFile, aLine, aFunc, aCond };

        if( assertCache.find( key ) == assertCache.end() )
        {
            sentry_value_t exc = sentry_value_new_exception( "assert", assertStr.toUtf8().constData() );
            sentry_value_set_stacktrace( exc, NULL, 0 );

            sentry_value_t sentryEvent = sentry_value_new_event();
            sentry_event_add_exception( sentryEvent, exc );
            sentry_capture_event( sentryEvent );
            assertCache.insert( key );
        }
    }
#endif
}


const QString& PGM_BASE::GetExecutablePath() const
{
    return PATHS::GetExecutablePath();
}


void PGM_BASE::ReadPdfBrowserInfos()
{
    SetPdfBrowserName( GetCommonSettings()->m_System.pdf_viewer_name );
    m_use_system_pdf_browser = GetCommonSettings()->m_System.use_system_pdf_viewer;
}


void PGM_BASE::WritePdfBrowserInfos()
{
    GetCommonSettings()->m_System.pdf_viewer_name = GetPdfBrowserName();
    GetCommonSettings()->m_System.use_system_pdf_viewer = m_use_system_pdf_browser;
}


static PGM_BASE* process;


PGM_BASE& Pgm()
{
    Q_ASSERT( process );
    return *process;
}


PGM_BASE* PgmOrNull()
{
    return process;
}


void SetPgm( PGM_BASE* pgm )
{
    process = pgm;
}