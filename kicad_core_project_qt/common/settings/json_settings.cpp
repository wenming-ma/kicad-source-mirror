#include <algorithm>
#include <fstream>
#include <iomanip>
#include <utility>
#include <sstream>

#include <locale_io.h>
#include <gal/color4d.h>
#include <settings/json_settings.h>
#include <settings/json_settings_internals.h>
#include <settings/nested_settings.h>
#include <settings/parameters.h>
#include <settings/bom_settings.h>
#include <settings/grid_settings.h>
#include <settings/aui_settings.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QStandardPaths>
#include <QDebug>
#include <QTextStream>


nlohmann::json::json_pointer JSON_SETTINGS_INTERNALS::PointerFromString( std::string aPath )
{
    std::replace( aPath.begin(), aPath.end(), '.', '/' );
    aPath.insert( 0, "/" );

    nlohmann::json::json_pointer p;

    try
    {
        p = nlohmann::json::json_pointer( aPath );
    }
    catch( ... )
    {
        Q_ASSERT_X( false, "JSON_SETTINGS_INTERNALS::PointerFromString", "Invalid pointer path in PointerFromString!" );
    }

    return p;
}


JSON_SETTINGS::JSON_SETTINGS( const QString& aFilename, SETTINGS_LOC aLocation,
                              int aSchemaVersion, bool aCreateIfMissing, bool aCreateIfDefault,
                              bool aWriteFile ) :
        m_filename( aFilename ),
        m_legacy_filename( "" ),
        m_location( aLocation ),
        m_createIfMissing( aCreateIfMissing ),
        m_createIfDefault( aCreateIfDefault ),
        m_writeFile( aWriteFile ),
        m_modified( false ),
        m_deleteLegacyAfterMigration( true ),
        m_resetParamsIfMissing( true ),
        m_schemaVersion( aSchemaVersion ),
        m_isFutureFormat( false ),
        m_manager( nullptr )
{
    m_internals = std::make_unique<JSON_SETTINGS_INTERNALS>();

    try
    {
        m_internals->SetFromString( "meta.filename", GetFullFilename() );
    }
    catch( ... )
    {
        qDebug() << "Error: Could not create filename field for" << GetFullFilename();
    }


    m_params.emplace_back( new PARAM<int>( "meta.version", &m_schemaVersion, m_schemaVersion,
                                           true ) );
}


JSON_SETTINGS::~JSON_SETTINGS()
{
    for( PARAM_BASE* param: m_params )
        delete param;

    m_params.clear();
}


QString JSON_SETTINGS::GetFullFilename() const
{
    if( m_filename.section( '.', -1 ) == getFileExt() )
        return m_filename;

    return QString( m_filename + "." + getFileExt() );
}


nlohmann::json& JSON_SETTINGS::At( const std::string& aPath )
{
    return m_internals->At( aPath );
}


bool JSON_SETTINGS::Contains( const std::string& aPath ) const
{
    return m_internals->contains( JSON_SETTINGS_INTERNALS::PointerFromString( aPath ) );
}


JSON_SETTINGS_INTERNALS* JSON_SETTINGS::Internals()
{
    return m_internals.get();
}


void JSON_SETTINGS::Load()
{
    for( PARAM_BASE* param : m_params )
    {
        try
        {
            param->Load( *this, m_resetParamsIfMissing );
        }
        catch( ... )
        {
            // Skip unreadable parameters in file
            qDebug() << "param" << param->GetJsonPath().c_str() << "load err";
        }
    }
}


bool JSON_SETTINGS::LoadFromFile( const QString& aDirectory )
{
    // First, load all params to default values
    m_internals->clear();
    Load();

    bool success         = true;
    bool migrated        = false;
    bool legacy_migrated = false;

    LOCALE_IO locale;

    auto migrateFromLegacy =
            [&] ( QFileInfo& aPath )
            {
                // Backup and restore during migration so that the original can be mutated if
                // convenient
                bool backed_up = false;
                QString tempPath;

                if( QFileInfo(aPath.dir().path()).isWritable() )
                {
                    tempPath = aPath.filePath() + ".tmp";

                    if( !QFile::copy( aPath.filePath(), tempPath ) )
                    {
                        qDebug() << GetFullFilename() << ": could not create temp file for migration";
                    }
                    else
                    {
                        backed_up = true;
                    }
                }

                // Silence popups if legacy file is read-only
                // Note: Qt doesn't require wxLogNull equivalent, Qt handles silencing differently

                // Qt uses QSettings for configuration management
                auto cfg = std::make_unique<QSettings>( aPath.filePath(),
                                                       QSettings::IniFormat );

                // If migrate fails or is not implemented, fall back to built-in defaults that
                // were already loaded above
                if( !MigrateFromLegacy( cfg.get() ) )
                {
                    success = false;
                    qDebug() << GetFullFilename() << ": migrated; not all settings were found in legacy file";
                }
                else
                {
                    success = true;
                    qDebug() << GetFullFilename() << ": migrated from legacy format";
                }

                if( backed_up )
                {
                    cfg.reset();

                    if( !QFile::copy( tempPath, aPath.filePath() ) )
                    {
                        qDebug() << "migrate; copy temp file" << tempPath << "to" << aPath.filePath() << "failed";
                    }

                    if( !QFile::remove( tempPath ) )
                    {
                        qDebug() << "migrate; failed to remove temp file" << tempPath;
                    }
                 }

                // Either way, we want to clean up the old file afterwards
                legacy_migrated = true;
            };

    QFileInfo path;

    if( aDirectory.isEmpty() )
    {
        path = QFileInfo( m_filename + "." + getFileExt() );
    }
    else
    {
        path = QFileInfo( QDir(aDirectory), m_filename + "." + getFileExt() );
    }

    if( !path.exists() )
    {
        // Case 1: legacy migration, no .json extension yet
        QString legacyPath = path.path() + "/" + path.baseName() + "." + getLegacyFileExt();
        QFileInfo legacyInfo(legacyPath);

        if( legacyInfo.exists() )
        {
            migrateFromLegacy( legacyInfo );
        }
        // Case 2: legacy filename is different from new one
        else if( !m_legacy_filename.isEmpty() )
        {
            QString altLegacyPath = path.path() + "/" + m_legacy_filename + "." + getLegacyFileExt();
            QFileInfo altLegacyInfo(altLegacyPath);

            if( altLegacyInfo.exists() )
                migrateFromLegacy( altLegacyInfo );
        }
        else
        {
            success = false;
        }
    }
    else
    {
        if( !path.isWritable() )
            m_writeFile = false;

        try
        {
            QFile fp( path.filePath() );
            
            if( fp.open( QIODevice::ReadOnly | QIODevice::Text ) )
            {
                QTextStream stream(&fp);
                std::string content = stream.readAll().toStdString();
                *static_cast<nlohmann::json*>( m_internals.get() ) =
                        nlohmann::json::parse( content, nullptr,
                                               /* allow_exceptions = */ true,
                                               /* ignore_comments  = */ true );

                // Save whatever we loaded, before doing any migration etc
                m_internals->m_original = *static_cast<nlohmann::json*>( m_internals.get() );

                // If parse succeeds, check if schema migration is required
                int filever = -1;

                try
                {
                    filever = m_internals->Get<int>( "meta.version" );
                }
                catch( ... )
                {
                    qDebug() << GetFullFilename() << ": file version could not be read!";
                    success = false;
                }

                if( filever >= 0 && filever < m_schemaVersion )
                {
                    qDebug() << GetFullFilename() << ": attempting migration from version" 
                             << filever << "to" << m_schemaVersion;

                    if( Migrate() )
                    {
                        migrated = true;
                    }
                    else
                    {
                        qDebug() << GetFullFilename() << ": migration failed!";
                    }
                }
                else if( filever > m_schemaVersion )
                {
                    qDebug() << GetFullFilename() << ": warning: file version" << filever 
                             << "is newer than latest (" << m_schemaVersion << ")";
                    m_isFutureFormat = true;
                }
            }
            else
            {
                qDebug() << GetFullFilename() << "exists but can't be opened for read";
            }
        }
        catch( nlohmann::json::parse_error& error )
        {
            success = false;
            qDebug() << "Json parse error reading" << path.filePath() << ":" << error.what();
            qDebug() << "Attempting migration in case file is in legacy format";
            migrateFromLegacy( path );
        }
    }

    // Now that we have new data in the JSON structure, load the params again
    Load();

    // And finally load any nested settings
    for( NESTED_SETTINGS* settings : m_nested_settings )
        settings->LoadFromFile();

    qDebug() << "Loaded <" << GetFullFilename() << "> with schema" << m_schemaVersion;

    m_modified = false;

    // If we migrated, clean up the legacy file (with no extension)
    if( m_writeFile && ( legacy_migrated || migrated ) )
    {
        if( legacy_migrated && m_deleteLegacyAfterMigration && !QFile::remove( path.filePath() ) )
        {
            qDebug() << "Warning: could not remove legacy file" << path.filePath();
        }

        // And write-out immediately so that we don't lose data if the program later crashes.
        if( m_deleteLegacyAfterMigration )
            SaveToFile( aDirectory, true );
    }

    return success;
}


bool JSON_SETTINGS::Store()
{
    for( PARAM_BASE* param : m_params )
    {
        m_modified |= !param->MatchesFile( *this );
        param->Store( this );
    }

    return m_modified;
}


void JSON_SETTINGS::ResetToDefaults()
{
    for( PARAM_BASE* param : m_params )
        param->SetDefault();
}


bool JSON_SETTINGS::SaveToFile( const QString& aDirectory, bool aForce )
{
    if( !m_writeFile )
        return false;

    // Default PROJECT won't have a filename set
    if( m_filename.isEmpty() )
        return false;

    QFileInfo path;

    if( aDirectory.isEmpty() )
    {
        path = QFileInfo( m_filename + "." + getFileExt() );
    }
    else
    {
        path = QFileInfo( QDir(aDirectory), m_filename + "." + getFileExt() );
    }

    if( !m_createIfMissing && !path.exists() )
    {
        qDebug() << "File for" << GetFullFilename() << "doesn't exist and m_createIfMissing == false; not saving";
        return false;
    }

    // Ensure the path exists, and create it if not.
    QDir dir = path.dir();
    if( !dir.exists() && !dir.mkpath(".") )
    {
        qDebug() << "Warning: could not create path" << dir.path() << ", can't save" << GetFullFilename();
        return false;
    }

    if( ( path.exists() && !path.isWritable() ) ||
        ( !path.exists() && !QFileInfo(path.dir().path()).isWritable() ) )
    {
        qDebug() << "File for" << GetFullFilename() << "is read-only; not saving";
        return false;
    }

    bool modified = false;

    for( NESTED_SETTINGS* settings : m_nested_settings )
    {
        if( !settings ) continue;

        modified |= settings->SaveToFile();
    }

    modified |= Store();

    if( !modified && !aForce && path.exists() )
    {
        qDebug() << GetFullFilename() << "contents not modified, skipping save";
        return false;
    }
    else if( !modified && !aForce && !m_createIfDefault )
    {
        qDebug() << GetFullFilename() << "contents still default and m_createIfDefault == false; not saving";
        return false;
    }

    qDebug() << "Saving" << GetFullFilename();

    LOCALE_IO dummy;
    bool success = true;

    nlohmann::json toSave = m_internals->m_original;


    for( PARAM_BASE* param : m_params )
    {
        if( param->ClearUnknownKeys() )
        {
            nlohmann::json_pointer p
                    = JSON_SETTINGS_INTERNALS::PointerFromString( param->GetJsonPath() );

            toSave[p] = nlohmann::json( {} );
        }
    }

    toSave.update( m_internals->begin(), m_internals->end(), /* merge_objects = */ true );

    try
    {
        std::stringstream buffer;
        buffer << std::setw( 2 ) << toSave << std::endl;

        QFile fileStream( path.filePath() );

        if( !fileStream.open( QIODevice::WriteOnly ) ||
            fileStream.write( buffer.str().c_str(), buffer.str().size() ) != (qint64)buffer.str().size() )
        {
            qDebug() << "Warning: could not save" << GetFullFilename();
            success = false;
        }
    }
    catch( nlohmann::json::exception& error )
    {
        qDebug() << "Catch error: could not save" << GetFullFilename() << ". Json error" << error.what();
        success = false;
    }
    catch( ... )
    {
        qDebug() << "Error: could not save" << GetFullFilename();
        success = false;
    }

    if( success )
        m_modified = false;

    return success;
}


const std::string JSON_SETTINGS::FormatAsString()
{
    Store();

    LOCALE_IO dummy;

    std::stringstream buffer;
    buffer << std::setw( 2 ) << *m_internals << std::endl;

    return buffer.str();
}


bool JSON_SETTINGS::LoadFromRawFile( const QString& aPath )
{
    try
    {
        QFile fp( aPath );

        if( fp.open( QIODevice::ReadOnly | QIODevice::Text ) )
        {
            QTextStream stream(&fp);
            std::string content = stream.readAll().toStdString();
            *static_cast<nlohmann::json*>( m_internals.get() ) =
                    nlohmann::json::parse( content, nullptr,
                                           /* allow_exceptions = */ true,
                                           /* ignore_comments  = */ true );
        }
        else
        {
            return false;
        }
    }
    catch( nlohmann::json::parse_error& error )
    {
        qDebug() << "Json parse error reading" << aPath << ":" << error.what();

        return false;
    }

    // Now that we have new data in the JSON structure, load the params again
    Load();
    return true;
}


std::optional<nlohmann::json> JSON_SETTINGS::GetJson( const std::string& aPath ) const
{
    nlohmann::json::json_pointer ptr = m_internals->PointerFromString( aPath );

    if( m_internals->contains( ptr ) )
    {
        try
        {
            return std::optional<nlohmann::json>{ m_internals->at( ptr ) };
        }
        catch( ... )
        {
        }
    }

    return std::optional<nlohmann::json>{};
}


template<typename ValueType>
std::optional<ValueType> JSON_SETTINGS::Get( const std::string& aPath ) const
{
    if( std::optional<nlohmann::json> ret = GetJson( aPath ) )
    {
        try
        {
            return ret->get<ValueType>();
        }
        catch( ... )
        {
        }
    }

    return std::nullopt;
}


// Instantiate all required templates here to allow reducing scope of json.hpp
template KICOMMON_API std::optional<bool>
                      JSON_SETTINGS::Get<bool>( const std::string& aPath ) const;
template KICOMMON_API std::optional<double>
                      JSON_SETTINGS::Get<double>( const std::string& aPath ) const;
template KICOMMON_API std::optional<float>
                      JSON_SETTINGS::Get<float>( const std::string& aPath ) const;
template KICOMMON_API std::optional<int> JSON_SETTINGS::Get<int>( const std::string& aPath ) const;
template KICOMMON_API std::optional<unsigned int>
                      JSON_SETTINGS::Get<unsigned int>( const std::string& aPath ) const;
template KICOMMON_API std::optional<unsigned long long>
                      JSON_SETTINGS::Get<unsigned long long>( const std::string& aPath ) const;
template KICOMMON_API std::optional<std::string>
                      JSON_SETTINGS::Get<std::string>( const std::string& aPath ) const;
template KICOMMON_API std::optional<nlohmann::json>
                      JSON_SETTINGS::Get<nlohmann::json>( const std::string& aPath ) const;
template KICOMMON_API std::optional<KIGFX::COLOR4D>
                      JSON_SETTINGS::Get<KIGFX::COLOR4D>( const std::string& aPath ) const;
template KICOMMON_API std::optional<BOM_FIELD>
                      JSON_SETTINGS::Get<BOM_FIELD>( const std::string& aPath ) const;
template KICOMMON_API std::optional<BOM_PRESET>
                      JSON_SETTINGS::Get<BOM_PRESET>( const std::string& aPath ) const;
template KICOMMON_API std::optional<BOM_FMT_PRESET>
                      JSON_SETTINGS::Get<BOM_FMT_PRESET>( const std::string& aPath ) const;
template KICOMMON_API std::optional<GRID>
                      JSON_SETTINGS::Get<GRID>( const std::string& aPath ) const;
template KICOMMON_API std::optional<QPoint>
                      JSON_SETTINGS::Get<QPoint>( const std::string& aPath ) const;
template KICOMMON_API std::optional<QSize>
                      JSON_SETTINGS::Get<QSize>( const std::string& aPath ) const;
template KICOMMON_API std::optional<QRect>
                      JSON_SETTINGS::Get<QRect>( const std::string& aPath ) const;

template<typename ValueType>
void JSON_SETTINGS::Set( const std::string& aPath, ValueType aVal )
{
    m_internals->SetFromString( aPath, std::move( aVal ) );
}


// Instantiate all required templates here to allow reducing scope of json.hpp
template KICOMMON_API void JSON_SETTINGS::Set<bool>( const std::string& aPath, bool aValue );
template KICOMMON_API void JSON_SETTINGS::Set<double>( const std::string& aPath, double aValue );
template KICOMMON_API void JSON_SETTINGS::Set<float>( const std::string& aPath, float aValue );
template KICOMMON_API void JSON_SETTINGS::Set<int>( const std::string& aPath, int aValue );
template KICOMMON_API void JSON_SETTINGS::Set<unsigned int>( const std::string& aPath,
                                                             unsigned int       aValue );
template KICOMMON_API void JSON_SETTINGS::Set<unsigned long long>( const std::string& aPath,
                                                                   unsigned long long aValue );
template KICOMMON_API void JSON_SETTINGS::Set<const char*>( const std::string& aPath,
                                                            const char*        aValue );
template KICOMMON_API void JSON_SETTINGS::Set<std::string>( const std::string& aPath,
                                                            std::string        aValue );
template KICOMMON_API void JSON_SETTINGS::Set<nlohmann::json>( const std::string& aPath,
                                                               nlohmann::json     aValue );
template KICOMMON_API void JSON_SETTINGS::Set<KIGFX::COLOR4D>( const std::string& aPath,
                                                               KIGFX::COLOR4D     aValue );
template KICOMMON_API void JSON_SETTINGS::Set<BOM_FIELD>( const std::string& aPath,
                                                          BOM_FIELD          aValue );
template KICOMMON_API void JSON_SETTINGS::Set<BOM_PRESET>( const std::string& aPath,
                                                           BOM_PRESET         aValue );
template KICOMMON_API void JSON_SETTINGS::Set<BOM_FMT_PRESET>( const std::string& aPath,
                                                               BOM_FMT_PRESET     aValue );
template KICOMMON_API void JSON_SETTINGS::Set<GRID>( const std::string& aPath, GRID aValue );
template KICOMMON_API void JSON_SETTINGS::Set<QPoint>( const std::string& aPath, QPoint aValue );
template KICOMMON_API void JSON_SETTINGS::Set<QSize>( const std::string& aPath, QSize aValue );
template KICOMMON_API void JSON_SETTINGS::Set<QRect>( const std::string& aPath, QRect aValue );


void JSON_SETTINGS::registerMigration( int aOldSchemaVersion, int aNewSchemaVersion,
                                       std::function<bool()> aMigrator )
{
    Q_ASSERT( aNewSchemaVersion > aOldSchemaVersion );
    Q_ASSERT( aNewSchemaVersion <= m_schemaVersion );
    m_migrators[aOldSchemaVersion] = std::make_pair( aNewSchemaVersion, aMigrator );
}


bool JSON_SETTINGS::Migrate()
{
    int filever = m_internals->Get<int>( "meta.version" );

    while( filever < m_schemaVersion )
    {
        Q_ASSERT( m_migrators.count( filever ) > 0 );

        if( !m_migrators.count( filever ) )
        {
            qDebug() << "Migrator missing for" << typeid( *this ).name() << "version" << filever << "!";
            return false;
        }

        std::pair<int, std::function<bool()>> pair = m_migrators.at( filever );

        if( pair.second() )
        {
            qDebug() << "Migrated" << typeid( *this ).name() << "from" << filever << "to" << pair.first;
            filever = pair.first;
            m_internals->At( "meta.version" ) = filever;
        }
        else
        {
            qDebug() << "Migration failed for" << typeid( *this ).name() << "from" << filever << "to" << pair.first;
            return false;
        }
    }

    return true;
}


bool JSON_SETTINGS::MigrateFromLegacy( QSettings* aLegacyConfig )
{
    qDebug() << "MigrateFromLegacy() not implemented for" << typeid( *this ).name();
    return false;
}


bool JSON_SETTINGS::SetIfPresent( const nlohmann::json& aObj, const std::string& aPath,
                                  QString& aTarget )
{
    nlohmann::json::json_pointer ptr = JSON_SETTINGS_INTERNALS::PointerFromString( aPath );

    if( aObj.contains( ptr ) && aObj.at( ptr ).is_string() )
    {
        aTarget = QString::fromStdString( aObj.at( ptr ).get<std::string>() );
        return true;
    }

    return false;
}


bool JSON_SETTINGS::SetIfPresent( const nlohmann::json& aObj, const std::string& aPath,
                                  bool& aTarget )
{
    nlohmann::json::json_pointer ptr = JSON_SETTINGS_INTERNALS::PointerFromString( aPath );

    if( aObj.contains( ptr ) && aObj.at( ptr ).is_boolean() )
    {
        aTarget = aObj.at( ptr ).get<bool>();
        return true;
    }

    return false;
}


bool JSON_SETTINGS::SetIfPresent( const nlohmann::json& aObj, const std::string& aPath,
                                  int& aTarget )
{
    nlohmann::json::json_pointer ptr = JSON_SETTINGS_INTERNALS::PointerFromString( aPath );

    if( aObj.contains( ptr ) && aObj.at( ptr ).is_number_integer() )
    {
        aTarget = aObj.at( ptr ).get<int>();
        return true;
    }

    return false;
}


bool JSON_SETTINGS::SetIfPresent( const nlohmann::json& aObj, const std::string& aPath,
                                  unsigned int& aTarget )
{
    nlohmann::json::json_pointer ptr = JSON_SETTINGS_INTERNALS::PointerFromString( aPath );

    if( aObj.contains( ptr ) && aObj.at( ptr ).is_number_unsigned() )
    {
        aTarget = aObj.at( ptr ).get<unsigned int>();
        return true;
    }

    return false;
}


template<typename ValueType>
bool JSON_SETTINGS::fromLegacy( QSettings* aConfig, const std::string& aKey,
                             const std::string& aDest )
{
    if( !aConfig )
        return false;

    if( aConfig->contains( QString::fromStdString( aKey ) ) )
    {
        ValueType val = aConfig->value( QString::fromStdString( aKey ) ).value<ValueType>();
        
        try
        {
            ( *m_internals )[aDest] = val;
        }
        catch( ... )
        {
            Q_ASSERT_X( false, "JSON_SETTINGS::fromLegacy", "Could not write value in fromLegacy!" );
            return false;
        }

        return true;
    }

    return false;
}


// Explicitly declare these because we only support a few types anyway, and it means we can keep
// QSettings detail out of the header file
template
KICOMMON_API bool JSON_SETTINGS::fromLegacy<int>( QSettings*, const std::string&,
                                                  const std::string& );

template
KICOMMON_API bool JSON_SETTINGS::fromLegacy<double>( QSettings*, const std::string&,
                                                     const std::string& );

template
KICOMMON_API bool JSON_SETTINGS::fromLegacy<bool>( QSettings*, const std::string&,
                                                   const std::string& );


bool JSON_SETTINGS::fromLegacyString( QSettings* aConfig, const std::string& aKey,
                                      const std::string& aDest )
{
    if( !aConfig )
        return false;

    if( aConfig->contains( QString::fromStdString( aKey ) ) )
    {
        QString str = aConfig->value( QString::fromStdString( aKey ) ).toString();
        
        try
        {
            ( *m_internals )[aDest] = str.toStdString();
        }
        catch( ... )
        {
            Q_ASSERT_X( false, "JSON_SETTINGS::fromLegacyString", "Could not write value in fromLegacyString!" );
            return false;
        }

        return true;
    }

    return false;
}


bool JSON_SETTINGS::fromLegacyColor( QSettings* aConfig, const std::string& aKey,
    const std::string& aDest )
{
    if( !aConfig )
        return false;

    if( aConfig->contains( QString::fromStdString( aKey ) ) )
    {
        QString str = aConfig->value( QString::fromStdString( aKey ) ).toString();
        KIGFX::COLOR4D color;
        color.SetFromQString( str );

        try
        {
            nlohmann::json js = nlohmann::json::array( { color.r, color.g, color.b, color.a } );
            ( *m_internals )[aDest] = std::move( js );
        }
        catch( ... )
        {
            Q_ASSERT_X( false, "JSON_SETTINGS::fromLegacyColor", "Could not write value in fromLegacyColor!" );
            return false;
        }

        return true;
    }

    return false;
}


void JSON_SETTINGS::AddNestedSettings( NESTED_SETTINGS* aSettings )
{
    qDebug() << "AddNestedSettings" << aSettings->GetFilename();
    m_nested_settings.push_back( aSettings );
}


void JSON_SETTINGS::ReleaseNestedSettings( NESTED_SETTINGS* aSettings )
{
    if( !aSettings || !m_manager )
        return;

    auto it = std::find_if( m_nested_settings.begin(), m_nested_settings.end(),
                            [&aSettings]( const JSON_SETTINGS* aPtr )
                            {
                                return aPtr == aSettings;
                            } );

    if( it != m_nested_settings.end() )
    {
        qDebug() << "Flush and release" << ( *it )->GetFilename();
        m_modified |= ( *it )->SaveToFile();
        m_nested_settings.erase( it );
    }

    aSettings->SetParent( nullptr );
}


// Specializations to allow conversion between QString and std::string via JSON_SETTINGS API
template<>
std::optional<QString> JSON_SETTINGS::Get( const std::string& aPath ) const
{
    if( std::optional<nlohmann::json> opt_json = GetJson( aPath ) )
        return QString::fromStdString( opt_json->get<std::string>() );

    return std::nullopt;
}


template<>
void JSON_SETTINGS::Set<QString>( const std::string& aPath, QString aVal )
{
    ( *m_internals )[aPath] = aVal.toStdString();
}


template<typename ResultType>
ResultType JSON_SETTINGS::fetchOrDefault( const nlohmann::json& aJson, const std::string& aKey,
                                          ResultType aDefault )
{
    ResultType ret = std::move( aDefault );

    try
    {
        if( aJson.contains( aKey ) )
            ret = aJson.at( aKey ).get<ResultType>();
    }
    catch( ... )
    {
    }

    return ret;
}


template
KICOMMON_API std::string JSON_SETTINGS::fetchOrDefault( const nlohmann::json& aJson,
                                                        const std::string& aKey,
                                                        std::string aDefault );


template
KICOMMON_API bool JSON_SETTINGS::fetchOrDefault( const nlohmann::json& aJson,
                                                 const std::string& aKey, bool aDefault );
