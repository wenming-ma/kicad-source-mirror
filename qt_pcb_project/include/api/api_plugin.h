
#ifndef KICAD_API_PLUGIN_H
#define KICAD_API_PLUGIN_H

#include <memory>
#include <optional>
#include <set>
#include <nlohmann/json_fwd.hpp>
#include <QPixmap>
#include <QString>
#include <QVector>

#include <kicommon.h>


struct API_PLUGIN_CONFIG;
class API_PLUGIN;
class JSON_SCHEMA_VALIDATOR;


struct PLUGIN_DEPENDENCY
{
    QString package_name;
    QString version;
};


enum class PLUGIN_RUNTIME_TYPE
{
    INVALID,
    PYTHON,
    EXEC
};


enum class PLUGIN_ACTION_SCOPE
{
    INVALID,
    PCB,
    SCHEMATIC,
    FOOTPRINT,
    SYMBOL,
    PROJECT_MANAGER
};


struct PLUGIN_RUNTIME
{
    bool FromJson( const nlohmann::json& aJson );

    PLUGIN_RUNTIME_TYPE type;
    QString min_version;
    QVector<PLUGIN_DEPENDENCY> dependencies;
};


struct PLUGIN_ACTION
{
    PLUGIN_ACTION( const API_PLUGIN& aPlugin ) :
            plugin( aPlugin )
    {}

    QString identifier;
    QString name;
    QString description;
    bool show_button = false;
    QString entrypoint;
    std::set<PLUGIN_ACTION_SCOPE> scopes;
    QVector<QString> args;
    QPixmap icon_light;
    QPixmap icon_dark;

    const API_PLUGIN& plugin;
};

class KICOMMON_API API_PLUGIN
{
public:
    API_PLUGIN( const QString& aConfigFile, const JSON_SCHEMA_VALIDATOR& aValidator );

    ~API_PLUGIN();

    bool IsOk() const;

    static bool IsValidIdentifier( const QString& aIdentifier );

    const QString& Identifier() const;
    const QString& Name() const;
    const QString& Description() const;
    const PLUGIN_RUNTIME& Runtime() const;
    QString BasePath() const;

    const QVector<PLUGIN_ACTION>& Actions() const;

    QString ActionSettingsKey( const PLUGIN_ACTION& aAction ) const;

private:
    friend struct API_PLUGIN_CONFIG;

    std::optional<PLUGIN_ACTION> createActionFromJson( const nlohmann::json& aJson );

    QString m_configFile;

    std::unique_ptr<API_PLUGIN_CONFIG> m_config;
};

/**
 * Comparison functor for ensuring API_PLUGINs have unique identifiers
 */
struct CompareApiPluginIdentifiers
{
    bool operator()( const std::unique_ptr<API_PLUGIN>& item1,
                     const std::unique_ptr<API_PLUGIN>& item2 ) const
    {
        return item1->Identifier() < item2->Identifier();
    }
};

#endif //KICAD_API_PLUGIN_H
