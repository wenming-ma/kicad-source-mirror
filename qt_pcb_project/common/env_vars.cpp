
#include <build_version.h>
#include <env_vars.h>
#include <settings/environment.h>

#include <QRegularExpression>
#include <QCoreApplication>
#include <QProcessEnvironment>

using STRING_MAP = QHash<QString, QString>;

static const ENV_VAR::ENV_VAR_LIST predefinedEnvVars = {
    QStringLiteral( "KIPRJMOD" ),
    ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "SYMBOL_DIR" ) ),
    ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "3DMODEL_DIR" ) ),
    ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "FOOTPRINT_DIR" ) ),
    ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "TEMPLATE_DIR" ) ),
    QStringLiteral( "KICAD_USER_TEMPLATE_DIR" ),
    QStringLiteral( "KICAD_PTEMPLATES" ),
    ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "3RD_PARTY" ) ),
};


bool ENV_VAR::IsEnvVarImmutable( const QString& aEnvVar )
{
    for( const QString& s : predefinedEnvVars )
    {
        if( s == aEnvVar )
            return true;
    }

    return false;
}


const ENV_VAR::ENV_VAR_LIST& ENV_VAR::GetPredefinedEnvVars()
{
    return predefinedEnvVars;
}


QString ENV_VAR::GetVersionedEnvVarName( const QString& aBaseName )
{
    int version = 0;
    std::tie(version, std::ignore, std::ignore) = GetMajorMinorPatchTuple();

    return QString( "KICAD%1_%2" ).arg( version ).arg( aBaseName );
}


std::optional<QString> ENV_VAR::GetVersionedEnvVarValue( const ENV_VAR_MAP& aMap,
                                                          const QString& aBaseName )
{
    QString exactMatch = ENV_VAR::GetVersionedEnvVarName( aBaseName );

    if( aMap.count( exactMatch ) )
        return aMap.at( exactMatch ).GetValue();

    QString partialMatch = QString( "KICAD*_%1" ).arg( aBaseName );
    QRegularExpression regex( partialMatch.replace( "*", ".*" ) );

    for( const auto& [k, v] : aMap )
    {
        if( regex.match( k ).hasMatch() )
            return v.GetValue();
    }

    return std::nullopt;
}


static void initialiseEnvVarHelp( STRING_MAP& aMap )
{
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "FOOTPRINT_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "The base path of locally installed system "
            "footprint libraries (.pretty folders)." );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "3DMODEL_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "The base path of system footprint 3D shapes (.3Dshapes folders)." );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "SYMBOL_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "The base path of the locally installed symbol libraries." );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "TEMPLATE_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "A directory containing project templates installed with KiCad." );
    aMap[QStringLiteral( "KICAD_USER_TEMPLATE_DIR" )] =
        QCoreApplication::translate( "ENV_VAR", "Optional. Can be defined if you want to create your own project "
           "templates folder." );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "3RD_PARTY" ) )] =
        QCoreApplication::translate( "ENV_VAR", "A directory containing 3rd party plugins, libraries and other "
           "downloadable content." );
    aMap[QStringLiteral( "KIPRJMOD" )] =
        QCoreApplication::translate( "ENV_VAR", "Internally defined by KiCad (cannot be edited) and is set "
          "to the absolute path of the currently loaded project file.  This environment "
          "variable can be used to define files and paths relative to the currently loaded "
          "project.  For instance, ${KIPRJMOD}/libs/footprints.pretty can be defined as a "
          "folder containing a project specific footprint library named footprints.pretty." );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "SCRIPTING_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "A directory containing system-wide scripts installed with KiCad" );
    aMap[ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "USER_SCRIPTING_DIR" ) )] =
        QCoreApplication::translate( "ENV_VAR", "A directory containing user-specific scripts installed with KiCad" );

#define DEP( var ) QString( QCoreApplication::translate( "ENV_VAR", "Deprecated version of %1." ) ).arg( var )

    aMap[QStringLiteral( "KICAD_PTEMPLATES" )] =
            DEP( ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "TEMPLATE_DIR" ) ) );
    aMap[QStringLiteral( "KISYS3DMOD" )] = DEP( ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "3DMODEL_DIR" ) ) );
    aMap[QStringLiteral( "KISYSMOD" )] = DEP( ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "FOOTPRINT_DIR" ) ) );
    aMap[QStringLiteral( "KICAD_SYMBOL_DIR" )] = DEP( ENV_VAR::GetVersionedEnvVarName( QStringLiteral( "SYMBOL_DIR" ) ) );

#undef DEP
}


QString ENV_VAR::LookUpEnvVarHelp( const QString& aEnvVar )
{
    static STRING_MAP envVarHelpText;

    if( envVarHelpText.size() == 0 )
        initialiseEnvVarHelp( envVarHelpText );

    return envVarHelpText[ aEnvVar ];
}


template<>
std::optional<double> ENV_VAR::GetEnvVar( const QString& aEnvVarName )
{
    QProcessEnvironment systemEnv = QProcessEnvironment::systemEnvironment();
    
    if( systemEnv.contains( aEnvVarName ) )
    {
        QString env = systemEnv.value( aEnvVarName );
        bool ok;
        double value = env.toDouble( &ok );

        if( ok )
            return value;
    }

    return std::nullopt;
}


template<>
std::optional<QString> ENV_VAR::GetEnvVar( const QString& aEnvVarName )
{
    std::optional<QString> optValue;
    QProcessEnvironment systemEnv = QProcessEnvironment::systemEnvironment();

    if( systemEnv.contains( aEnvVarName ) )
    {
        optValue = systemEnv.value( aEnvVarName );
    }

    return optValue;
}
