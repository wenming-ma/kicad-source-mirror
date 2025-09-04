#ifndef KICAD_CURL_H_
#define KICAD_CURL_H_

/*
 * KICAD_CURL.h must be included before Qt because on Windows,
 * Qt ends up including windows.h before winsocks2.h inside curl
 * this causes build warnings
 * Because we are before Qt, we must explicitly define we are building with unicode.
 * Qt defaults to supporting unicode now, so this should be safe.
 */
#if defined(_WIN32)
    #ifndef UNICODE
    #    define UNICODE
    #endif

    #ifndef _UNICODE
    #    define _UNICODE
    #endif
#endif

#include <kicommon.h>
#include <curl/curl.h>
#include <string>
#include <shared_mutex>

// CURL_EXTERN expands to dllimport on MinGW which causes gcc warnings.  This really should
// expand to nothing on MinGW.
#if defined( __MINGW32__)
#  if defined( CURL_EXTERN )
#    undef CURL_EXTERN
#    define CURL_EXTERN
#  endif
#endif

// Simple wrapper class to call curl_global_init and curl_global_cleanup for KiCad.
class KICOMMON_API KICAD_CURL
{
public:
    // Call curl_global_init for the application. It must be used only once
    // and before any curl functions that perform requests.
    static void Init();

    // Call curl_global_cleanup for the application. It must be used only after
    // curl_global_init was called.
    static void Cleanup();

    // Returns the mutex for shared locking when performing curl operations.
    // Unique locking is performed when shutting down.
    static std::shared_mutex& Mutex();

    // Returns true if all curl operations should terminate.
    static bool IsShuttingDown();

    // Wrapper for curl_version(). Reports back a short string of loaded libraries.
    static const char* GetVersion()
    {
        return curl_version();
    }
};

#endif // KICAD_CURL_H_
