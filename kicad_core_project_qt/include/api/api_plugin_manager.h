
#include <deque>
#include <memory>
#include <set>

#include <QObject>
#include <QTimer>

#include <api/api_plugin.h>
#include <json_schema_validator.h>
#include <kicommon.h>




/**
 * Responsible for loading plugin definitions for API-based plugins (ones that do not run inside
 * KiCad itself, but instead are launched as external processes by KiCad)
 */
class KICOMMON_API API_PLUGIN_MANAGER : public QObject
{
public:
    API_PLUGIN_MANAGER( QObject* aParent );

    void ReloadPlugins();

    void RecreatePluginEnvironment( const QString& aIdentifier );

    void InvokeAction( const QString& aIdentifier );

    std::optional<const PLUGIN_ACTION*> GetAction( const QString& aIdentifier );

    std::vector<const PLUGIN_ACTION*> GetActionsForScope( PLUGIN_ACTION_SCOPE aScope );

    std::map<int, QString>& ButtonBindings() { return m_buttonBindings; }

    std::map<int, QString>& MenuBindings() { return m_menuBindings; }

private:
    void processPluginDependencies();

    void processNextJob();

    QObject* m_parent;

    std::set<std::unique_ptr<API_PLUGIN>, CompareApiPluginIdentifiers> m_plugins;

    std::map<QString, const API_PLUGIN*> m_pluginsCache;

    std::map<QString, const PLUGIN_ACTION*> m_actionsCache;

    std::map<QString, QString> m_environmentCache;

    std::map<int, QString> m_buttonBindings;

    std::map<int, QString> m_menuBindings;

    std::set<QString> m_readyPlugins;

    std::set<QString> m_busyPlugins;

    enum class JOB_TYPE
    {
        CREATE_ENV,
        SETUP_ENV,
        INSTALL_REQUIREMENTS
    };

    struct JOB
    {
        JOB_TYPE type;
        QString identifier;
        QString plugin_path;
        QString env_path;
    };

    std::deque<JOB> m_jobs;

    std::unique_ptr<JSON_SCHEMA_VALIDATOR> m_schema_validator;

    long m_lastPid;
    QTimer* m_raiseTimer;
};
