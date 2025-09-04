#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QString>
#include <QCoreApplication>
#include <QStringList>

#include <kiplatform/environment.h>
#include <paths.h>
#include <config.h>
#include <build_version.h>
#include <macros.h>

// lowercase or pretty case depending on platform
#if defined( Q_OS_MAC ) || defined( Q_OS_WIN )
#define KICAD_PATH_STR "KiCad"
#else
#define KICAD_PATH_STR "kicad"
#endif


void PATHS::getUserDocumentPath( QFileInfo& aPath )
{
    QString envPath;

    if( qEnvironmentVariableIsSet( "KICAD_DOCUMENTS_HOME" ) )
    {
        envPath = qgetenv( "KICAD_DOCUMENTS_HOME" );
        aPath = QFileInfo( QDir( envPath ).absolutePath() );
    }
    else
    {
        aPath = QFileInfo( QDir( KIPLATFORM::ENV::GetDocumentsPath() ).absolutePath() );
    }

    QDir dir( aPath.absoluteFilePath() );
    dir.cd( KICAD_PATH_STR );
    dir.cd( GetMajorMinorVersion().toStdString().c_str() );
    aPath = QFileInfo( dir.absolutePath() );
}


QString PATHS::GetUserPluginsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "plugins" );

    return dir.absolutePath();
}


QString PATHS::GetUserScriptingPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "scripting" );

    return dir.absolutePath();
}


QString PATHS::GetUserTemplatesPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "template" );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetDefaultUserSymbolsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "symbols" );

    return dir.absolutePath();
}


QString PATHS::GetDefaultUserFootprintsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "footprints" );

    return dir.absolutePath();
}


QString PATHS::GetDefaultUserDesignBlocksPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "blocks" );

    return dir.absolutePath();
}


QString PATHS::GetDefaultUser3DModelsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "3dmodels" );

    return dir.absolutePath();
}


QString PATHS::GetDefault3rdPartyPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "3rdparty" );

    return dir.absolutePath();
}


QString PATHS::GetDefaultUserProjectsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "projects" );

    return dir.absolutePath();
}


static QString getBuildDirectoryRoot()
{
    // We don't have a perfect way to spot a build directory (e.g. when archived as artifacts in
    // CI) but we can assume that the build directory will have a schemas directory that contains
    // JSON files, as that's one of the things that we use this path for.
    const auto looksLikeBuildDir = []( const QFileInfo& aPath ) -> bool
    {
        const QDir schema_dir( aPath.absoluteFilePath() + QDir::separator() + "schemas" );

        if( !schema_dir.exists() )
            return false;

        QStringList jsonFiles = schema_dir.entryList( QStringList() << "*.json", QDir::Files );
        return !jsonFiles.isEmpty();
    };

    const QString execPath = PATHS::GetExecutablePath();
    QFileInfo     fn( execPath );

    // Climb the directory tree until we find a directory that looks like a build directory
    // Normally we expect to climb one or two levels only.
    QDir dir( fn.absoluteFilePath() );
    while( dir.cdUp() && !looksLikeBuildDir( QFileInfo( dir.absolutePath() ) ) )
    {
        // Continue climbing
    }

    Q_ASSERT_X( !dir.isRoot(), "getBuildDirectoryRoot",
                QString( "Could not find build root directory above %1" ).arg( execPath ).toLocal8Bit().data() );

    return dir.absolutePath();
}


QString PATHS::GetStockDataPath( bool aRespectRunFromBuildDir )
{
    QString path;

    if( aRespectRunFromBuildDir && qEnvironmentVariableIsSet( "KICAD_RUN_FROM_BUILD_DIR" ) )
    {
        // Allow debugging from build dir by placing relevant files/folders in the build root
#if defined( Q_OS_MAC )
        QFileInfo fn( QStandardPaths::findExecutable( QCoreApplication::applicationFilePath() ) );
        QDir dir( fn.absolutePath() );

        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        path = dir.absolutePath();
#elif defined( Q_OS_WIN )
        path = getWindowsKiCadRoot();
#else
        path = getBuildDirectoryRoot();
#endif
    }
    else if( qEnvironmentVariableIsSet( "KICAD_STOCK_DATA_HOME" ) )
    {
        path = qgetenv( "KICAD_STOCK_DATA_HOME" );
        if( !path.isEmpty() )
            return path;
    }
    else
    {
#if defined( Q_OS_MAC )
        path = GetOSXKicadDataDir();
#elif defined( Q_OS_WIN )
        path = getWindowsKiCadRoot() + "share/kicad";
#else
        path = QString::fromUtf8( KICAD_DATA );
#endif
    }

    return path;
}


#ifdef _WIN32

QString PATHS::GetWindowsBaseSharePath()
{
    return getWindowsKiCadRoot() + "share\\";
}

#endif


QString PATHS::GetStockEDALibraryPath()
{
    QString path;

#if defined( Q_OS_MAC )
    path = GetOSXKicadMachineDataDir();
#elif defined( Q_OS_WIN )
    path = GetStockDataPath( false );
#else
    path = QString::fromUtf8( KICAD_LIBRARY_DATA );
#endif

    return path;
}


QString PATHS::GetStockSymbolsPath()
{
    QString path;

    path = GetStockEDALibraryPath() + "/symbols";

    return path;
}


QString PATHS::GetStockFootprintsPath()
{
    QString path;

    path = GetStockEDALibraryPath() + "/footprints";

    return path;
}


QString PATHS::GetStockDesignBlocksPath()
{
    QString path;

    path = GetStockEDALibraryPath() + "/blocks";

    return path;
}


QString PATHS::GetStock3dmodelsPath()
{
    QString path;

    path = GetStockEDALibraryPath() + "/3dmodels";

    return path;
}


QString PATHS::GetStockScriptingPath()
{
    QString path;

    path = GetStockDataPath() + "/scripting";

    return path;
}


QString PATHS::GetStockTemplatesPath()
{
    QString path;

    path = GetStockEDALibraryPath() + "/template";

    return path;
}


QString PATHS::GetLocaleDataPath()
{
    QString path;

    path = GetStockDataPath() + "/internat";

    return path;
}


QString PATHS::GetStockPluginsPath()
{
    QDir dir;

#if defined( Q_OS_WIN )
    dir.setPath( GetExecutablePath() );
    dir.cd( "scripting" );
#else
    dir.setPath( PATHS::GetStockDataPath( false ) );
#endif
    dir.cd( "plugins" );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetStockPlugins3DPath()
{
    QDir dir;

#if defined( Q_OS_WIN )
    if( qEnvironmentVariableIsSet( "KICAD_RUN_FROM_BUILD_DIR" ) )
    {
        dir.setPath( getWindowsKiCadRoot() );
    }
    else
    {
        dir.setPath( GetExecutablePath() );
    }

    dir.cd( "plugins" );
#elif defined( Q_OS_MAC )
    QString pluginsDir = QStandardPaths::locate( QStandardPaths::DataLocation, "plugins", QStandardPaths::LocateDirectory );
    dir.setPath( pluginsDir );

    // This must be mapped to main bundle for everything but kicad.app
    QStringList pathParts = dir.absolutePath().split( '/' );

    // Check if we are the main kicad binary.  in this case, the path will be
    //     /path/to/bundlename.app/Contents/PlugIns
    // If we are an aux binary, the path will be something like
    //     /path/to/bundlename.app/Contents/Applications/<standalone>.app/Contents/PlugIns
    if( pathParts.count() >= 6 &&
        pathParts[pathParts.count() - 4] == "Applications" &&
        pathParts[pathParts.count() - 6].toLower().endsWith( "app" ) )
    {
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        dir.cdUp();
        dir.cd( "PlugIns" );
    }
#else
    // KICAD_PLUGINDIR = CMAKE_INSTALL_FULL_LIBDIR path is the absolute path
    // corresponding to the install path used for constructing KICAD_USER_PLUGIN
    QString tfname = QString::fromUtf8( KICAD_PLUGINDIR );
    dir.setPath( tfname );
    dir.cd( "kicad" );
    dir.cd( "plugins" );
#endif

    dir.cd( "3d" );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetStockDemosPath()
{
    QDir dir;

    dir.setPath( PATHS::GetStockDataPath( false ) );
    dir.cd( "demos" );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetUserCachePath()
{
    QString   envPath;
    QDir dir;

    dir.setPath( KIPLATFORM::ENV::GetUserCachePath() );

    // Use KICAD_CACHE_HOME to allow the user to force a specific cache path.
    if( qEnvironmentVariableIsSet( "KICAD_CACHE_HOME" ) )
    {
        envPath = qgetenv( "KICAD_CACHE_HOME" );
        if( !envPath.isEmpty() )
        {
            // Override the assignment above with KICAD_CACHE_HOME
            dir.setPath( envPath );
        }
    }

    dir.cd( KICAD_PATH_STR );
    dir.cd( GetMajorMinorVersion().toStdString().c_str() );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetDocumentationPath()
{
    QString path;

#if defined( Q_OS_MAC )
    path = GetOSXKicadDataDir();
#elif defined( Q_OS_WIN )
    path = getWindowsKiCadRoot() + "share/doc/kicad";
#else
    path = QString::fromUtf8( KICAD_DOCS );
#endif

    return path;
}


QString PATHS::GetInstanceCheckerPath()
{
    QDir dir( QStandardPaths::writableLocation( QStandardPaths::TempLocation ) );
    dir.cd( "org.kicad.kicad" );
    dir.cd( "instances" );
    return dir.absolutePath() + QDir::separator();
}


QString PATHS::GetLogsPath()
{
    QFileInfo tmp;
    getUserDocumentPath( tmp );

    QDir dir( tmp.absoluteFilePath() );
    dir.cd( "logs" );

    return dir.absolutePath();
}


bool PATHS::EnsurePathExists( const QString& aPath, bool aPathToFile )
{
    QString   pathString = aPath;
    if( !aPathToFile )
    {
        // ensures the path is treated fully as directory
        pathString += QDir::separator();
    }

    QFileInfo path( pathString );
    QDir dir( path.absolutePath() );

    if( aPathToFile )
        dir = QDir( path.absoluteDir() );

    if( !dir.exists() )
    {
        if( !dir.mkpath( dir.absolutePath() ) )
        {
            return false;
        }
    }

    return true;
}


void PATHS::EnsureUserPathsExist()
{
    EnsurePathExists( GetUserCachePath() );
    EnsurePathExists( GetUserPluginsPath() );
    EnsurePathExists( GetUserScriptingPath() );
    EnsurePathExists( GetUserTemplatesPath() );
    EnsurePathExists( GetDefaultUserProjectsPath() );
    EnsurePathExists( GetDefaultUserSymbolsPath() );
    EnsurePathExists( GetDefaultUserFootprintsPath() );
    EnsurePathExists( GetDefaultUser3DModelsPath() );
    EnsurePathExists( GetDefault3rdPartyPath() );
}


#ifdef Q_OS_MAC
QString PATHS::GetOSXKicadUserDataDir()
{
    // According to Qt documentation for GenericDataLocation:
    // Mac: ~/Library/Application Support/appname
    QDir udir( QStandardPaths::writableLocation( QStandardPaths::GenericDataLocation ) );

    // Since appname is different if started via launcher or standalone binary
    // map all to "kicad" here
    udir.cdUp();
    udir.cd( "kicad" );

    return udir.absolutePath();
}


QString PATHS::GetOSXKicadMachineDataDir()
{
    // 6.0 forward:  Same as the main data dir
    return GetOSXKicadDataDir();
}


QString PATHS::GetOSXKicadDataDir()
{
    // According to Qt documentation for GenericDataLocation:
    // Mac: appname.app/Contents/SharedSupport bundle subdirectory
    QDir ddir( QStandardPaths::locate( QStandardPaths::DataLocation, "", QStandardPaths::LocateDirectory ) );

    // This must be mapped to main bundle for everything but kicad.app
    QStringList pathParts = ddir.absolutePath().split( '/' );

    // Check if we are the main kicad binary.  in this case, the path will be
    //     /path/to/bundlename.app/Contents/SharedSupport
    // If we are an aux binary, the path will be something like
    //     /path/to/bundlename.app/Contents/Applications/<standalone>.app/Contents/SharedSupport
    if( pathParts.count() >= 6 &&
        pathParts[pathParts.count() - 4] == "Applications" &&
        pathParts[pathParts.count() - 6].toLower().endsWith( "app" ) )
    {
        ddir.cdUp();
        ddir.cdUp();
        ddir.cdUp();
        ddir.cdUp();
        ddir.cd( "SharedSupport" );
    }

    return ddir.absolutePath();
}
#endif


#ifdef _WIN32
QString PATHS::GetWindowsFontConfigDir()
{
    QDir dir( getWindowsKiCadRoot() );
    dir.cd( "etc" );
    dir.cd( "fonts" );

    return dir.absolutePath() + QDir::separator();
}


QString PATHS::getWindowsKiCadRoot()
{
    QDir root( GetExecutablePath() + "/../" );
    root.makeAbsolute();

    return root.absolutePath() + QDir::separator();
}
#endif


QString PATHS::GetUserSettingsPath()
{
    static QString user_settings_path;

    if( user_settings_path.isEmpty() )
        user_settings_path = CalculateUserSettingsPath();

    return user_settings_path;
}


QString PATHS::CalculateUserSettingsPath( bool aIncludeVer, bool aUseEnv )
{
    QDir cfgpath;

    QString envstr;
    if( aUseEnv && qEnvironmentVariableIsSet( "KICAD_CONFIG_HOME" ) )
    {
        envstr = qgetenv( "KICAD_CONFIG_HOME" );
        if( !envstr.isEmpty() )
        {
            // Override the assignment above with KICAD_CONFIG_HOME
            cfgpath.setPath( envstr );
        }
    }
    else
    {
        cfgpath.setPath( KIPLATFORM::ENV::GetUserConfigPath() );

        cfgpath.cd( TO_STR( KICAD_CONFIG_DIR ) );
    }

    if( aIncludeVer )
        cfgpath.cd( GetMajorMinorVersion().toStdString().c_str() );

    return cfgpath.absolutePath();
}


const QString& PATHS::GetExecutablePath()
{
    static QString exe_path;

    if( exe_path.isEmpty() )
    {
        QString bin_dir = QCoreApplication::applicationDirPath();

#ifdef Q_OS_MAC
        // On OSX applicationDirPath() will always point to main
        // bundle directory, e.g., /Applications/kicad.app/

        QFileInfo fn( bin_dir );
        QDir dir( fn.absoluteFilePath() );
        
        // Resolve symlinks if needed (simplified approach)
        QString baseName = QFileInfo( QCoreApplication::applicationFilePath() ).baseName();
        
        if( baseName == "kicad" || baseName == "kicad-cli" )
        {
            // kicad launcher, so just remove the Contents/MacOS part
            dir.cdUp();
            dir.cdUp();
        }
        else
        {
            // standalone binaries live in Contents/Applications/<standalone>.app/Contents/MacOS
            dir.cdUp();
            dir.cdUp();
            dir.cdUp();
            dir.cdUp();
            dir.cdUp();
        }

        bin_dir = dir.absolutePath() + "/";
#else
        // Use unix notation for paths. I am not sure this is a good idea,
        // but it simplifies compatibility between Windows and Unices.
        // However it is a potential problem in path handling under Windows.
        bin_dir.replace( "\\", "/" );

        // Remove file name form command line:
        while( bin_dir.endsWith( '/' ) == false && !bin_dir.isEmpty() )
            bin_dir.chop( 1 );
        
        if( !bin_dir.endsWith( '/' ) )
            bin_dir += "/";
#endif
        exe_path = bin_dir;
    }

    return exe_path;
}
