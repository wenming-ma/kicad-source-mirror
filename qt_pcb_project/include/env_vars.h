

#ifndef ENV_VARS_H
#define ENV_VARS_H

#include <kicommon.h>
#include <QString>
#include <map>
#include <vector>
#include <optional>

class ENV_VAR_ITEM;

namespace ENV_VAR
{
    using ENV_VAR_LIST = std::vector<QString>;

    KICOMMON_API bool IsEnvVarImmutable( const QString& aEnvVar );

    KICOMMON_API const ENV_VAR_LIST& GetPredefinedEnvVars();

    KICOMMON_API QString GetVersionedEnvVarName( const QString& aBaseName );

    KICOMMON_API std::optional<QString>
                 GetVersionedEnvVarValue( const std::map<QString, ENV_VAR_ITEM>& aMap,
                                          const QString&                         aBaseName );

    KICOMMON_API QString LookUpEnvVarHelp( const QString& aEnvVar );

    template <typename VAL_TYPE>
    KICOMMON_API std::optional<VAL_TYPE> GetEnvVar( const QString& aEnvVarName );

    template<>
    KICOMMON_API std::optional<QString> GetEnvVar( const QString& aEnvVarName );

    template <>
    KICOMMON_API std::optional<double> GetEnvVar( const QString& aEnvVarName );
};

#endif /* ENV_VARS_H */
