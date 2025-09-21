
#include <kiplatform/policy.h>

#include <QString>


KIPLATFORM::POLICY::PBOOL KIPLATFORM::POLICY::GetPolicyBool( const QString& aKey )
{
    return PBOOL::NOT_CONFIGURED;
}


std::uint32_t KIPLATFORM::POLICY::GetPolicyEnumUInt( const QString& aKey )
{
    return 0;
}