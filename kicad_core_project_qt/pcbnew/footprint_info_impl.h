
#ifndef FOOTPRINT_INFO_IMPL_H
#define FOOTPRINT_INFO_IMPL_H

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include <footprint_info.h>
#include <core/sync_queue.h>

class LOCALE_IO;

class FOOTPRINT_INFO_IMPL : public FOOTPRINT_INFO
{
public:
    FOOTPRINT_INFO_IMPL( FOOTPRINT_LIST* aOwner, const QString& aNickname, const QString& aFootprintName,
                         const LOCALE_IO* aLocale )
    {
        m_nickname = aNickname;
        m_fpname = aFootprintName;
        m_num = 0;
        m_pad_count = 0;
        m_unique_pad_count = 0;

        m_owner = aOwner;
        m_loaded = false;
        load( aLocale );
    }

    // A constructor for cached items
    FOOTPRINT_INFO_IMPL( const QString& aNickname, const QString& aFootprintName,
                         const QString& aDescription, const QString& aKeywords,
                         int aOrderNum, unsigned int aPadCount, unsigned int aUniquePadCount )
    {
        m_nickname = aNickname;
        m_fpname = aFootprintName;
        m_num = aOrderNum;
        m_pad_count = aPadCount;
        m_unique_pad_count = aUniquePadCount;
        m_doc = aDescription;
        m_keywords = aKeywords;

        m_owner = nullptr;
        m_loaded = true;
    }


    // A dummy constructor for use as a target in a binary search
    FOOTPRINT_INFO_IMPL( const QString& aNickname, const QString& aFootprintName )
    {
        m_nickname = aNickname;
        m_fpname = aFootprintName;

        m_owner = nullptr;
        m_loaded = true;
    }

protected:
    virtual void load( const LOCALE_IO* aLocale ) override;
};


class FOOTPRINT_LIST_IMPL : public FOOTPRINT_LIST
{
public:
    FOOTPRINT_LIST_IMPL();
    virtual ~FOOTPRINT_LIST_IMPL() {};

    void WriteCacheToFile( const QString& aFilePath ) override;
    void ReadCacheFromFile( const QString& aFilePath ) override;

    bool ReadFootprintFiles( FP_LIB_TABLE* aTable, const QString* aNickname = nullptr,
                             PROGRESS_REPORTER* aProgressReporter = nullptr ) override;

    void Clear() override;

protected:
    void loadFootprints();

private:
    /**
     * Call aFunc, pushing any IO_ERRORs and std::exceptions it throws onto m_errors.
     *
     * @return true if no error occurred.
     */
    bool CatchErrors( const std::function<void()>& aFunc );

    SYNC_QUEUE<QString>     m_queue;
    long long                m_list_timestamp;
    PROGRESS_REPORTER*       m_progress_reporter;
    std::atomic_bool         m_cancelled;
    std::mutex               m_join;
};

extern FOOTPRINT_LIST_IMPL GFootprintList;        // KIFACE scope.


#endif // FOOTPRINT_INFO_IMPL_H
