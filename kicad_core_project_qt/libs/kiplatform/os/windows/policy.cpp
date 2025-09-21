
#include <kiplatform/policy.h>

#include <QString>
#include <QStringList>
#include <QSettings>
#include <windows.h>

#include <memory>

#define POLICY_KEY_ROOT "Software\\Policies\\KiCad\\KiCad"


static HKEY GetPolicyRegKey( QString& aKey )
{
    QString  key = aKey;
    HKEY keyToUse = nullptr;

    QString keyPath = POLICY_KEY_ROOT;

    QStringList tokens = aKey.split( "\\" );
    for( int i = 0; i < tokens.size(); i++ )
    {
        QString token = tokens[i];

        if( i < tokens.size() - 1 )
        {
            keyPath.append( "\\" );
            keyPath.append( token );
        }
        else
            key = token;
    }

    HKEY userKey;
    LONG result = RegOpenKeyExA( HKEY_CURRENT_USER, keyPath.toStdString().c_str(), 0, KEY_READ, &userKey );

    // we have user level policies take precedence over computer level policies
    if( result == ERROR_SUCCESS )
    {
        DWORD type;
        DWORD size = 0;
        if( RegQueryValueExA( userKey, key.toStdString().c_str(), nullptr, &type, nullptr, &size ) == ERROR_SUCCESS )
        {
            keyToUse = userKey;
        }
        else
        {
            RegCloseKey( userKey );
        }
    }

    if( keyToUse == nullptr )
    {
        HKEY compKey;
        result = RegOpenKeyExA( HKEY_LOCAL_MACHINE, keyPath.toStdString().c_str(), 0, KEY_READ, &compKey );

        if( result == ERROR_SUCCESS )
        {
            DWORD type;
            DWORD size = 0;
            if( RegQueryValueExA( compKey, key.toStdString().c_str(), nullptr, &type, nullptr, &size ) == ERROR_SUCCESS )
            {
                keyToUse = compKey;
            }
            else
            {
                RegCloseKey( compKey );
            }
        }
    }

    aKey = key;
    return keyToUse;
}


KIPLATFORM::POLICY::PBOOL KIPLATFORM::POLICY::GetPolicyBool( const QString& aKey )
{
    QString  key = aKey;
    HKEY keyToUse = GetPolicyRegKey( key );

    if( keyToUse != nullptr )
    {
        DWORD value;
        DWORD size = sizeof(DWORD);
        DWORD type;
        if( RegQueryValueExA( keyToUse, key.toStdString().c_str(), nullptr, &type, (LPBYTE)&value, &size ) == ERROR_SUCCESS )
        {
            RegCloseKey( keyToUse );
            if( value == 1 )
                return POLICY::PBOOL::ENABLED;
            else
                return POLICY::PBOOL::DISABLED;
        }
        RegCloseKey( keyToUse );
    }

    return PBOOL::NOT_CONFIGURED;
}


std::uint32_t KIPLATFORM::POLICY::GetPolicyEnumUInt( const QString& aKey )
{
    QString  key = aKey;
    HKEY keyToUse = GetPolicyRegKey( key );

    if( keyToUse != nullptr )
    {
        DWORD value;
        DWORD size = sizeof(DWORD);
        DWORD type;
        if( RegQueryValueExA( keyToUse, key.toStdString().c_str(), nullptr, &type, (LPBYTE)&value, &size ) == ERROR_SUCCESS )
        {
            RegCloseKey( keyToUse );
            return value;
        }
        RegCloseKey( keyToUse );
    }

    return 0;
}