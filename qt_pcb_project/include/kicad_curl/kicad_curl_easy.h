#ifndef KICAD_CURL_EASY_H_
#define KICAD_CURL_EASY_H_

// curl.h, and therefore kicad_curl.h must be included before Qt headers because on Windows,
// Qt may end up including windows.h before winsocks2.h
// curl and curl.h causes build warnings if included before any Qt headers
//
// So kicad_curl_easy.h does not include curl.h to avoid constraints,
// and including kicad_curl.h could be needed in a few sources

#include <kicommon.h>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <cstdint>
#include <shared_mutex>
#include <vector>

typedef void CURL;
struct curl_slist;


typedef std::function<int( size_t, size_t, size_t, size_t )> TRANSFER_CALLBACK;
struct CURL_PROGRESS;


class KICOMMON_API KICAD_CURL_EASY
{
public:
    KICAD_CURL_EASY();
    ~KICAD_CURL_EASY();

    int Perform();

    void SetHeader( const std::string& aName, const std::string& aValue );

    bool SetUserAgent( const std::string& aAgent );

    bool SetPostFields( const std::vector<std::pair<std::string, std::string>>& aFields );

    bool SetPostFields( const std::string& aField );

    bool SetURL( const std::string& aURL );

    bool SetFollowRedirects( bool aFollow );

    const std::string GetErrorText( int aCode );

    int GetTransferTotal( uint64_t& aDownloadedBytes ) const;

    const std::string& GetBuffer() { return m_buffer; }

    std::string Escape( const std::string& aUrl );

    bool SetTransferCallback( const TRANSFER_CALLBACK& aCallback, size_t aInterval );

    bool SetOutputStream( const std::ostream* aOutput );

    CURL* GetCurl() { return m_CURL; }

    int GetResponseStatusCode();

private:
    template <typename T>
    int setOption( int aOption, T aArg );

    CURL*                               m_CURL;
    curl_slist*                         m_headers;
    std::string                         m_buffer;
    std::unique_ptr<CURL_PROGRESS>      progress;
    std::shared_lock<std::shared_mutex> m_curlSharedLock;
};


#endif // KICAD_CURL_EASY_H_