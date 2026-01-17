#ifndef KIPLATFORM_SECRETS_H_
#define KIPLATFORM_SECRETS_H_

#include <QString>

namespace KIPLATFORM
{
    namespace SECRETS
    {

        bool StoreSecret( const QString& aService, const QString& aKey, const QString& aSecret );

        bool GetSecret( const QString& aService, const QString& aKey, QString& aSecret );

    }
}

#endif // KIPLATFORM_SECRETS_H_