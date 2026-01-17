// kicad_curl.h must be included before Qt headers, to avoid
// conflicts for some defines, at least on Windows
#include <kicad_curl/kicad_curl.h>

#include <mutex>
#include <atomic>
#include <ki_exception.h>   // THROW_IO_ERROR


static std::shared_mutex s_curlMutex;
static std::atomic<bool> s_curlShuttingDown = false;


void KICAD_CURL::Init()
{
    s_curlShuttingDown = false;

    if( curl_global_init( CURL_GLOBAL_ALL ) != CURLE_OK )
        THROW_IO_ERROR( "curl_global_init() failed." );
}


void KICAD_CURL::Cleanup()
{
    s_curlShuttingDown = true;

    std::unique_lock lock( s_curlMutex );

    curl_global_cleanup();
}


std::shared_mutex& KICAD_CURL::Mutex()
{
    return s_curlMutex;
}


bool KICAD_CURL::IsShuttingDown()
{
    return s_curlShuttingDown;
}


std::string GetKicadCurlVersion()
{
    return KICAD_CURL::GetVersion();
}


std::string GetCurlLibVersion()
{
    return LIBCURL_VERSION;
}
