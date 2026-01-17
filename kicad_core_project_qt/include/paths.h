// QT_TRANSFORMATION_COMPLETED

#ifndef PATHS_H
#define PATHS_H

#include <kicommon.h>
#include <QString>
#include <QDir>

#define UNIX_STRING_DIR_SEP "/"
#define WIN_STRING_DIR_SEP "\\"

class KICOMMON_API PATHS
{
public:

    static QString GetUserScriptingPath();

    static QString GetUserTemplatesPath();

    static QString GetUserPluginsPath();

    static QString GetDefaultUserProjectsPath();

    static QString GetDefaultUserSymbolsPath();

    static QString GetDefaultUserFootprintsPath();

    static QString GetDefaultUserDesignBlocksPath();

    static QString GetDefaultUser3DModelsPath();

    static QString GetStockDataPath( bool aRespectRunFromBuildDir = true );

    static QString GetStockEDALibraryPath();

    static QString GetDefault3rdPartyPath();

    static QString GetStockSymbolsPath();

    static QString GetStockFootprintsPath();

    static QString GetStockDesignBlocksPath();

    static QString GetStock3dmodelsPath();

    static QString GetStockScriptingPath();

    static QString GetStockPluginsPath();

    static QString GetStockPlugins3DPath();

    static QString GetStockDemosPath();

    static QString GetStockTemplatesPath();

    static QString GetLocaleDataPath();

    static QString GetUserCachePath();

    static QString GetDocumentationPath();

    static QString GetInstanceCheckerPath();

    static QString GetLogsPath();

    static bool EnsurePathExists( const QString& aPath, bool aPathToFile = false );

    static void EnsureUserPathsExist();

#ifdef __WXMAC__
    static QString GetOSXKicadUserDataDir();

    static QString GetOSXKicadMachineDataDir();

    static QString GetOSXKicadDataDir();
#endif

#ifdef _WIN32
    static QString GetWindowsFontConfigDir();

    static QString GetWindowsBaseSharePath();
#endif

    static QString GetUserSettingsPath();

    static QString CalculateUserSettingsPath( bool aIncludeVer = true, bool aUseEnv = true );

    static const QString& GetExecutablePath();
private:
    // we are a static helper
    PATHS() {}

    static void getUserDocumentPath( QDir& aPath );

#ifdef _WIN32
    static QString getWindowsKiCadRoot();
#endif
};

#endif
