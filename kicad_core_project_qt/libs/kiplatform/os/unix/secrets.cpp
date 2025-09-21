
#include <kiplatform/secrets.h>

#include <libsecret/secret.h>

namespace KIPLATFORM
{
    namespace SECRETS
    {
        static const SecretSchema schema =
        {
            "org.kicad.kicad", SECRET_SCHEMA_NONE,
            {
                { "service", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { "key", SECRET_SCHEMA_ATTRIBUTE_STRING },
                { nullptr, SECRET_SCHEMA_ATTRIBUTE_STRING }
            }
        };

        bool StoreSecret( const QString& aService, const QString& aKey, const QString& aSecret )
        {
            GError* error = nullptr;
            QString display = aService + ":" + aKey;

            secret_password_store_sync( &schema,
                                        SECRET_COLLECTION_DEFAULT,
                                        display.toLocal8Bit().constData(),   // Display name
                                        aSecret.toLocal8Bit().constData(),   // Secret value
                                        nullptr,
                                        &error,
                                        "service", aService.toStdString().c_str(),
                                        "key", aKey.toStdString().c_str(),
                                        nullptr );

            if( error )
            {
                g_error_free( error );
                return false;
            }

            return true;
        }

        bool GetSecret( const QString& aService, const QString& aKey, QString& aSecret )
        {
            GError* error = nullptr;
            gchar* secret = secret_password_lookup_sync( &schema,
                                                         nullptr,
                                                         &error,
                                                         "service", aService.toStdString().c_str(),
                                                         "key", aKey.toStdString().c_str(),
                                                         nullptr );

            if( error )
            {
                g_error_free( error );
                return false;
            }

            aSecret = QString::fromLocal8Bit(secret);
            g_free( secret );

            return true;
        }
    }
}