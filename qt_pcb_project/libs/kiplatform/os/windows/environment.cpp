
#include <kiplatform/environment.h>
#include <QString>
#include <QStringList>
#include <QStandardPaths>
#include <QCoreApplication>
#include <QUrl>
#include <QWidget>
#include <QDir>
#include <QRegularExpression>

#include <Windows.h>
#include <shellapi.h>
#include <shlwapi.h>
#include <propkey.h>
#include <propvarutil.h>
#if defined( __MINGW32__ )
    #include <shobjidl.h>
#else
    #include <shobjidl_core.h>
#endif
#include <winhttp.h>

#if defined( __MINGW32__ )
    #include <shlobj.h>
#else
    #include <shlobj_core.h>
#endif

#include <Softpub.h>
#include <wincrypt.h>
#include <wintrust.h>

#define INCLUDE_KICAD_VERSION // fight me
#include <kicad_build_version.h>


void KIPLATFORM::ENV::Init()
{
    ::SetCurrentProcessExplicitAppUserModelID( reinterpret_cast<LPCWSTR>( GetAppUserModelId().utf16() ) );
}


bool KIPLATFORM::ENV::MoveToTrash( const QString& aPath, QString& aError )
{
    // The filename field must be a double-null terminated string
    QString temp = aPath + '\0';

    SHFILEOPSTRUCT fileOp;
    ::ZeroMemory( &fileOp, sizeof( fileOp ) );

    fileOp.hwnd   = nullptr; // Set to null since there is no progress dialog
    fileOp.wFunc  = FO_DELETE;
    fileOp.pFrom  = reinterpret_cast<LPCWSTR>( temp.utf16() );
    fileOp.pTo    = nullptr; // Set to to NULL since we aren't moving the file
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;

    int eVal = SHFileOperation( &fileOp );

    if( eVal != 0 )
    {
        aError = QString( "Error code: %1" ).arg( eVal );
        return false;
    }

    return true;
}


bool KIPLATFORM::ENV::IsNetworkPath( const QString& aPath )
{
    return ::PathIsNetworkPathW( reinterpret_cast<LPCWSTR>( aPath.utf16() ) );
}


QString KIPLATFORM::ENV::GetDocumentsPath()
{
    // If called by a python script in stand-alone (outside KiCad), QCoreApplication::instance()
    // may be nullptr, so handle gracefully
    return QStandardPaths::writableLocation( QStandardPaths::DocumentsLocation );
}


QString KIPLATFORM::ENV::GetUserConfigPath()
{
    // If called by a python script in stand-alone (outside KiCad), QCoreApplication::instance()
    // may be nullptr, so handle gracefully
    return QStandardPaths::writableLocation( QStandardPaths::AppConfigLocation );
}


QString KIPLATFORM::ENV::GetUserDataPath()
{
    // If called by a python script in stand-alone (outside KiCad), QCoreApplication::instance()
    // may be nullptr, so handle gracefully
    return QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );
}


QString KIPLATFORM::ENV::GetUserLocalDataPath()
{
    // If called by a python script in stand-alone (outside KiCad), QCoreApplication::instance()
    // may be nullptr, so handle gracefully
    return QStandardPaths::writableLocation( QStandardPaths::AppLocalDataLocation );
}


QString KIPLATFORM::ENV::GetUserCachePath()
{
    // Unfortunately AppData/Local is the closest analog to "Cache" directories of other platforms
    // If called by a python script in stand-alone (outside KiCad), QCoreApplication::instance()
    // may be nullptr, so handle gracefully
    return QStandardPaths::writableLocation( QStandardPaths::CacheLocation );
}


bool KIPLATFORM::ENV::GetSystemProxyConfig( const QString& aURL, PROXY_CONFIG& aCfg )
{
    // Original source from Microsoft sample (public domain)
    // https://github.com/microsoft/Windows-classic-samples/blob/main/Samples/WinhttpProxy/cpp/GetProxy.cpp#L844
    bool                                 autoProxyDetect = false;
    WINHTTP_CURRENT_USER_IE_PROXY_CONFIG ieProxyConfig = { 0 };
    WINHTTP_AUTOPROXY_OPTIONS            autoProxyOptions = { 0 };
    WINHTTP_PROXY_INFO                   autoProxyInfo = { 0 };
    HINTERNET                            proxyResolveSession = NULL;
    bool                                 success = false;

    QUrl uri( aURL );

    LPWSTR proxyStr = NULL;
    LPWSTR bypassProxyStr = NULL;

    if( WinHttpGetIEProxyConfigForCurrentUser( &ieProxyConfig ) )
    {
        // welcome to the wonderful world of IE
        // we use the ie config simply to handle it off to the other win32 api
        if( ieProxyConfig.fAutoDetect )
        {
            autoProxyDetect = true;
        }

        if( ieProxyConfig.lpszAutoConfigUrl != NULL )
        {
            autoProxyDetect = true;
            autoProxyOptions.lpszAutoConfigUrl = ieProxyConfig.lpszAutoConfigUrl;
        }
    }
    else if( GetLastError() == ERROR_FILE_NOT_FOUND )
    {
        // this is the only error code where we want to continue attempting to find a proxy
        autoProxyDetect = true;
    }

    if( autoProxyDetect )
    {
        proxyResolveSession =
                WinHttpOpen( NULL, WINHTTP_ACCESS_TYPE_AUTOMATIC_PROXY, WINHTTP_NO_PROXY_NAME,
                             WINHTTP_NO_PROXY_BYPASS, WINHTTP_FLAG_ASYNC );

        if( proxyResolveSession )
        {
            // either we use the ie url or we set the auto detect mode
            if( autoProxyOptions.lpszAutoConfigUrl != NULL )
            {
                autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_CONFIG_URL;
            }
            else
            {
                autoProxyOptions.dwFlags = WINHTTP_AUTOPROXY_AUTO_DETECT;
                autoProxyOptions.dwAutoDetectFlags =
                        WINHTTP_AUTO_DETECT_TYPE_DHCP | WINHTTP_AUTO_DETECT_TYPE_DNS_A;
            }

            // dont do auto logon at first, this allows windows to use an cache
            // per https://docs.microsoft.com/en-us/windows/win32/winhttp/autoproxy-cache
            autoProxyOptions.fAutoLogonIfChallenged = FALSE;

            autoProxyDetect = WinHttpGetProxyForUrl( proxyResolveSession, reinterpret_cast<LPCWSTR>( aURL.utf16() ),
                                                     &autoProxyOptions, &autoProxyInfo );

            if( !autoProxyDetect && GetLastError() == ERROR_WINHTTP_LOGIN_FAILURE )
            {
                autoProxyOptions.fAutoLogonIfChallenged = TRUE;

                // try again with auto login now
                autoProxyDetect = WinHttpGetProxyForUrl( proxyResolveSession, reinterpret_cast<LPCWSTR>( aURL.utf16() ),
                                                         &autoProxyOptions, &autoProxyInfo );
            }

            if( autoProxyDetect )
            {
                if( autoProxyInfo.dwAccessType == WINHTTP_ACCESS_TYPE_NAMED_PROXY )
                {
                    proxyStr = autoProxyInfo.lpszProxy;
                    bypassProxyStr = autoProxyInfo.lpszProxyBypass;
                }
            }

            WinHttpCloseHandle( proxyResolveSession );
        }
    }

    if( !autoProxyDetect && ieProxyConfig.lpszProxy != NULL )
    {
        proxyStr = ieProxyConfig.lpszProxy;
        bypassProxyStr = ieProxyConfig.lpszProxyBypass;
    }

    bool bypassed = false;
    if( bypassProxyStr != NULL )
    {
        QStringList tokens = QString::fromWCharArray( bypassProxyStr ).split( QLatin1Char( ';' ) );

        for( const QString& host : tokens )
        {
            if( host == uri.host() )
            {
                // the given url has a host in the proxy bypass list
                return false;
            }

            // <local> is a special case that says all local sites bypass
            // the windows way for considering local is any host without periods in the name that would imply
            // some non-internal dns resolution
            if( host == "<local>" )
            {
                if( !uri.host().contains( "." ) )
                {
                    // great its a local uri that is bypassed
                    bypassed = true;
                    break;
                }
            }
        }
    }

    if( !bypassed && proxyStr != NULL )
    {
        // proxyStr can be in the following format per MSDN
        //([<scheme>=][<scheme>"://"]<server>[":"<port>])
        //and separated by semicolons or whitespace
        QStringList tokens = QString::fromWCharArray( proxyStr ).split( QRegularExpression( "[; \t]+" ), Qt::SkipEmptyParts );

        for( const QString& entry : tokens )
        {
            QString processedEntry = entry;

            // deal with the [<scheme>=] part, which may or may not exist
            if( processedEntry.contains( "=" ) )
            {
                QString scheme = processedEntry.section( '=', 0, 0 ).toLower();
                processedEntry = processedEntry.section( '=', 1 );

                // skip processing if the scheme doesnt match
                if( scheme != uri.scheme().toLower() )
                {
                    continue;
                }

                // we continue with the [<scheme>=] stripped off if we matched
            }

            // is the entry left not empty? we just take the first result
            // : and :: are also special cases we want to ignore
            if( !processedEntry.isEmpty() && processedEntry != ":" && processedEntry != "::" )
            {
                aCfg.host = processedEntry;
                success = true;
                break;
            }
        }
    }


    // We have to clean up the strings the win32 api returned
    if( autoProxyInfo.lpszProxy )
    {
        GlobalFree( autoProxyInfo.lpszProxy );
        autoProxyInfo.lpszProxy = NULL;
    }

    if( autoProxyInfo.lpszProxyBypass )
    {
        GlobalFree( autoProxyInfo.lpszProxyBypass );
        autoProxyInfo.lpszProxyBypass = NULL;
    }

    if( ieProxyConfig.lpszAutoConfigUrl != NULL )
    {
        GlobalFree( ieProxyConfig.lpszAutoConfigUrl );
        ieProxyConfig.lpszAutoConfigUrl = NULL;
    }

    if( ieProxyConfig.lpszProxy != NULL )
    {
        GlobalFree( ieProxyConfig.lpszProxy );
        ieProxyConfig.lpszProxy = NULL;
    }

    if( ieProxyConfig.lpszProxyBypass != NULL )
    {
        GlobalFree( ieProxyConfig.lpszProxyBypass );
        ieProxyConfig.lpszProxyBypass = NULL;
    }

    return success;
}


bool KIPLATFORM::ENV::VerifyFileSignature( const QString& aPath )
{
    WINTRUST_FILE_INFO fileData;
    memset( &fileData, 0, sizeof( fileData ) );
    fileData.cbStruct = sizeof( WINTRUST_FILE_INFO );
    fileData.pcwszFilePath = reinterpret_cast<LPCWSTR>( aPath.utf16() );

    // verifies entire certificate chain
    GUID policy = WINTRUST_ACTION_GENERIC_VERIFY_V2;

    WINTRUST_DATA trustData;
    memset( &trustData, 0, sizeof( trustData ) );

    trustData.cbStruct = sizeof( trustData );
    trustData.dwUIChoice = WTD_UI_NONE;
    // revocation checking incurs latency penalities due to need for online queries
    trustData.fdwRevocationChecks = WTD_REVOKE_NONE;
    trustData.dwUnionChoice = WTD_CHOICE_FILE;
    trustData.dwStateAction = WTD_STATEACTION_VERIFY;
    trustData.pFile = &fileData;


    bool verified = false;
    LONG status = WinVerifyTrust( NULL, &policy, &trustData );

    verified = ( status == ERROR_SUCCESS );

    // Cleanup/release (yes its weird looking)
    trustData.dwStateAction = WTD_STATEACTION_CLOSE;
    WinVerifyTrust( NULL, &policy, &trustData );

    return verified;
}


QString KIPLATFORM::ENV::GetAppUserModelId()
{
    // The application model id allows for taskbar grouping
    // However, be warned, this cannot be too unique like per-process
    // Because longer scope Windows features, such as "Pin to Taskbar"
    // on a running application, depend on this being consistent.
    QStringList modelIdComponents;
    modelIdComponents.append( "Kicad" );
    modelIdComponents.append( "Kicad" );
    
    if( QCoreApplication::instance() )
        modelIdComponents.append( QCoreApplication::instance()->applicationName() );
    else
        modelIdComponents.append( "KiCad" );
    
    modelIdComponents.append( KICAD_MAJOR_MINOR_VERSION );

    QString modelId = modelIdComponents.join( "." );
    modelId.replace( " ", "_" ); // remove spaces sanity

    // the other limitation is 127 characters but we arent trying to hit that limit yet

    return modelId;
}


void KIPLATFORM::ENV::SetAppDetailsForWindow( QWidget* aWindow, const QString& aRelaunchCommand,
                                              const QString& aRelaunchDisplayName )
{
    IPropertyStore* pps;
    HRESULT         hr = ::SHGetPropertyStoreForWindow( reinterpret_cast<HWND>( aWindow->winId() ), IID_PPV_ARGS( &pps ) );
    if( SUCCEEDED( hr ) )
    {
        PROPVARIANT pv;

        // This is required for any the other properties to actually work
        hr = ::InitPropVariantFromString( reinterpret_cast<LPCWSTR>( GetAppUserModelId().utf16() ), &pv );

        if( SUCCEEDED( hr ) )
        {
            hr = pps->SetValue( PKEY_AppUserModel_ID, pv );
            PropVariantClear( &pv );
        }


        if( !aRelaunchCommand.isEmpty() )
        {
            hr = ::InitPropVariantFromString( reinterpret_cast<LPCWSTR>( aRelaunchCommand.utf16() ), &pv );
        }
        else
        {
            // empty var
            ::PropVariantInit( &pv );
        }

        if( SUCCEEDED( hr ) )
        {
            hr = pps->SetValue( PKEY_AppUserModel_RelaunchCommand, pv );
            PropVariantClear( &pv );
        }

        if( !aRelaunchDisplayName.isEmpty() )
        {
            hr = ::InitPropVariantFromString( reinterpret_cast<LPCWSTR>( aRelaunchDisplayName.utf16() ), &pv );
        }
        else
        {
            // empty var
            ::PropVariantInit( &pv );
        }

        if( SUCCEEDED( hr ) )
        {
            hr = pps->SetValue( PKEY_AppUserModel_RelaunchDisplayNameResource, pv );
            PropVariantClear( &pv );
        }

        pps->Release();
    }
}


QString KIPLATFORM::ENV::GetCommandLineStr()
{
    return QString::fromWCharArray( ::GetCommandLine() );
}


void KIPLATFORM::ENV::AddToRecentDocs( const QString& aPath )
{
    IShellItem* psi = nullptr;
    HRESULT     hr = SHCreateItemFromParsingName( reinterpret_cast<LPCWSTR>( aPath.utf16() ), NULL, IID_PPV_ARGS( &psi ) );

    if( SUCCEEDED( hr ) )
    {
        QString        appID = GetAppUserModelId();
        SHARDAPPIDINFO info;
        info.psi = psi;
        info.pszAppID = reinterpret_cast<LPCWSTR>( appID.utf16() );
        ::SHAddToRecentDocs( SHARD_APPIDINFO, &info );

        psi->Release();
    }

    ::SHAddToRecentDocs( SHARD_PATHW, reinterpret_cast<LPCWSTR>( aPath.utf16() ) );
}