
#include <kiplatform/secrets.h>

#include <windows.h>
#include <wincred.h>
#include <QString>

namespace KIPLATFORM
{
    namespace SECRETS
    {
        bool StoreSecret( const QString& aService, const QString& aKey, const QString& aSecret )
        {
            QString display = aService + ":" + aKey;

            CREDENTIALW cred = { 0 };
            cred.Type = CRED_TYPE_GENERIC;
            cred.TargetName = (LPWSTR) display.utf16();
            cred.CredentialBlobSize = (DWORD) aSecret.size();
            cred.CredentialBlob = (LPBYTE) aSecret.toUtf8().data();
            cred.Persist = CRED_PERSIST_ENTERPRISE;

            return CredWriteW( &cred, 0 );
        }

        bool GetSecret( const QString& aService, const QString& aKey, QString& aSecret )
        {
            QString display = aService + ":" + aKey;

            CREDENTIALW* cred = nullptr;
            bool result = CredReadW( (LPWSTR) display.utf16(), CRED_TYPE_GENERIC, 0, &cred );

            if( result )
            {
                aSecret = QString::fromUtf8( (const char*) cred->CredentialBlob,
                                              cred->CredentialBlobSize );
                CredFree( cred );
            }

            return result;
        }
    }
}