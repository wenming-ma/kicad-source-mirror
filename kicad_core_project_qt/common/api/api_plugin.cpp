
#include <magic_enum.hpp>
#include <json_common.h>
#include <vector>
#include <QLoggingCategory>
#include <QRegularExpression>
#include <QTextStream>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QPixmap>
#include <QIODevice>

#include <api/api_plugin.h>
#include <api/api_plugin_manager.h>
#include <json_conversions.h>
#include <json_schema_validator.h>


class LOGGING_ERROR_HANDLER : public nlohmann::json_schema::error_handler
{
public:
    LOGGING_ERROR_HANDLER() : m_hasError( false ) {}

    bool HasError() const { return m_hasError; }

    void error( const nlohmann::json::json_pointer& ptr, const nlohmann::json& instance,
                const std::string& message ) override
    {
        m_hasError = true;
        qDebug() << QString("JSON error: at %1, value:\n%2\n%3")
                        .arg(QString::fromStdString(ptr.to_string()))
                        .arg(QString::fromStdString(instance.dump()))
                        .arg(QString::fromStdString(message));
    }

private:
    bool m_hasError;
};


bool PLUGIN_RUNTIME::FromJson( const nlohmann::json& aJson )
{

    try
    {
        type = magic_enum::enum_cast<PLUGIN_RUNTIME_TYPE>( aJson.at( "type" ).get<std::string>(),
                                                           magic_enum::case_insensitive )
                       .value_or( PLUGIN_RUNTIME_TYPE::INVALID );


    }
    catch( ... )
    {
        return false;
    }

    return type != PLUGIN_RUNTIME_TYPE::INVALID;
}


struct API_PLUGIN_CONFIG
{
    API_PLUGIN_CONFIG( API_PLUGIN& aParent, const QString& aConfigFile,
                       const JSON_SCHEMA_VALIDATOR& aValidator );

    bool valid;
    QString identifier;
    QString name;
    QString description;
    PLUGIN_RUNTIME runtime;
    std::vector<PLUGIN_ACTION> actions;

    API_PLUGIN& parent;
};


API_PLUGIN_CONFIG::API_PLUGIN_CONFIG( API_PLUGIN& aParent, const QString& aConfigFile,
                                      const JSON_SCHEMA_VALIDATOR& aValidator ) :
        parent( aParent )
{
    valid = false;

    QFile file(aConfigFile);
    if( !file.exists() || !file.open(QIODevice::ReadOnly | QIODevice::Text) )
        return;

    qDebug() << "Plugin: parsing config file";

    QTextStream stream(&file);

    nlohmann::json js;

    try
    {
        QString jsonText = stream.readAll();
        js = nlohmann::json::parse( jsonText.toStdString(), nullptr,
                                    /* allow_exceptions = */ true,
                                    /* ignore_comments  = */ true );
    }
    catch( ... )
    {
        qDebug() << "Plugin: exception during parse";
        return;
    }

    LOGGING_ERROR_HANDLER handler;
    aValidator.Validate( js, handler, nlohmann::json_uri( "#/definitions/Plugin" ) );

    if( !handler.HasError() )
        qDebug() << "Plugin: schema validation successful";

    try
    {
        identifier = QString::fromStdString(js.at( "identifier" ).get<std::string>());
        name = QString::fromStdString(js.at( "name" ).get<std::string>());
        description = QString::fromStdString(js.at( "description" ).get<std::string>());

        if( !runtime.FromJson( js.at( "runtime" ) ) )
        {
            qDebug() << "Plugin: error parsing runtime section";
            return;
        }
    }
    catch( ... )
    {
        qDebug() << "Plugin: exception while parsing required keys";
        return;
    }

    if( !API_PLUGIN::IsValidIdentifier( identifier ) )
    {
        qDebug() << QString("Plugin: identifier %1 does not meet requirements").arg(identifier);
        return;
    }

    qDebug() << QString("Plugin: %1 (%2)").arg(identifier, name);

    try
    {
        const nlohmann::json& actionsJs = js.at( "actions" );

        if( actionsJs.is_array() )
        {
            for( const nlohmann::json& actionJs : actionsJs )
            {
                if( std::optional<PLUGIN_ACTION> a = parent.createActionFromJson( actionJs ) )
                {
                    a->identifier = QString("%1.%2").arg(identifier, a->identifier);
                    qDebug() << QString("Plugin: loaded action %1").arg(a->identifier);
                    actions.emplace_back( *a );
                }
            }
        }
    }
    catch( ... )
    {
        qDebug() << "Plugin: exception while parsing actions";
    }

    valid = true;
}


API_PLUGIN::API_PLUGIN( const QString& aConfigFile, const JSON_SCHEMA_VALIDATOR& aValidator ) :
        m_configFile( aConfigFile ),
        m_config( std::make_unique<API_PLUGIN_CONFIG>( *this, aConfigFile, aValidator ) )
{
}


API_PLUGIN::~API_PLUGIN()
{
}


bool API_PLUGIN::IsOk() const
{
    return m_config->valid;
}


bool API_PLUGIN::IsValidIdentifier( const QString& aIdentifier )
{
    QRegularExpression identifierRegex( "[\\w\\d]{2,}\\.[\\w\\d]+\\.[\\w\\d]+" );
    return identifierRegex.match( aIdentifier ).hasMatch();
}


const QString& API_PLUGIN::Identifier() const
{
    return m_config->identifier;
}


const QString& API_PLUGIN::Name() const
{
    return m_config->name;
}


const QString& API_PLUGIN::Description() const
{
    return m_config->description;
}


const PLUGIN_RUNTIME& API_PLUGIN::Runtime() const
{
    return m_config->runtime;
}


const std::vector<PLUGIN_ACTION>& API_PLUGIN::Actions() const
{
    return m_config->actions;
}


QString API_PLUGIN::BasePath() const
{
    QFileInfo fileInfo(m_configFile);
    return fileInfo.absolutePath();
}


QString API_PLUGIN::ActionSettingsKey( const PLUGIN_ACTION& aAction ) const
{
    return Identifier() + "." + aAction.identifier;
}



std::optional<PLUGIN_ACTION> API_PLUGIN::createActionFromJson( const nlohmann::json& aJson )
{
    PLUGIN_ACTION action( *this );

    try
    {
        action.identifier = QString::fromStdString(aJson.at( "identifier" ).get<std::string>());
        qDebug() << QString("Plugin: load action %1").arg(action.identifier);
        action.name = QString::fromStdString(aJson.at( "name" ).get<std::string>());
        action.description = QString::fromStdString(aJson.at( "description" ).get<std::string>());
        action.entrypoint = QString::fromStdString(aJson.at( "entrypoint" ).get<std::string>());
        action.show_button = aJson.contains( "show-button" ) && aJson.at( "show-button" ).get<bool>();
    }
    catch( ... )
    {
        qDebug() << "Plugin: exception while parsing action required keys";
        return std::nullopt;
    }

    QFileInfo f( action.entrypoint );

    if( f.isAbsolute() )
    {
        qDebug() << QString("Plugin: action contains abs path %1; skipping").arg(action.entrypoint);
        return std::nullopt;
    }

    QFileInfo configFileInfo(m_configFile);
    QString absolutePath = QDir(configFileInfo.absolutePath()).absoluteFilePath(action.entrypoint);
    QFileInfo absoluteFileInfo(absolutePath);

    if( !absoluteFileInfo.exists() || !absoluteFileInfo.isReadable() )
    {
        qDebug() << QString("WARNING: action entrypoint %1 is not readable").arg(absolutePath);
    }

    if( aJson.contains( "args" ) && aJson.at( "args" ).is_array() )
    {
        for( const nlohmann::json& argJs : aJson.at( "args" ) )
        {
            try
            {
                action.args.append( QString::fromStdString(argJs.get<std::string>()) );
            }
            catch( ... )
            {
                qDebug() << "Plugin: exception while parsing action args";
                continue;
            }
        }
    }

    if( aJson.contains( "scopes" ) && aJson.at( "scopes" ).is_array() )
    {
        for( const nlohmann::json& scopeJs : aJson.at( "scopes" ) )
        {
            try
            {
                action.scopes.insert( magic_enum::enum_cast<PLUGIN_ACTION_SCOPE>(
                        scopeJs.get<std::string>(), magic_enum::case_insensitive )
                       .value_or( PLUGIN_ACTION_SCOPE::INVALID ) );
            }
            catch( ... )
            {
                qDebug() << "Plugin: exception while parsing action scopes";
                continue;
            }
        }
    }

    auto handleBitmap =
            [&]( const std::string& aKey, QPixmap& aDest )
            {
                if( aJson.contains( aKey ) && aJson.at( aKey ).is_array() )
                {
                    std::vector<QPixmap> bitmaps;

                    for( const nlohmann::json& iconJs : aJson.at( aKey ) )
                    {
                        QString iconFile;

                        try
                        {
                            iconFile = QString::fromStdString(iconJs.get<std::string>());
                        }
                        catch( ... )
                        {
                            continue;
                        }

                        QFileInfo configFileInfo(m_configFile);
                        QString absoluteIconPath = QDir(configFileInfo.absolutePath()).absoluteFilePath(iconFile);

                        qDebug() << QString("Plugin: action %1: loading icon %2")
                                        .arg(action.identifier, absoluteIconPath);

                        QFileInfo iconFileInfo(absoluteIconPath);
                        if( !iconFileInfo.exists() || !iconFileInfo.isReadable() )
                        {
                            qDebug() << "Plugin: icon file could not be read";
                            continue;
                        }

                        QPixmap bmp;
                        bmp.load( absoluteIconPath );

                        if( !bmp.isNull() )
                            bitmaps.emplace_back( bmp );
                        else
                            qDebug() << "Plugin: icon file not a valid bitmap";
                    }

                    if( !bitmaps.empty() )
                        aDest = bitmaps.front();
                }
            };

    handleBitmap( "icons-light", action.icon_light );
    handleBitmap( "icons-dark", action.icon_dark );

    return action;
}
