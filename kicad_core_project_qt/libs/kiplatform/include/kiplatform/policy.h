#ifndef KIPLATFORM_POLICY_H_
#define KIPLATFORM_POLICY_H_

#include <cstdint>
#include <QString>

namespace KIPLATFORM
{
    namespace POLICY
    {
        enum class PBOOL
        {
            ENABLED,
            DISABLED,
            NOT_CONFIGURED
        };

        PBOOL         GetPolicyBool( const QString& aKey );
        std::uint32_t GetPolicyEnumUInt( const QString& aKey );

        template <typename T>
        T GetPolicyEnum( const QString& aKey )
        {
            return static_cast<T>( GetPolicyEnumUInt( aKey ) );
        }
    }
}

#endif