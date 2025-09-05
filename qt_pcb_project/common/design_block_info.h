#ifndef DESIGN_BLOCK_INFO_H_
#define DESIGN_BLOCK_INFO_H_

#include <kicommon.h>
#include <boost/ptr_container/ptr_vector.hpp>
#include <import_export.h>
#include <ki_exception.h>
#include <core/sync_queue.h>
#include <lib_tree_item.h>
#include <atomic>
#include <functional>
#include <memory>
#include <vector>
#include <QString>


class DESIGN_BLOCK_LIB_TABLE;
class DESIGN_BLOCK_LIST;
class DESIGN_BLOCK_LIST_IMPL;
class PROGRESS_REPORTER;
class QWidget;
class KIWAY;
class LOCALE_IO;
class QTextStream;
class KICOMMON_API DESIGN_BLOCK_INFO : public LIB_TREE_ITEM
{
public:
    virtual ~DESIGN_BLOCK_INFO() {}

    const QString& GetDesignBlockName() const { return m_dbname; }

    QString GetLibNickname() const override { return m_nickname; }

    QString GetName() const override { return m_dbname; }

    LIB_ID GetLIB_ID() const override { return LIB_ID( m_nickname, m_dbname ); }

    QString GetDesc() override
    {
        ensure_loaded();
        return m_doc;
    }

    void SetDesc( const QString& aDesc ) { m_doc = aDesc; }

    QString GetKeywords()
    {
        ensure_loaded();
        return m_keywords;
    }

    std::vector<SEARCH_TERM> GetSearchTerms() override;

    int GetOrderNum()
    {
        ensure_loaded();
        return m_num;
    }

    bool InLibrary( const QString& aLibrary ) const;
    friend bool operator<( const DESIGN_BLOCK_INFO& lhs, const DESIGN_BLOCK_INFO& rhs );

protected:
    void ensure_loaded()
    {
        if( !m_loaded )
            load();
    }

    virtual void load( const LOCALE_IO* locale = nullptr ) {};

protected:
    DESIGN_BLOCK_LIST* m_owner;
    bool               m_loaded;
    QString            m_nickname;
    QString            m_dbname;
    int                m_num;
    QString            m_doc;
    QString            m_keywords;
};


class KICOMMON_API DESIGN_BLOCK_LIST
{
public:
    DESIGN_BLOCK_LIST() : m_lib_table( nullptr ) {}

    virtual ~DESIGN_BLOCK_LIST() {}

    unsigned GetCount() const { return m_list.size(); }

    const std::vector<std::unique_ptr<DESIGN_BLOCK_INFO>>& GetList() const { return m_list; }

    void Clear() { m_list.clear(); }

    DESIGN_BLOCK_INFO* GetDesignBlockInfo( const QString& aDesignBlockName );

    DESIGN_BLOCK_INFO* GetDesignBlockInfo( const QString& aLibNickname,
                                           const QString& aDesignBlockName );
    DESIGN_BLOCK_INFO& GetItem( unsigned aIdx ) const { return *m_list[aIdx]; }

    unsigned GetErrorCount() const { return m_errors.size(); }

    std::unique_ptr<IO_ERROR> PopError()
    {
        std::unique_ptr<IO_ERROR> error;

        m_errors.pop( error );
        return error;
    }

    virtual bool ReadDesignBlockFiles( DESIGN_BLOCK_LIB_TABLE* aTable,
                                       const QString*          aNickname = nullptr,
                                       PROGRESS_REPORTER*      aProgressReporter = nullptr ) = 0;

    DESIGN_BLOCK_LIB_TABLE* GetTable() const { return m_lib_table; }

protected:
    DESIGN_BLOCK_LIB_TABLE*                        m_lib_table = nullptr;
    std::vector<std::unique_ptr<DESIGN_BLOCK_INFO>>    m_list;
    SYNC_QUEUE<std::unique_ptr<IO_ERROR>>          m_errors;
};

#endif // DESIGN_BLOCK_INFO_H_
