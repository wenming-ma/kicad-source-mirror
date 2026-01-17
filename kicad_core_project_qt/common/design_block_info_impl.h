#ifndef DESIGN_BLOCK_INFO_IMPL_H
#define DESIGN_BLOCK_INFO_IMPL_H

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include <vector>
#include <QString>

#include <kicommon.h>
#include <design_block_info.h>
#include <core/sync_queue.h>

class LOCALE_IO;

class KICOMMON_API DESIGN_BLOCK_INFO_IMPL : public DESIGN_BLOCK_INFO
{
public:
    DESIGN_BLOCK_INFO_IMPL( DESIGN_BLOCK_LIST* aOwner, const QString& aNickname,
                            const QString& aDesignBlockName, const LOCALE_IO* aLocale )
    {
        m_nickname = aNickname;
        m_dbname = aDesignBlockName;
        m_num = 0;

        m_owner = aOwner;
        m_loaded = false;
        load( aLocale );
    }

    DESIGN_BLOCK_INFO_IMPL( const QString& aNickname, const QString& aDesignBlockName,
                            const QString& aDescription, const QString& aKeywords, int aOrderNum )
    {
        m_nickname = aNickname;
        m_dbname = aDesignBlockName;
        m_num = aOrderNum;
        m_doc = aDescription;
        m_keywords = aKeywords;

        m_owner = nullptr;
        m_loaded = true;
    }


    DESIGN_BLOCK_INFO_IMPL( const QString& aNickname, const QString& aDesignBlockName )
    {
        m_nickname = aNickname;
        m_dbname = aDesignBlockName;

        m_owner = nullptr;
        m_loaded = true;
    }

protected:
    virtual void load( const LOCALE_IO* aLocale = nullptr ) override;
};


class KICOMMON_API DESIGN_BLOCK_LIST_IMPL : public DESIGN_BLOCK_LIST
{
public:
    DESIGN_BLOCK_LIST_IMPL();
    virtual ~DESIGN_BLOCK_LIST_IMPL(){};

    bool ReadDesignBlockFiles( DESIGN_BLOCK_LIB_TABLE* aTable, const QString* aNickname = nullptr,
                               PROGRESS_REPORTER* aProgressReporter = nullptr ) override;

protected:
    void loadDesignBlocks();

private:
    bool CatchErrors( const std::function<void()>& aFunc );

private:
    SYNC_QUEUE<QString> m_queue;
    long long            m_list_timestamp;
    PROGRESS_REPORTER*   m_progress_reporter;
    std::atomic_bool     m_cancelled;
    std::mutex           m_join;
};

#endif // DESIGN_BLOCK_INFO_IMPL_H
