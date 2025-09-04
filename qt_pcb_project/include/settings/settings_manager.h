
// QT_TRANSFORMATION_COMPLETED
#ifndef _SETTINGS_MANAGER_H
#define _SETTINGS_MANAGER_H

#include <algorithm>
#include <mutex>
#include <typeinfo>
#include <QHash>
#include <QString>
#include <settings/color_settings.h>

class COLOR_SETTINGS;
class COMMON_SETTINGS;
class KIWAY;
class PROJECT;
class PROJECT_FILE;
class REPORTER;
class LOCKFILE;
class QFileInfo;


#define PROJECT_BACKUPS_DIR_SUFFIX "-backups"


class KICOMMON_API SETTINGS_MANAGER
{
public:
    SETTINGS_MANAGER( bool aHeadless = false );

    ~SETTINGS_MANAGER();

    bool IsOK() { return m_ok; }

    void SetKiway( KIWAY* aKiway ) { m_kiway = aKiway; }

    template<typename T>
    T* RegisterSettings( T* aSettings, bool aLoadNow = true )
    {
        return static_cast<T*>( registerSettings( aSettings, aLoadNow ) );
    }

    void Load();

    void Load( JSON_SETTINGS* aSettings );

    void Save();

    void Save( JSON_SETTINGS* aSettings );

    void FlushAndRelease( JSON_SETTINGS* aSettings, bool aSave = true );

    template<typename T>
    T* GetAppSettings( const char* aFilename )
    {
        std::lock_guard lock( m_app_settings_mutex );

        T*     ret      = nullptr;
        size_t typeHash = typeid( T ).hash_code();

         if( m_app_settings_cache.count( typeHash ) )
            ret = static_cast<T*>( m_app_settings_cache.at( typeHash ) );

        if( ret )
            return ret;

#if defined(__clang__)
        auto it = std::find_if( m_settings.begin(), m_settings.end(),
                                [&]( const std::unique_ptr<JSON_SETTINGS>& aSettings )
                                {
                                    return aSettings->GetFilename() == aFilename;
                                } );
#else
        auto it = std::find_if( m_settings.begin(), m_settings.end(),
                                []( const std::unique_ptr<JSON_SETTINGS>& aSettings )
                                {
                                    return dynamic_cast<T*>( aSettings.get() );
                                } );
#endif

        if( it != m_settings.end() )
        {
            // Do NOT use dynamic_cast here.  CLang will think it's the wrong class across
            // compile boundaries and return nullptr.
            ret = static_cast<T*>( it->get() );
        }
        else
        {
            throw std::runtime_error( "Tried to GetAppSettings before registering" );
        }

        m_app_settings_cache[typeHash] = ret;

        return ret;
    }

    COLOR_SETTINGS* GetColorSettings( const QString& aName = "user" );

    std::vector<COLOR_SETTINGS*> GetColorSettingsList()
    {
        std::vector<COLOR_SETTINGS*> ret;

        for( auto it = m_color_settings.begin(); it != m_color_settings.end(); ++it )
            ret.push_back( it.value() );

        std::sort( ret.begin(), ret.end(), []( COLOR_SETTINGS* a, COLOR_SETTINGS* b )
                                           {
                                               return a->GetName() < b->GetName();
                                           } );

        return ret;
    }

    void SaveColorSettings( COLOR_SETTINGS* aSettings, const std::string& aNamespace = "" );

    COLOR_SETTINGS* AddNewColorSettings( const QString& aFilename );

    COLOR_SETTINGS* GetMigratedColorSettings();

    COMMON_SETTINGS* GetCommonSettings() const { return m_common_settings; }

    QString GetPathForSettingsFile( JSON_SETTINGS* aSettings );

    bool MigrateIfNeeded();

    void SetMigrationSource( const QString& aSource ) { m_migration_source = aSource; }

    void SetMigrateLibraryTables( bool aMigrate = true ) { m_migrateLibraryTables = aMigrate; }

    bool GetPreviousVersionPaths( std::vector<QString>* aName = nullptr );

    void ReloadColorSettings();

    bool LoadProject( const QString& aFullPath, bool aSetActive = true );

    bool UnloadProject( PROJECT* aProject, bool aSave = true );

    bool IsProjectOpen() const;

    bool IsProjectOpenNotDummy() const;

    PROJECT& Prj() const;

    PROJECT* GetProject( const QString& aFullPath ) const;

    std::vector<QString> GetOpenProjects() const;

    bool SaveProject( const QString& aFullPath = QString(), PROJECT* aProject = nullptr );

    void SaveProjectAs( const QString& aFullPath, PROJECT* aProject = nullptr );

    void SaveProjectCopy( const QString& aFullPath, PROJECT* aProject = nullptr );

    QString GetProjectBackupsPath() const;

    bool BackupProject( REPORTER& aReporter, QFileInfo& aTarget ) const;

    bool TriggerBackupIfNeeded( REPORTER& aReporter ) const;

    static bool IsSettingsPathValid( const QString& aPath );

    static QString GetColorSettingsPath();

    static std::string GetSettingsVersion();

    static QString GetUserSettingsPath();

private:
    JSON_SETTINGS* registerSettings( JSON_SETTINGS* aSettings, bool aLoadNow = true );

    static int compareVersions( const std::string& aFirst, const std::string& aSecond );

    static bool extractVersion( const std::string& aVersionString, int* aMajor = nullptr,
                                int* aMinor = nullptr );

    COLOR_SETTINGS* loadColorSettingsByName( const QString& aName );

    COLOR_SETTINGS* registerColorSettings( const QString& aFilename, bool aAbsolutePath = false );

    void loadAllColorSettings();

    bool loadProjectFile( PROJECT& aProject );

    bool unloadProjectFile( PROJECT* aProject, bool aSave );

    void registerBuiltinColorSettings();

private:

    bool m_headless;

    KIWAY* m_kiway;

    std::vector<std::unique_ptr<JSON_SETTINGS>> m_settings;

    QHash<QString, COLOR_SETTINGS*> m_color_settings;

    std::unordered_map<size_t, JSON_SETTINGS*> m_app_settings_cache;
    std::mutex m_app_settings_mutex;

    COMMON_SETTINGS* m_common_settings;

    QString m_migration_source;

    bool m_migrateLibraryTables;

    bool m_ok;

    std::vector<std::unique_ptr<PROJECT>> m_projects_list;

    QHash<QString, PROJECT*> m_projects;

    QHash<QString, PROJECT_FILE*> m_project_files;

    std::unique_ptr<LOCKFILE> m_project_lock;

    static QString backupDateTimeFormat;
};

#endif
