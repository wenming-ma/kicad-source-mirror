#include <fstream>

#include <env_vars.h>
#include <fmt/format.h>
#include <QDir>
#include <QTimer>
#include <QCoreApplication>
#include <QLoggingCategory>
#include <QStandardPaths>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QProcessEnvironment>
#include <QDebug>

#include <api/api_plugin_manager.h>
#include <api/api_server.h>
#include <gestfich.h>
#include <paths.h>
#include <pgm_base.h>
#include <python_manager.h>
#include <settings/settings_manager.h>
#include <settings/common_settings.h>


API_PLUGIN_MANAGER::API_PLUGIN_MANAGER( QObject* aParent ) :
        QObject( aParent ),
        m_parent( aParent ),
        m_lastPid( 0 ),
        m_raiseTimer( nullptr )
{
    // Read and store pcm schema
    QString stockDataPath = QString::fromStdString( PATHS::GetStockDataPath( true ) );
    QString schemaFilePath = QDir( stockDataPath ).filePath( "schemas/api.v1.schema.json" );

    m_schema_validator = std::make_unique<JSON_SCHEMA_VALIDATOR>( schemaFilePath );
}


class PLUGIN_TRAVERSER
{
private:
    std::function<void( const QString& )> m_action;

public:
    explicit PLUGIN_TRAVERSER( std::function<void( const QString& )> aAction )
            : m_action( std::move( aAction ) )
    {
    }

    void traverse( const QString& dirPath )
    {
        QDir dir( dirPath );
        if( !dir.exists() )
            return;

        QDirIterator it( dirPath, QDirIterator::Subdirectories );
        while( it.hasNext() )
        {
            QString filePath = it.next();
            QFileInfo fileInfo( filePath );
            if( fileInfo.fileName() == "plugin.json" )
                m_action( filePath );
        }
    }
};


void API_PLUGIN_MANAGER::ReloadPlugins()
{
    m_plugins.clear();
    m_pluginsCache.clear();
    m_actionsCache.clear();
    m_environmentCache.clear();
    m_buttonBindings.clear();
    m_menuBindings.clear();
    m_readyPlugins.clear();

    PLUGIN_TRAVERSER loader(
            [&]( const QString& aFilePath )
            {
                qDebug() << "Manager: loading plugin from" << aFilePath;

                auto plugin = std::make_unique<API_PLUGIN>( aFilePath, *m_schema_validator );

                if( plugin->IsOk() )
                {
                    if( m_pluginsCache.count( plugin->Identifier() ) )
                    {
                        qDebug() << "Manager: identifier" << plugin->Identifier() << "already present!";
                        return;
                    }
                    else
                    {
                        m_pluginsCache[plugin->Identifier()] = plugin.get();
                    }

                    for( const PLUGIN_ACTION& action : plugin->Actions() )
                        m_actionsCache[action.identifier] = &action;

                    m_plugins.insert( std::move( plugin ) );
                }
                else
                {
                    qDebug() << "Manager: loading failed";
                }
            } );

    QString systemPluginsPath = QString::fromStdString( PATHS::GetStockPluginsPath() );
    QDir systemPluginsDir( systemPluginsPath );

    if( systemPluginsDir.exists() )
    {
        qDebug() << "Manager: scanning system path (" << systemPluginsPath << ") for plugins...";
        loader.traverse( systemPluginsPath );
    }

    QString thirdPartyPath;
    const ENV_VAR_MAP& env = Pgm().GetLocalEnvVariables();

    if( std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( env, "3RD_PARTY" ) )
        thirdPartyPath = *v;
    else
        thirdPartyPath = QString::fromStdString( PATHS::GetDefault3rdPartyPath() );

    QDir thirdParty( thirdPartyPath );

    if( thirdParty.exists() )
    {
        qDebug() << "Manager: scanning PCM path (" << thirdPartyPath << ") for plugins...";
        loader.traverse( thirdPartyPath );
    }

    QString userPluginsPath = QString::fromStdString( PATHS::GetUserPluginsPath() );
    QDir userPluginsDir( userPluginsPath );

    if( userPluginsDir.exists() )
    {
        qDebug() << "Manager: scanning user path (" << userPluginsPath << ") for plugins...";
        loader.traverse( userPluginsPath );
    }

    processPluginDependencies();
}


void API_PLUGIN_MANAGER::RecreatePluginEnvironment( const QString& aIdentifier )
{
    if( !m_pluginsCache.contains( aIdentifier ) )
        return;

    const API_PLUGIN* plugin = m_pluginsCache.at( aIdentifier );
    if( !plugin )
        return;

    std::optional<QString> env = PYTHON_MANAGER::GetPythonEnvironment( plugin->Identifier() );
    if( !env.has_value() )
        return;

    QString envConfigPath = QDir( *env ).filePath( "pyvenv.cfg" );
    QFileInfo envConfigInfo( envConfigPath );

    QDir envDir( envConfigInfo.dir() );
    if( envDir.exists() && envDir.removeRecursively() )
    {
        qDebug() << "Manager: Removed existing Python environment at" << envConfigInfo.dir().path() << "for" << plugin->Identifier();

        JOB job;
        job.type = JOB_TYPE::CREATE_ENV;
        job.identifier = plugin->Identifier();
        job.plugin_path = plugin->BasePath();
        job.env_path = envConfigInfo.dir().path();
        m_jobs.emplace_back( job );
        processNextJob();
    }
}


std::optional<const PLUGIN_ACTION*> API_PLUGIN_MANAGER::GetAction( const QString& aIdentifier )
{
    if( !m_actionsCache.contains( aIdentifier ) )
        return std::nullopt;

    return m_actionsCache.at( aIdentifier );
}


void API_PLUGIN_MANAGER::InvokeAction( const QString& aIdentifier )
{
    if( !m_actionsCache.contains( aIdentifier ) )
        return;

    const PLUGIN_ACTION* action = m_actionsCache.at( aIdentifier );
    const API_PLUGIN& plugin = action->plugin;

    if( !m_readyPlugins.count( plugin.Identifier() ) )
    {
        qDebug() << "Manager: Plugin" << plugin.Identifier() << "is not ready";
        return;
    }

    QDir pluginBaseDir( plugin.BasePath() );
    QString pluginPath = pluginBaseDir.filePath( action->entrypoint );
    QFileInfo pluginFile( pluginPath );
    pluginPath = pluginFile.absoluteFilePath();

    QStringList args;
    std::optional<QString> py;

    switch( plugin.Runtime().type )
    {
    case PLUGIN_RUNTIME_TYPE::PYTHON:
    {
        py = PYTHON_MANAGER::GetVirtualPython( plugin.Identifier() );

        if( !py )
        {
            qDebug() << "Manager: Python interpreter for" << plugin.Identifier() << "not found";
            return;
        }

        if( !pluginFile.isReadable() )
        {
            qDebug() << "Manager: Python entrypoint" << pluginFile.absoluteFilePath() << "is not readable";
            return;
        }

        std::optional<QString> pythonHome =
                PYTHON_MANAGER::GetPythonEnvironment( plugin.Identifier() );

        PYTHON_MANAGER manager( *py );
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#ifdef _WIN32
        QString systemRoot = qEnvironmentVariable( "SYSTEMROOT" );
        env.insert( "SYSTEMROOT", systemRoot );

        QString pythonInterpreter = QString::fromStdString( Pgm().GetCommonSettings()->m_Api.python_interpreter );
        QString kicadPython = QString::fromStdString( FindKicadFile( "pythonw.exe" ) );
        if( pythonInterpreter == kicadPython || !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
        {
            qDebug() << "Configured Python is the KiCad one; erasing path overrides...";
            env.remove( "PYTHONHOME" );
            env.remove( "PYTHONPATH" );
        }
#endif

        if( pythonHome )
            env.insert( "VIRTUAL_ENV", *pythonHome );

        QStringList executeArgs = { pluginFile.absoluteFilePath() };
        [[maybe_unused]] qint64 pid = manager.Execute( executeArgs,
                []( int aRetVal, const QString& aOutput, const QString& aError )
                {
                    qDebug() << "Manager: action exited with code" << aRetVal;

                    if( !aError.isEmpty() )
                        qDebug() << "Manager: action stderr:" << aError;
                },
                &env, true );

#ifdef Q_OS_MACOS
        if( pid )
        {
            if( !m_raiseTimer )
            {
                m_raiseTimer = new QTimer( this );

                connect( m_raiseTimer, &QTimer::timeout, this,
                        [this]()
                        {
                            QString script = QString(
                                "tell application \"System Events\"\n"
                                "  set plist to every process whose unix id is %1\n"
                                "  repeat with proc in plist\n"
                                "    set the frontmost of proc to true\n"
                                "  end repeat\n"
                                "end tell" ).arg( m_lastPid );

                            QString cmd = QString( "osascript -e '%1'" ).arg( script );
                            qDebug() << "Execute:" << cmd;
                            QProcess::startDetached( cmd );
                        } );
            }

            m_lastPid = pid;
            m_raiseTimer->setSingleShot( true );
            m_raiseTimer->start( 250 );
        }
#endif

        break;
    }

    case PLUGIN_RUNTIME_TYPE::EXEC:
    {
        if( !pluginFile.isExecutable() )
        {
            qDebug() << "Manager: Exec entrypoint" << pluginFile.absoluteFilePath() << "is not executable";
            return;
        }

        args.append( pluginPath );

        for( const QString& arg : action->args )
            args.append( arg );

        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

        QProcess* process = new QProcess( this );
        process->setProcessEnvironment( env );
        process->setWorkingDirectory( pluginFile.dir().path() );
        
        qint64 pid = 0;
        if( process->startDetached( args.first(), args.mid(1), pluginFile.dir().path(), &pid ) )
        {
            qDebug() << "Manager: launching action" << action->identifier << "-> pid" << pid;
        }
        else
        {
            qDebug() << "Manager: launching action" << action->identifier << "failed";
        }
        
        process->deleteLater();
        break;
    }

    default:
        qDebug() << "Manager: unhandled runtime for action" << action->identifier;
        return;
    }
}


std::vector<const PLUGIN_ACTION*> API_PLUGIN_MANAGER::GetActionsForScope( PLUGIN_ACTION_SCOPE aScope )
{
    std::vector<const PLUGIN_ACTION*> actions;

    for( auto& [identifier, action] : m_actionsCache )
    {
        if( !m_readyPlugins.count( action->plugin.Identifier() ) )
            continue;

        if( action->scopes.count( aScope ) )
            actions.emplace_back( action );
    }

    return actions;
}


void API_PLUGIN_MANAGER::processPluginDependencies()
{
    bool addedAnyJobs = false;

    for( const std::unique_ptr<API_PLUGIN>& plugin : m_plugins )
    {
        if( m_busyPlugins.contains( plugin->Identifier() ) )
            continue;

        qDebug() << "Manager: processing dependencies for" << plugin->Identifier();
        m_environmentCache[plugin->Identifier()] = QString();

        if( plugin->Runtime().type != PLUGIN_RUNTIME_TYPE::PYTHON )
        {
            qDebug() << "Manager:" << plugin->Identifier() << "is not a Python plugin, all set";
            m_readyPlugins.insert( plugin->Identifier() );
            continue;
        }

        std::optional<QString> env = PYTHON_MANAGER::GetPythonEnvironment( plugin->Identifier() );

        if( !env )
        {
            qDebug() << "Manager: could not create env for" << plugin->Identifier();
            continue;
        }

        m_busyPlugins.insert( plugin->Identifier() );

        QString envConfigPath = QDir( *env ).filePath( "pyvenv.cfg" );
        QFileInfo envConfigInfo( envConfigPath );

        if( envConfigInfo.isReadable() )
        {
            qDebug() << "Manager: Python env for" << plugin->Identifier() << "exists at" << envConfigInfo.dir().path();
            JOB job;
            job.type = JOB_TYPE::INSTALL_REQUIREMENTS;
            job.identifier = plugin->Identifier();
            job.plugin_path = plugin->BasePath();
            job.env_path = envConfigInfo.dir().path();
            m_jobs.emplace_back( job );
            addedAnyJobs = true;
            continue;
        }

        qDebug() << "Manager: will create Python env for" << plugin->Identifier() << "at" << envConfigInfo.dir().path();
        JOB job;
        job.type = JOB_TYPE::CREATE_ENV;
        job.identifier = plugin->Identifier();
        job.plugin_path = plugin->BasePath();
        job.env_path = envConfigInfo.dir().path();
        m_jobs.emplace_back( job );
        addedAnyJobs = true;
    }

    if( addedAnyJobs )
    {
        processNextJob();
    }
}


void API_PLUGIN_MANAGER::processNextJob()
{
    if( m_jobs.empty() )
    {
        qDebug() << "Manager: no more jobs to process";
        return;
    }

    qDebug() << "Manager: begin processing;" << m_jobs.size() << "jobs left in queue";

    JOB& job = m_jobs.front();

    if( job.type == JOB_TYPE::CREATE_ENV )
    {
        qDebug() << "Manager: Using Python interpreter at" << QString::fromStdString( Pgm().GetCommonSettings()->m_Api.python_interpreter );
        qDebug() << "Manager: creating Python env at" << job.env_path;
        QString pythonInterpreter = QString::fromStdString( Pgm().GetCommonSettings()->m_Api.python_interpreter );
        PYTHON_MANAGER manager( pythonInterpreter );
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#ifdef _WIN32
        QString systemRoot = qEnvironmentVariable( "SYSTEMROOT" );
        env.insert( "SYSTEMROOT", systemRoot );

        QString kicadPython = QString::fromStdString( FindKicadFile( "pythonw.exe" ) );
        if( pythonInterpreter == kicadPython || !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
        {
            qDebug() << "Configured Python is the KiCad one; erasing path overrides...";
            env.remove( "PYTHONHOME" );
            env.remove( "PYTHONPATH" );
        }
#endif
        QStringList args = {
                "-m",
                "venv",
                "--system-site-packages",
                job.env_path
            };

        manager.Execute( args,
                [this]( int aRetVal, const QString& aOutput, const QString& aError )
                {
                    qDebug() << "Manager: created venv (python returned" << aRetVal << ")";

                    if( !aError.isEmpty() )
                        qDebug() << "Manager: venv err:" << aError;

                    processNextJob();
                }, &env );

        JOB nextJob( job );
        nextJob.type = JOB_TYPE::SETUP_ENV;
        m_jobs.emplace_back( nextJob );
    }
    else if( job.type == JOB_TYPE::SETUP_ENV )
    {
        qDebug() << "Manager: setting up environment for" << job.plugin_path;

        std::optional<QString> pythonHome = PYTHON_MANAGER::GetPythonEnvironment( job.identifier );
        std::optional<QString> python = PYTHON_MANAGER::GetVirtualPython( job.identifier );

        if( !python )
        {
            qDebug() << "Manager: error: python not found at" << job.env_path;
        }
        else
        {
            PYTHON_MANAGER manager( *python );
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

            if( pythonHome )
                env.insert( "VIRTUAL_ENV", *pythonHome );

#ifdef _WIN32
            QString systemRoot = qEnvironmentVariable( "SYSTEMROOT" );
            env.insert( "SYSTEMROOT", systemRoot );

            QString pythonInterpreterPath = QString::fromStdString( Pgm().GetCommonSettings()->m_Api.python_interpreter );
            QString kicadPython = QString::fromStdString( FindKicadFile( "pythonw.exe" ) );
            if( pythonInterpreterPath == kicadPython || !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
            {
                qDebug() << "Configured Python is the KiCad one; erasing path overrides...";
                env.remove( "PYTHONHOME" );
                env.remove( "PYTHONPATH" );
            }
#endif

            QStringList args = {
                    "-m",
                    "pip",
                    "install",
                    "--upgrade",
                    "pip"
                };

            manager.Execute( args,
                [this]( int aRetVal, const QString& aOutput, const QString& aError )
                {
                    qDebug() << "Manager: upgrade pip returned" << aRetVal;

                    if( !aError.isEmpty() )
                    {
                        qDebug() << "Manager: upgrade pip stderr:" << aError;
                    }

                    processNextJob();
                }, &env );

            JOB nextJob( job );
            nextJob.type = JOB_TYPE::INSTALL_REQUIREMENTS;
            m_jobs.emplace_back( nextJob );
        }
    }
    else if( job.type == JOB_TYPE::INSTALL_REQUIREMENTS )
    {
        qDebug() << "Manager: installing dependencies for" << job.plugin_path;

        std::optional<QString> pythonHome = PYTHON_MANAGER::GetPythonEnvironment( job.identifier );
        std::optional<QString> python = PYTHON_MANAGER::GetVirtualPython( job.identifier );
        QString reqsPath = QDir( job.plugin_path ).filePath( "requirements.txt" );
        QFileInfo reqs( reqsPath );

        if( !python )
        {
            qDebug() << "Manager: error: python not found at" << job.env_path;
        }
        else if( !reqs.isReadable() )
        {
            qDebug() << "Manager: error: requirements.txt not found at" << job.plugin_path;
        }
        else
        {
            qDebug() << "Manager: Python exe" << *python;

            PYTHON_MANAGER manager( *python );
            QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

#ifdef _WIN32
            QString systemRoot = qEnvironmentVariable( "SYSTEMROOT" );
            env.insert( "SYSTEMROOT", systemRoot );

            env.remove( "PYTHONHOME" );
            env.remove( "PYTHONPATH" );
#endif

            if( pythonHome )
                env.insert( "VIRTUAL_ENV", *pythonHome );

            QStringList args = {
                    "-m",
                    "pip",
                    "install",
                    "--no-input",
                    "--isolated",
                    "--only-binary",
                    ":all:",
                    "--require-virtualenv",
                    "--exists-action",
                    "i",
                    "-r",
                    reqs.absoluteFilePath()
                };

            manager.Execute( args,
                [this, job]( int aRetVal, const QString& aOutput, const QString& aError )
                {
                    if( !aError.isEmpty() )
                        qDebug() << "Manager: pip stderr:" << aError;

                    if( aRetVal == 0 )
                    {
                        qDebug() << "Manager: marking" << job.identifier << "as ready";
                        m_readyPlugins.insert( job.identifier );
                        m_busyPlugins.erase( job.identifier );
                    }

                    processNextJob();
                }, &env );
        }

        processNextJob();
    }

    m_jobs.pop_front();
    qDebug() << "Manager: finished job;" << m_jobs.size() << "left in queue";
}