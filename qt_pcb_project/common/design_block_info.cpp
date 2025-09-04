
#include <design_block_info.h>
#include <fp_lib_table.h>
#include <dialogs/html_message_box.h>
#include <string_utils.h>
#include <kiface_ids.h>
#include <kiway.h>
#include <lib_id.h>
#include <thread>
#include <utility>
#include <QString>
#include <QStringList>
#include <QVector>
#include <kiface_base.h>
#include <locale_io.h>

DESIGN_BLOCK_INFO* DESIGN_BLOCK_LIST::GetDesignBlockInfo( const QString& aLibNickname,
                                                          const QString& aDesignBlockName )
{
    if( aDesignBlockName.isEmpty() )
        return nullptr;

    for( std::unique_ptr<DESIGN_BLOCK_INFO>& db : m_list )
    {
        if( aLibNickname == db->GetLibNickname() && aDesignBlockName == db->GetDesignBlockName() )
            return db.get();
    }

    return nullptr;
}


DESIGN_BLOCK_INFO* DESIGN_BLOCK_LIST::GetDesignBlockInfo( const QString& aDesignBlockName )
{
    if( aDesignBlockName.isEmpty() )
        return nullptr;

    LIB_ID dbid;

    if( dbid.Parse( aDesignBlockName ) >= 0 )
        return nullptr;

    return GetDesignBlockInfo( dbid.GetLibNickname(), dbid.GetLibItemName() );
}


QVector<SEARCH_TERM> DESIGN_BLOCK_INFO::GetSearchTerms()
{
    QVector<SEARCH_TERM> terms;

    terms.emplace_back( SEARCH_TERM( GetLibNickname(), 4 ) );
    terms.emplace_back( SEARCH_TERM( GetName(), 8 ) );
    terms.emplace_back( SEARCH_TERM( GetLIB_ID().Format(), 16 ) );

    QStringList keywordTokens = GetKeywords().split( QLatin1Char( ' ' ), Qt::SkipEmptyParts );

    for( const QString& token : keywordTokens )
        terms.emplace_back( SEARCH_TERM( token, 4 ) );

    // Also include keywords as one long string, just in case
    terms.emplace_back( SEARCH_TERM( GetKeywords(), 1 ) );
    terms.emplace_back( SEARCH_TERM( GetDesc(), 1 ) );

    return terms;
}


bool DESIGN_BLOCK_INFO::InLibrary( const QString& aLibrary ) const
{
    return aLibrary == m_nickname;
}


bool operator<( const DESIGN_BLOCK_INFO& lhs, const DESIGN_BLOCK_INFO& rhs )
{
    int retv = StrNumCmp( lhs.m_nickname, rhs.m_nickname, false );

    if( retv != 0 )
        return retv < 0;

    // Technically design block names are not case sensitive because the file name is used
    // as the design block name.  On windows this would be problematic because windows does
    // not support case sensitive file names by default.  This should not cause any issues
    // and allow for a future change to use the name defined in the design block file.
    return StrNumCmp( lhs.m_dbname, rhs.m_dbname, false ) < 0;
}
