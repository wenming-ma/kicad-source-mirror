
#include "settings/json_settings.h"
#include <regex>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QCoreApplication>

#include <build_version.h>
#include <confirm.h>
#include <dialogs/dialog_migrate_settings.h>
#include <gestfich.h>
#include <kiplatform/environment.h>
#include <kiplatform/io.h>
#include <kiway.h>
#include <lockfile.h>
#include <macros.h>
#include <pgm_base.h>
#include <paths.h>
#include <project.h>
#include <project/project_archiver.h>
#include <project/project_file.h>
#include <project/project_local_settings.h>
#include <settings/color_settings.h>
#include <settings/common_settings.h>
#include <settings/json_settings_internals.h>
#include <settings/settings_manager.h>
#include <wildcards_and_files_ext.h>
#include <env_vars.h>


SETTINGS_MANAGER::SETTINGS_MANAGER( bool aHeadless ) :
        m_headless( aHeadless ),
        m_kiway( nullptr ),
        m_common_settings( nullptr ),
        m_migration_source(),
        m_migrateLibraryTables( true )
{
    // Check if the settings directory already exists, and if not, perform a migration if possible
    if( !MigrateIfNeeded() )
    {
        m_ok = false;
        return;
    }

    m_ok = true;

    // create the common settings shared by all applications.  Not loaded immediately
    m_common_settings = RegisterSettings( new COMMON_SETTINGS, false );

    // Create the built-in color settings
    // Here to allow the Python API to access the built-in colors
    registerBuiltinColorSettings();

    QFileInfo commonSettings( QDir(GetPathForSettingsFile( m_common_settings )).filePath(m_common_settings->GetFullFilename()) );

    if( !commonSettings.exists() )
    {
        m_common_settings->Load();
        Save( m_common_settings );
    }
}


SETTINGS_MANAGER::~SETTINGS_MANAGER()
{
    for( std::unique_ptr<PROJECT>& project : m_projects_list )
        project.reset();

    m_projects.clear();

    for( std::unique_ptr<JSON_SETTINGS>& settings : m_settings )
        settings.reset();

    m_settings.clear();

    m_color_settings.clear();
}


JSON_SETTINGS* SETTINGS_MANAGER::registerSettings( JSON_SETTINGS* aSettings, bool aLoadNow )
{
    std::unique_ptr<JSON_SETTINGS> ptr( aSettings );

    ptr->SetManager( this );

    qDebug() << "Registered new settings object" << ptr->GetFullFilename();

    if( aLoadNow )
        ptr->LoadFromFile( GetPathForSettingsFile( ptr.get() ) );

    m_settings.push_back( std::move( ptr ) );
    return m_settings.back().get();
}


void SETTINGS_MANAGER::Load()
{
    // TODO(JE) We should check for dirty settings here and write them if so, because
    // Load() could be called late in the application lifecycle
    std::vector<JSON_SETTINGS*> toLoad;

    // Cache a copy of raw pointers; m_settings may be modified during the load loop
    std::transform( m_settings.begin(), m_settings.end(), std::back_inserter( toLoad ),
                    []( std::unique_ptr<JSON_SETTINGS>& aSettings )
                    {
                        return aSettings.get();
                    } );

    for( JSON_SETTINGS* settings : toLoad )
        settings->LoadFromFile( GetPathForSettingsFile( settings ) );
}


void SETTINGS_MANAGER::Load( JSON_SETTINGS* aSettings )
{
    auto it = std::find_if( m_settings.begin(), m_settings.end(),
                            [&aSettings]( const std::unique_ptr<JSON_SETTINGS>& aPtr )
                            {
                                return aPtr.get() == aSettings;
                            } );

    if( it != m_settings.end() )
        ( *it )->LoadFromFile( GetPathForSettingsFile( it->get() ) );
}


void SETTINGS_MANAGER::Save()
{
    for( auto&& settings : m_settings )
    {
        // Never automatically save color settings, caller should use SaveColorSettings
        if( dynamic_cast<COLOR_SETTINGS*>( settings.get() ) )
            continue;

        // Never automatically save project settings, caller should use SaveProject or UnloadProject
        if( dynamic_cast<PROJECT_FILE*>( settings.get() )
            || dynamic_cast<PROJECT_LOCAL_SETTINGS*>( settings.get() ) )
        {
            continue;
        }

        settings->SaveToFile( GetPathForSettingsFile( settings.get() ) );
    }
}


void SETTINGS_MANAGER::Save( JSON_SETTINGS* aSettings )
{
    auto it = std::find_if( m_settings.begin(), m_settings.end(),
                            [&aSettings]( const std::unique_ptr<JSON_SETTINGS>& aPtr )
                            {
                                return aPtr.get() == aSettings;
                            } );

    if( it != m_settings.end() )
    {
        qDebug() << "Saving" << ( *it )->GetFullFilename();
        ( *it )->SaveToFile( GetPathForSettingsFile( it->get() ) );
    }
}


void SETTINGS_MANAGER::FlushAndRelease( JSON_SETTINGS* aSettings, bool aSave )
{
    auto it = std::find_if( m_settings.begin(), m_settings.end(),
                            [&aSettings]( const std::unique_ptr<JSON_SETTINGS>& aPtr )
                            {
                                return aPtr.get() == aSettings;
                            } );

    if( it != m_settings.end() )
    {
        qDebug() << "Flush and release" << ( *it )->GetFullFilename();

        if( aSave )
            ( *it )->SaveToFile( GetPathForSettingsFile( it->get() ) );

        JSON_SETTINGS* tmp = it->get(); // We use a temporary to suppress a Clang warning
        size_t         typeHash = typeid( *tmp ).hash_code();

        if( m_app_settings_cache.count( typeHash ) )
            m_app_settings_cache.erase( typeHash );

        m_settings.erase( it );
    }
}


COLOR_SETTINGS* SETTINGS_MANAGER::GetColorSettings( const QString& aName )
{
    // Find settings the fast way
    if( m_color_settings.count( aName ) )
        return m_color_settings.at( aName );

    // Maybe it's the display name (cli is one method of invoke)
    auto it = std::find_if( m_color_settings.begin(), m_color_settings.end(),
                            [&aName]( const std::pair<QString, COLOR_SETTINGS*>& p )
                            {
                                return p.second->GetName().toLower() == aName.toLower();
                            } );

    if( it != m_color_settings.end() )
    {
        return it->second;
    }

    // No match? See if we can load it
    if( !aName.isEmpty() )
    {
        COLOR_SETTINGS* ret = loadColorSettingsByName( aName );

        if( !ret )
        {
            ret = registerColorSettings( aName );
            *ret = *m_color_settings.at( COLOR_SETTINGS::COLOR_BUILTIN_DEFAULT );
            ret->SetFilename( "user" );
            ret->SetReadOnly( false );
        }

        return ret;
    }

    // This had better work
    return m_color_settings.at( COLOR_SETTINGS::COLOR_BUILTIN_DEFAULT );
}


COLOR_SETTINGS* SETTINGS_MANAGER::loadColorSettingsByName( const QString& aName )
{
    qDebug() << "Attempting to load color theme" << aName;

    QFileInfo fn( QDir(GetColorSettingsPath()).filePath(aName + ".json") );

    if( !fn.exists() )
    {
        qDebug() << "Theme file" << (aName + ".json") << "not found, falling back to user";
        return nullptr;
    }

    COLOR_SETTINGS* settings = RegisterSettings( new COLOR_SETTINGS( aName ) );

    if( settings->GetFilename() != aName.toStdString() )
    {
        qDebug() << "Warning: stored filename is actually" << settings->GetFilename();
    }

    m_color_settings[aName] = settings;

    return settings;
}


class JSON_DIR_TRAVERSER
{
private:
    std::function<void( const QFileInfo& )> m_action;

public:
    explicit JSON_DIR_TRAVERSER( std::function<void( const QFileInfo& )> aAction )
            : m_action( std::move( aAction ) )
    {
    }

    void traverse( const QDir& dir )
    {
        QStringList files = dir.entryList(QStringList() << "*.json", QDir::Files);
        for( const QString& fileName : files )
        {
            QFileInfo file(dir.filePath(fileName));
            m_action( file );
        }
        
        QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for( const QString& subdir : subdirs )
        {
            QDir subDir(dir.filePath(subdir));
            traverse(subDir);
        }
    }
};


COLOR_SETTINGS* SETTINGS_MANAGER::registerColorSettings( const QString& aName, bool aAbsolutePath )
{
    if( !m_color_settings.count( aName ) )
    {
        COLOR_SETTINGS* colorSettings = RegisterSettings( new COLOR_SETTINGS( aName,
                                                                              aAbsolutePath ) );
        m_color_settings[aName] = colorSettings;
    }

    return m_color_settings.at( aName );
}


COLOR_SETTINGS* SETTINGS_MANAGER::AddNewColorSettings( const QString& aName )
{
    if( aName.endsWith( ".json" ) )
        return registerColorSettings( aName.left(aName.lastIndexOf('.')) );
    else
        return registerColorSettings( aName );
}


COLOR_SETTINGS* SETTINGS_MANAGER::GetMigratedColorSettings()
{
    if( !m_color_settings.count( "user" ) )
    {
        COLOR_SETTINGS* settings = registerColorSettings( "user" );
        settings->SetName( "User" );
        Save( settings );
    }

    return m_color_settings.at( "user" );
}


void SETTINGS_MANAGER::registerBuiltinColorSettings()
{
    for( COLOR_SETTINGS* settings : COLOR_SETTINGS::CreateBuiltinColorSettings() )
        m_color_settings[settings->GetFilename()] = RegisterSettings( settings, false );
}


void SETTINGS_MANAGER::loadAllColorSettings()
{
    // Create the built-in color settings
    registerBuiltinColorSettings();

    QDir third_party_path;
    const ENV_VAR_MAP& env = Pgm().GetLocalEnvVariables();
    auto               it = env.find( ENV_VAR::GetVersionedEnvVarName( "3RD_PARTY" ) );

    if( it != env.end() && !it->second.GetValue().isEmpty() )
        third_party_path.setPath( it->second.GetValue() );
    else
        third_party_path.setPath( PATHS::GetDefault3rdPartyPath() );

    third_party_path = QDir(third_party_path.filePath("colors"));

    // PCM-managed themes
    QDir third_party_colors_dir( third_party_path.absolutePath() );

    // System-installed themes
    QDir system_colors_dir( PATHS::GetStockDataPath( false ) + "/colors" );

    // User-created themes
    QDir colors_dir( GetColorSettingsPath() );

    // Search for and load any other settings
    JSON_DIR_TRAVERSER loader( [&]( const QFileInfo& aFileInfo )
                               {
                                   registerColorSettings( aFileInfo.baseName() );
                               } );

    JSON_DIR_TRAVERSER readOnlyLoader(
            [&]( const QFileInfo& aFileInfo )
            {
                COLOR_SETTINGS* settings = registerColorSettings( aFileInfo.absoluteFilePath(), true );
                settings->SetReadOnly( true );
            } );

    if( system_colors_dir.exists() )
        readOnlyLoader.traverse( system_colors_dir );

    if( third_party_colors_dir.exists() )
        readOnlyLoader.traverse( third_party_colors_dir );

    if( colors_dir.exists() )
        loader.traverse( colors_dir );
}


void SETTINGS_MANAGER::ReloadColorSettings()
{
    m_color_settings.clear();
    loadAllColorSettings();
}


void SETTINGS_MANAGER::SaveColorSettings( COLOR_SETTINGS* aSettings, const std::string& aNamespace )
{
    // The passed settings should already be managed
    Q_ASSERT( std::find_if( m_color_settings.begin(), m_color_settings.end(),
                            [aSettings] ( const std::pair<QString, COLOR_SETTINGS*>& el )
                            {
                                return el.second->GetFilename() == aSettings->GetFilename();
                            }
                            ) != m_color_settings.end() );

    if( aSettings->IsReadOnly() )
        return;

    if( !aSettings->Store() )
    {
        qDebug() << "Color scheme" << QString::fromStdString(aNamespace) << "not modified; skipping save";
        return;
    }

    Q_ASSERT( aSettings->Contains( aNamespace ) );

    qDebug() << "Saving color scheme" << aSettings->GetFilename() << ", preserving" << QString::fromStdString(aNamespace);

    std::optional<nlohmann::json> backup = aSettings->GetJson( aNamespace );
    QString path = GetColorSettingsPath();

    aSettings->LoadFromFile( path );

    if( backup )
        ( *aSettings->Internals() )[aNamespace].update( *backup );

    aSettings->Load();

    aSettings->SaveToFile( path, true );
}


QString SETTINGS_MANAGER::GetPathForSettingsFile( JSON_SETTINGS* aSettings )
{
    Q_ASSERT( aSettings );

    switch( aSettings->GetLocation() )
    {
    case SETTINGS_LOC::USER:
        return PATHS::GetUserSettingsPath();

    case SETTINGS_LOC::PROJECT:
        // TODO: MDI support
        return Prj().GetProjectPath();

    case SETTINGS_LOC::COLORS:
        return GetColorSettingsPath();

    case SETTINGS_LOC::NONE:
        return QString();

    default:
        Q_ASSERT_X( false, "GetPathForSettingsFile", "Unknown settings location!" );
    }

    return "";
}


class MIGRATION_TRAVERSER
{
private:
    QString m_src;
    QString m_dest;
    QString m_errors;
    bool    m_migrateTables;

public:
    MIGRATION_TRAVERSER( const QString& aSrcDir, const QString& aDestDir, bool aMigrateTables ) :
            m_src( aSrcDir ),
            m_dest( aDestDir ),
            m_migrateTables( aMigrateTables )
    {
    }

    QString GetErrors() { return m_errors; }

    void traverse( const QDir& srcDir )
    {
        QStringList files = srcDir.entryList(QDir::Files);
        for( const QString& fileName : files )
        {
            QFileInfo file(srcDir.filePath(fileName));

            if( !m_migrateTables && ( file.baseName() == FILEEXT::SymbolLibraryTableFileName ||
                                      file.baseName() == FILEEXT::FootprintLibraryTableFileName ) )
            {
                continue;
            }

            // Skip migrating PCM installed packages as packages themselves are not moved
            if( file.fileName() == "installed_packages.json" )
                continue;

            // Don't migrate hotkeys config files; we don't have a reasonable migration handler for them
            // and so there is no way to resolve conflicts at the moment
            if( file.suffix() == "hotkeys" )
                continue;

            QString destPath = file.absoluteFilePath();
            destPath.replace( m_src, m_dest );
            QFileInfo destFile(destPath);

            qDebug() << "Copying" << file.absoluteFilePath() << "to" << destFile.absoluteFilePath();

            // For now, just copy everything
            KiCopyFile( file.absoluteFilePath(), destFile.absoluteFilePath(), m_errors );
        }

        QStringList subdirs = srcDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for( const QString& subdir : subdirs )
        {
            QDir dir(srcDir.filePath(subdir));
            QFileInfo dirInfo(dir.absolutePath());

            // Whitelist of directories to migrate
            if( dirInfo.baseName() == "colors" ||
                dirInfo.baseName() == "3d" )
            {
                QString destDirPath = dir.absolutePath();
                destDirPath.replace( m_src, m_dest );
                QDir destDir(destDirPath);

                destDir.mkpath(".");
                traverse( dir );
            }
        }
    }
};


bool SETTINGS_MANAGER::MigrateIfNeeded()
{
    QDir path( PATHS::GetUserSettingsPath() );
    qDebug() << "Using settings path" << path.absolutePath();

    if( m_headless )
    {
        // Special case namely for cli
        // Ensure the settings directory at least exists to prevent additional loading errors
        // from subdirectories.
        // TODO review headless (unit tests) vs cli needs, this should be fine for unit tests though
        if( !path.exists() )
        {
            qDebug() << "Path didn't exist; creating it";
            path.mkpath(".");
        }

        qDebug() << "Settings migration not checked; running headless";
        return true;
    }

    if( path.exists() )
    {
        QFileInfo common( path.filePath("kicad_common.json") );

        if( common.exists() )
        {
            qDebug() << "Path exists and has a kicad_common, continuing!";
            return true;
        }
    }

    // Now we have an empty path, let's figure out what to put in it
    // Migration dialog handling removed

    if( !path.exists() )
    {
        qDebug() << "Path didn't exist; creating it";
        path.mkpath(".");
    }

    if( m_migration_source.isEmpty() )
    {
        qDebug() << "No migration source given; starting with defaults";
        return true;
    }

    qDebug() << "Migrating from path" << m_migration_source;

    MIGRATION_TRAVERSER traverser( m_migration_source, path.absolutePath(), m_migrateLibraryTables );
    QDir source_dir( m_migration_source );

    traverser.traverse( source_dir );

    if( !traverser.GetErrors().isEmpty() )
        DisplayErrorMessage( nullptr, traverser.GetErrors() );

    // Remove any library configuration if we didn't choose to import
    if( !m_migrateLibraryTables )
    {
        COMMON_SETTINGS common;
        QString        commonPath = GetPathForSettingsFile( &common );
        common.LoadFromFile( commonPath );

        const std::vector<QString> libKeys = {
            "KICAD6_SYMBOL_DIR",
            "KICAD6_3DMODEL_DIR",
            "KICAD6_FOOTPRINT_DIR",
            "KICAD6_TEMPLATE_DIR", // Stores the default library table to be copied
            "KICAD7_SYMBOL_DIR",
            "KICAD7_3DMODEL_DIR",
            "KICAD7_FOOTPRINT_DIR",
            "KICAD7_TEMPLATE_DIR",
            "KICAD8_SYMBOL_DIR",
            "KICAD8_3DMODEL_DIR",
            "KICAD8_FOOTPRINT_DIR",
            "KICAD8_TEMPLATE_DIR",

            // Deprecated keys
            "KICAD_PTEMPLATES",
            "KISYS3DMOD",
            "KISYSMOD",
            "KICAD_SYMBOL_DIR",
        };

        for( const QString& key : libKeys )
            common.m_Env.vars.erase( key );

        common.SaveToFile( commonPath );
    }

    return true;
}


bool SETTINGS_MANAGER::GetPreviousVersionPaths( std::vector<QString>* aPaths )
{
    Q_ASSERT( aPaths );

    aPaths->clear();

    QDir dir;
    std::vector<QDir> base_paths;

    base_paths.emplace_back( QDir( PATHS::CalculateUserSettingsPath( false ) ) );

    // If the env override is set, also check the default paths
    if( qEnvironmentVariableIsSet( "KICAD_CONFIG_HOME" ) )
        base_paths.emplace_back( QDir( PATHS::CalculateUserSettingsPath( false, false ) ) );

#ifdef __linux__
    // When running inside FlatPak, KIPLATFORM::ENV::GetUserConfigPath() will return a sandboxed
    // path.  In case the user wants to move from non-FlatPak KiCad to FlatPak KiCad, let's add our
    // best guess as to the non-FlatPak config path.  Unfortunately FlatPak also hides the host
    // XDG_CONFIG_HOME, so if the user customizes their config path, they will have to browse
    // for it.
    {
        QDir gtkPath( QDir::home().filePath(".config/kicad") );
        base_paths.emplace_back( gtkPath );

        // We also want to pick up regular flatpak if we are nightly
        QDir flatpakPath( QDir::home().filePath(".var/app/org.kicad.KiCad/config/kicad") );
        base_paths.emplace_back( flatpakPath );
    }
#endif

    QString subdir;
    std::string mine = GetSettingsVersion();

    auto check_dir = [&] ( const QString& aSubDir )
    {
        // Only older versions are valid for migration
        if( compareVersions( aSubDir.toStdString(), mine ) <= 0 )
        {
            QString sub_path = dir.absolutePath() + "/" + aSubDir;

            if( IsSettingsPathValid( sub_path ) )
            {
                aPaths->push_back( sub_path );
                qDebug() << "GetPreviousVersionName:" << sub_path << "is valid";
            }
        }
    };

    std::set<QString> checkedPaths;

    for( const QDir& base_path : base_paths )
    {
        if( checkedPaths.count( base_path.absolutePath() ) )
            continue;

        checkedPaths.insert( base_path.absolutePath() );

        dir.setPath( base_path.absolutePath() );
        if( !dir.exists() )
        {
            qDebug() << "GetPreviousVersionName: could not open base path" << base_path.absolutePath();
            continue;
        }

        qDebug() << "GetPreviousVersionName: checking base path" << base_path.absolutePath();

        QStringList subdirs = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
        for( const QString& subdir : subdirs )
        {
            if( subdir != QString::fromStdString(mine) )
                check_dir( subdir );
        }

        // If we didn't find one yet, check for legacy settings without a version directory
        if( IsSettingsPathValid( dir.absolutePath() ) )
        {
            qDebug() << "GetPreviousVersionName: root path" << dir.dirName() << "is valid";
            aPaths->push_back( dir.dirName() );
        }
    }

    alg::delete_if( *aPaths, []( const QString& aPath ) -> bool
    {
        QDir fulldir( aPath );
        QFileInfo dirInfo( fulldir.absolutePath() );

        if( !dirInfo.isReadable() )
            return true;

        QString dirName = dirInfo.baseName();
        std::string ver = dirName.toStdString();

        if( !extractVersion( ver ) )
            return true;

        return false;
    } );

    std::sort( aPaths->begin(), aPaths->end(),
               [&]( const QString& a, const QString& b ) -> bool
               {
                   QFileInfo aInfo( a );
                   QFileInfo bInfo( b );

                   QString verAStr = aInfo.baseName();
                   QString verBStr = bInfo.baseName();

                   if( verAStr.isEmpty() )
                       return false;

                   if( verBStr.isEmpty() )
                       return true;

                   std::string verA = verAStr.toStdString();
                   std::string verB = verBStr.toStdString();

                   if( !extractVersion( verA ) )
                       return false;

                   if( !extractVersion( verB ) )
                       return true;

                   return compareVersions( verA, verB ) >= 0;
               } );

    return aPaths->size() > 0;
}


bool SETTINGS_MANAGER::IsSettingsPathValid( const QString& aPath )
{
    QFileInfo test( QDir(aPath).filePath("kicad_common") );

    if( test.exists() )
        return true;

    QFileInfo testJson( QDir(aPath).filePath("kicad_common.json") );

    return testJson.exists();
}


QString SETTINGS_MANAGER::GetColorSettingsPath()
{
    QDir path( PATHS::GetUserSettingsPath() );
    path = QDir(path.filePath("colors"));

    if( !path.exists() )
    {
        if( !path.mkpath(".") )
        {
            qDebug() << "GetColorSettingsPath(): Path" << path.absolutePath() << "missing and could not be created!";
        }
    }

    return path.absolutePath();
}


std::string SETTINGS_MANAGER::GetSettingsVersion()
{
    // CMake computes the major.minor string for us.
    return GetMajorMinorVersion().toStdString();
}


int SETTINGS_MANAGER::compareVersions( const std::string& aFirst, const std::string& aSecond )
{
    int a_maj = 0;
    int a_min = 0;
    int b_maj = 0;
    int b_min = 0;

    if( !extractVersion( aFirst, &a_maj, &a_min ) || !extractVersion( aSecond, &b_maj, &b_min ) )
    {
        qDebug() << "compareSettingsVersions: bad input" << QString::fromStdString(aFirst) << QString::fromStdString(aSecond);
        return -1;
    }

    if( a_maj < b_maj )
    {
        return -1;
    }
    else if( a_maj > b_maj )
    {
        return 1;
    }
    else
    {
        if( a_min < b_min )
        {
            return -1;
        }
        else if( a_min > b_min )
        {
            return 1;
        }
        else
        {
            return 0;
        }
    }
}


bool SETTINGS_MANAGER::extractVersion( const std::string& aVersionString, int* aMajor, int* aMinor )
{
    std::regex  re_version( "(\\d+)\\.(\\d+)" );
    std::smatch match;

    if( std::regex_match( aVersionString, match, re_version ) )
    {
        try
        {
            int major = std::stoi( match[1].str() );
            int minor = std::stoi( match[2].str() );

            if( aMajor )
                *aMajor = major;

            if( aMinor )
                *aMinor = minor;
        }
        catch( ... )
        {
            return false;
        }

        return true;
    }

    return false;
}


bool SETTINGS_MANAGER::LoadProject( const QString& aFullPath, bool aSetActive )
{
    // Normalize path to new format even if migrating from a legacy file
    QFileInfo path( aFullPath );
    
    QString fullPath = aFullPath;
    if( path.suffix() == FILEEXT::LegacyProjectFileExtension )
        fullPath = path.absolutePath() + "/" + path.baseName() + "." + QString::fromStdString(FILEEXT::ProjectFileExtension);
    
    if( fullPath == aFullPath )
        fullPath = path.absoluteFilePath();

    // If already loaded, we are all set.  This might be called more than once over a project's
    // lifetime in case the project is first loaded by the KiCad manager and then Eeschema or
    // Pcbnew try to load it again when they are launched.
    if( m_projects.count( fullPath ) )
        return true;

    bool readOnly = false;
    LOCKFILE lockFile( fullPath );

    if( !lockFile.Valid() )
    {
        qDebug() << "Project" << fullPath << "is locked; opening read-only";
        readOnly = true;
    }

    // No MDI yet
    if( aSetActive && !m_projects.empty() )
    {
        PROJECT* oldProject = m_projects.begin()->second;
        unloadProjectFile( oldProject, false );
        m_projects.erase( m_projects.begin() );

        auto it = std::find_if( m_projects_list.begin(), m_projects_list.end(),
                                [&]( const std::unique_ptr<PROJECT>& ptr )
                                {
                                    return ptr.get() == oldProject;
                                } );

        Q_ASSERT( it != m_projects_list.end() );
        m_projects_list.erase( it );
    }

    qDebug() << "Load project" << fullPath;

    std::unique_ptr<PROJECT> project = std::make_unique<PROJECT>();
    project->setProjectFullName( fullPath );

    if( aSetActive )
    {
        // until multiple projects are in play, set an environment variable for the
        // the project pointer.
        QFileInfo projectPath( fullPath );
        qputenv( PROJECT_VAR_NAME.toLocal8Bit().constData(), projectPath.absolutePath().toUtf8() );

        // set the cwd but don't impact kicad-cli
        if( !projectPath.absolutePath().isEmpty() && QCoreApplication::instance() )
            QDir::setCurrent( projectPath.absolutePath() );
    }

    bool success = loadProjectFile( *project );

    if( success )
    {
        project->SetReadOnly( readOnly || project->GetProjectFile().IsReadOnly() );

        if( lockFile && aSetActive )
            m_project_lock.reset( new LOCKFILE( std::move( lockFile ) ) );
    }

    m_projects_list.push_back( std::move( project ) );
    m_projects[fullPath] = m_projects_list.back().get();

    QString fn( QFileInfo(fullPath).baseName() );

    PROJECT_LOCAL_SETTINGS* settings = new PROJECT_LOCAL_SETTINGS( m_projects[fullPath], fn );

    if( aSetActive )
        settings = RegisterSettings( settings );
    else
        settings->LoadFromFile( QFileInfo(fullPath).absolutePath() );

    m_projects[fullPath]->setLocalSettings( settings );

    if( aSetActive && m_kiway )
        m_kiway->ProjectChanged();

    return success;
}


bool SETTINGS_MANAGER::UnloadProject( PROJECT* aProject, bool aSave )
{
    if( !aProject || !m_projects.count( aProject->GetProjectFullName() ) )
        return false;

    if( !unloadProjectFile( aProject, aSave ) )
        return false;

    QString projectPath = aProject->GetProjectFullName();
    qDebug() << "Unload project" << projectPath;

    PROJECT* toRemove = m_projects.at( projectPath );
    bool wasActiveProject = m_projects_list.begin()->get() == toRemove;

    auto it = std::find_if( m_projects_list.begin(), m_projects_list.end(),
                            [&]( const std::unique_ptr<PROJECT>& ptr )
                            {
                                return ptr.get() == toRemove;
                            } );

    Q_ASSERT( it != m_projects_list.end() );
    m_projects_list.erase( it );

    m_projects.erase( projectPath );

    if( wasActiveProject )
    {
        // Immediately reload a null project; this is required until the rest of the application
        // is refactored to not assume that Prj() always works
        if( m_projects_list.empty() )
            LoadProject( "" );

        // Remove the reference in the environment to the previous project
        qputenv( PROJECT_VAR_NAME.toLocal8Bit().constData(), "" );

        // Release lock on the file, in case we had one
        m_project_lock = nullptr;

        if( m_kiway )
            m_kiway->ProjectChanged();
    }

    return true;
}


PROJECT& SETTINGS_MANAGER::Prj() const
{
    // No MDI yet:  First project in the list is the active project
    Q_ASSERT_X( m_projects_list.size(), "Prj", "no project in list" );
    return *m_projects_list.begin()->get();
}


bool SETTINGS_MANAGER::IsProjectOpen() const
{
    return !m_projects.empty();
}


bool SETTINGS_MANAGER::IsProjectOpenNotDummy() const
{
    return m_projects.size() > 1 || ( m_projects.size() == 1
        && !m_projects.begin()->second->GetProjectFullName().isEmpty() );
}


PROJECT* SETTINGS_MANAGER::GetProject( const QString& aFullPath ) const
{
    if( m_projects.count( aFullPath ) )
        return m_projects.at( aFullPath );

    return nullptr;
}


std::vector<QString> SETTINGS_MANAGER::GetOpenProjects() const
{
    std::vector<QString> ret;

    for( const std::pair<const QString, PROJECT*>& pair : m_projects )
    {
        // Don't save empty projects (these are the default project settings)
        if( !pair.first.isEmpty() )
            ret.emplace_back( pair.first );
    }

    return ret;
}


bool SETTINGS_MANAGER::SaveProject( const QString& aFullPath, PROJECT* aProject )
{
    if( !aProject )
        aProject = &Prj();

    QString path = aFullPath;

    if( path.isEmpty() )
        path = aProject->GetProjectFullName();

    // TODO: refactor for MDI
    if( aProject->IsReadOnly() )
        return false;

    if( !m_project_files.count( path ) )
        return false;

    PROJECT_FILE* project     = m_project_files.at( path );
    QString       projectPath = aProject->GetProjectPath();

    project->SaveToFile( projectPath );
    aProject->GetLocalSettings().SaveToFile( projectPath );

    return true;
}


void SETTINGS_MANAGER::SaveProjectAs( const QString& aFullPath, PROJECT* aProject )
{
    if( !aProject )
        aProject = &Prj();

    QString oldName = aProject->GetProjectFullName();

    if( aFullPath == oldName )
    {
        SaveProject( aFullPath, aProject );
        return;
    }

    // Changing this will cause UnloadProject to not save over the "old" project when loading below
    aProject->setProjectFullName( aFullPath );

    QFileInfo fn( aFullPath );

    PROJECT_FILE* project = m_project_files.at( oldName );

    // Ensure read-only flags are copied; this allows doing a "Save As" on a standalone board/sch
    // without creating project files if the checkbox is turned off
    project->SetReadOnly( aProject->IsReadOnly() );
    aProject->GetLocalSettings().SetReadOnly( aProject->IsReadOnly() );

    project->SetFilename( fn.baseName() );
    project->SaveToFile( fn.absolutePath() );

    aProject->GetLocalSettings().SetFilename( fn.baseName() );
    aProject->GetLocalSettings().SaveToFile( fn.absolutePath() );

    m_project_files[fn.absoluteFilePath()] = project;
    m_project_files.erase( oldName );

    m_projects[fn.absoluteFilePath()] = m_projects[oldName];
    m_projects.erase( oldName );
}


void SETTINGS_MANAGER::SaveProjectCopy( const QString& aFullPath, PROJECT* aProject )
{
    if( !aProject )
        aProject = &Prj();

    PROJECT_FILE* project = m_project_files.at( aProject->GetProjectFullName() );
    QString       oldName = project->GetFilename();
    QFileInfo     fn( aFullPath );

    bool readOnly = project->IsReadOnly();
    project->SetReadOnly( false );

    project->SetFilename( fn.baseName() );
    project->SaveToFile( fn.absolutePath() );
    project->SetFilename( oldName );

    PROJECT_LOCAL_SETTINGS& localSettings = aProject->GetLocalSettings();

    localSettings.SetFilename( fn.baseName() );
    localSettings.SaveToFile( fn.absolutePath() );
    localSettings.SetFilename( oldName );

    project->SetReadOnly( readOnly );
}


bool SETTINGS_MANAGER::loadProjectFile( PROJECT& aProject )
{
    QFileInfo fullFn( aProject.GetProjectFullName() );
    QString fn( fullFn.baseName() );

    PROJECT_FILE* file = RegisterSettings( new PROJECT_FILE( fn ), false );

    m_project_files[aProject.GetProjectFullName()] = file;

    aProject.setProjectFile( file );
    file->SetProject( &aProject );

    QString path( fullFn.absolutePath() );

    return file->LoadFromFile( path );
}


bool SETTINGS_MANAGER::unloadProjectFile( PROJECT* aProject, bool aSave )
{
    if( !aProject )
        return false;

    QString name = aProject->GetProjectFullName();

    if( !m_project_files.count( name ) )
        return false;

    PROJECT_FILE* file = m_project_files[name];

    if( !file->ShouldAutoSave() )
        aSave = false;

    auto it = std::find_if( m_settings.begin(), m_settings.end(),
                            [&file]( const std::unique_ptr<JSON_SETTINGS>& aPtr )
                            {
                              return aPtr.get() == file;
                            } );

    if( it != m_settings.end() )
    {
        QString projectPath = GetPathForSettingsFile( it->get() );

        bool saveLocalSettings = aSave && aProject->GetLocalSettings().ShouldAutoSave();

        FlushAndRelease( &aProject->GetLocalSettings(), saveLocalSettings );

        if( aSave )
            ( *it )->SaveToFile( projectPath );

        m_settings.erase( it );
    }

    m_project_files.erase( name );

    return true;
}


QString SETTINGS_MANAGER::GetProjectBackupsPath() const
{
    return Prj().GetProjectPath() + Prj().GetProjectName() + PROJECT_BACKUPS_DIR_SUFFIX;
}


QString SETTINGS_MANAGER::backupDateTimeFormat = "%Y-%m-%d_%H%M%S";


bool SETTINGS_MANAGER::BackupProject( REPORTER& aReporter, QFileInfo& aTarget ) const
{
    QDateTime timestamp = QDateTime::currentDateTime();

    QString fileName = QString("%1-%2").arg(Prj().GetProjectName()).arg(timestamp.toString(backupDateTimeFormat));

    if( aTarget.filePath().isEmpty() )
    {
        QString targetPath = QDir(GetProjectBackupsPath()).filePath(fileName + "." + QString::fromStdString(FILEEXT::ArchiveFileExtension));
        aTarget = QFileInfo(targetPath);
    }

    QString test = aTarget.absolutePath();

    QDir targetDir(aTarget.absolutePath());
    if( !targetDir.exists() && !targetDir.mkpath(".") )
    {
        qDebug() << "Could not create project backup path" << aTarget.absolutePath();
        return false;
    }

    if( !QFileInfo(aTarget.absolutePath()).isWritable() )
    {
        qDebug() << "Backup directory" << aTarget.absolutePath() << "is not writable";
        return false;
    }

    qDebug() << "Backing up project to" << aTarget.absolutePath();

    return PROJECT_ARCHIVER::Archive( Prj().GetProjectPath(), aTarget.absoluteFilePath(), aReporter );
}


class VECTOR_INSERT_TRAVERSER
{
public:
    VECTOR_INSERT_TRAVERSER( std::vector<QString>& aVec,
                             std::function<bool( const QString& )> aCond ) :
            m_files( aVec ),
            m_condition( aCond )
    {
    }

    void traverse( const QDir& dir )
    {
        QStringList files = dir.entryList(QStringList() << "*.zip", QDir::Files);
        for( const QString& fileName : files )
        {
            QString filePath = dir.filePath(fileName);
            if( m_condition( filePath ) )
                m_files.emplace_back( filePath );
        }
    }

private:
    std::vector<QString>& m_files;

    std::function<bool( const QString& )> m_condition;
};


bool SETTINGS_MANAGER::TriggerBackupIfNeeded( REPORTER& aReporter ) const
{
    COMMON_SETTINGS::AUTO_BACKUP settings = GetCommonSettings()->m_Backup;

    if( !settings.enabled )
        return true;

    QString prefix = Prj().GetProjectName() + '-';

    auto modTime =
            [&prefix]( const QString& aFile )
            {
                QDateTime dt;
                QString fn( QFileInfo( aFile ).baseName() );
                fn.replace( prefix, "" );
                dt = QDateTime::fromString( fn, backupDateTimeFormat );
                return dt;
            };

    QDir projectPath( Prj().GetProjectPath() );

    // Skip backup if project path isn't valid or writable
    if( !projectPath.exists() || !QFileInfo(projectPath.absolutePath()).isWritable() )
        return true;

    QString backupPath = GetProjectBackupsPath();

    QDir backupDir(backupPath);
    if( !backupDir.exists() )
    {
        qDebug() << "Backup path" << backupPath << "doesn't exist, creating it";

        if( !backupDir.mkpath(".") )
        {
            qDebug() << "Could not create backups path!  Skipping backup";
            return false;
        }
    }

    QDir dir( backupPath );

    if( !dir.exists() )
    {
        qDebug() << "Could not open project backups path" << dir.dirName();
        return false;
    }

    std::vector<QString> files;

    VECTOR_INSERT_TRAVERSER traverser( files,
            [&modTime]( const QString& aFile )
            {
                return modTime( aFile ).isValid();
            } );

    traverser.traverse( dir );

    // Sort newest-first
    std::sort( files.begin(), files.end(),
               [&]( const QString& aFirst, const QString& aSecond ) -> bool
               {
                   QDateTime first  = modTime( aFirst );
                   QDateTime second = modTime( aSecond );

                   return first.toSecsSinceEpoch() > second.toSecsSinceEpoch();
               } );

    // Do we even need to back up?
    if( !files.empty() )
    {
        QDateTime lastTime = modTime( files[0] );

        if( lastTime.isValid() )
        {
            qint64 delta = QDateTime::currentDateTime().toSecsSinceEpoch() - modTime( files[0] ).toSecsSinceEpoch();

            if( delta < settings.min_interval )
                return true;
        }
    }

    // Backup
    QFileInfo target;
    bool backupSuccessful = BackupProject( aReporter, target );

    if( !backupSuccessful )
        return false;

    // Update the file list
    files.insert( files.begin(), target.absoluteFilePath() );

    // Are there any changes since the last backup?
    if( files.size() >= 2
        && PROJECT_ARCHIVER::AreZipArchivesIdentical( files[0], files[1], aReporter ) )
    {
        QFile::remove( files[0] );
        return true;
    }

    // Now that we know a backup is needed, apply the retention policy

    // Step 1: if we're over the total file limit, remove the oldest
    if( !files.empty() && settings.limit_total_files > 0 )
    {
        while( files.size() > static_cast<size_t>( settings.limit_total_files ) )
        {
            QFile::remove( files.back() );
            files.pop_back();
        }
    }

    // Step 2: Stay under the total size limit
    if( settings.limit_total_size > 0 )
    {
        qulonglong totalSize = 0;

        for( const QString& file : files )
            totalSize += QFileInfo( file ).size();

        while( !files.empty() && totalSize > static_cast<qulonglong>( settings.limit_total_size ) )
        {
            totalSize -= QFileInfo( files.back() ).size();
            QFile::remove( files.back() );
            files.pop_back();
        }
    }

    // Step 3: Stay under the daily limit
    if( settings.limit_daily_files > 0 && files.size() > 1 )
    {
        QDateTime day = modTime( files[0] );
        int       num = 1;

        Q_ASSERT( day.isValid() );

        std::vector<QString> filesToDelete;

        for( size_t i = 1; i < files.size(); i++ )
        {
            QDateTime dt = modTime( files[i] );

            if( dt.date() == day.date() )
            {
                num++;

                if( num > settings.limit_daily_files )
                    filesToDelete.emplace_back( files[i] );
            }
            else
            {
                day = dt;
                num = 1;
            }
        }

        for( const QString& file : filesToDelete )
            QFile::remove( file );
    }

    return true;
}


QString SETTINGS_MANAGER::GetUserSettingsPath()
{
    return PATHS::GetUserSettingsPath();
}
