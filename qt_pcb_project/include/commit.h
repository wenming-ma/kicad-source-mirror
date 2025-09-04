// QT_TRANSFORMATION_COMPLETED

#ifndef __COMMIT_H
#define __COMMIT_H

#include <set>
#include <vector>
#include <QString>
#include <undo_redo_container.h>
#include <kiid.h>

class EDA_ITEM;
class BASE_SCREEN;
enum CHANGE_TYPE {
    CHT_ADD     = 1,
    CHT_REMOVE  = 2,
    CHT_MODIFY  = 4,
    CHT_GROUP   = 8,
    CHT_UNGROUP = 16,
    CHT_TYPE    = CHT_ADD | CHT_REMOVE | CHT_MODIFY | CHT_GROUP | CHT_UNGROUP,

    CHT_DONE    = 32,
    CHT_FLAGS   = CHT_DONE
};

template<typename T>
CHANGE_TYPE operator|( CHANGE_TYPE aTypeA, T aTypeB )
{
    return CHANGE_TYPE( (int) aTypeA | (int) aTypeB );
}

template<typename T>
CHANGE_TYPE operator&( CHANGE_TYPE aTypeA, T aTypeB )
{
    return CHANGE_TYPE( (int) aTypeA & (int) aTypeB );
}


class COMMIT
{
public:
    COMMIT();
    virtual ~COMMIT();

    COMMIT& Add( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr )
    {
        return Stage( aItem, CHT_ADD, aScreen );
    }

    COMMIT& Added( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr )
    {
        return Stage( aItem, CHT_ADD | CHT_DONE, aScreen );
    }

    COMMIT& Remove( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr )
    {
        return Stage( aItem, CHT_REMOVE, aScreen );
    }

    COMMIT& Removed( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr )
    {
        return Stage( aItem, CHT_REMOVE | CHT_DONE, aScreen );
    }

    COMMIT& Modify( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr )
    {
        return Stage( aItem, CHT_MODIFY, aScreen );
    }

    COMMIT& Modified( EDA_ITEM* aItem, EDA_ITEM* aCopy, BASE_SCREEN *aScreen = nullptr )
    {
        return createModified( aItem, aCopy, 0, aScreen );
    }

    template<class Range>

    COMMIT& StageItems( const Range& aRange, CHANGE_TYPE aChangeType )
    {
        for( const auto& item : aRange )
            Stage( item, aChangeType );

        return *this;
    }

    virtual COMMIT& Stage( EDA_ITEM* aItem, CHANGE_TYPE aChangeType,
                           BASE_SCREEN *aScreen = nullptr );

    virtual COMMIT& Stage( std::vector<EDA_ITEM*>& container, CHANGE_TYPE aChangeType,
                           BASE_SCREEN *aScreen = nullptr );

    virtual COMMIT& Stage( const PICKED_ITEMS_LIST& aItems,
                           UNDO_REDO aModFlag = UNDO_REDO::UNSPECIFIED,
                           BASE_SCREEN *aScreen = nullptr );

    void Unstage( EDA_ITEM* aItem, BASE_SCREEN* aScreen );

    virtual void Push( const QString& aMessage = QStringLiteral( "A commit" ), int aFlags = 0 ) = 0;
    virtual void Revert() = 0;

    bool Empty() const
    {
        return m_changes.empty();
    }

    int GetStatus( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr );

    EDA_ITEM* GetFirst() const { return m_changes.empty() ? nullptr : m_changes[0].m_item; }

protected:
    struct COMMIT_LINE
    {
        EDA_ITEM*    m_item;
        EDA_ITEM*    m_copy;
        CHANGE_TYPE  m_type;
        KIID         m_parent = NilUuid();
        BASE_SCREEN* m_screen;
    };

    void clear()
    {
        m_changedItems.clear();
        m_deletedItems.clear();
        m_changes.clear();
    }

    COMMIT& createModified( EDA_ITEM* aItem, EDA_ITEM* aCopy, int aExtraFlags = 0,
                            BASE_SCREEN *aScreen = nullptr );

    virtual void makeEntry( EDA_ITEM* aItem, CHANGE_TYPE aType, EDA_ITEM* aCopy = nullptr,
                            BASE_SCREEN *aScreen = nullptr );

    COMMIT_LINE* findEntry( EDA_ITEM* aItem, BASE_SCREEN *aScreen = nullptr );

    virtual EDA_ITEM* parentObject( EDA_ITEM* aItem ) const = 0;

    virtual EDA_ITEM* makeImage( EDA_ITEM* aItem ) const = 0;

    CHANGE_TYPE convert( UNDO_REDO aType ) const;
    UNDO_REDO convert( CHANGE_TYPE aType ) const;

protected:
    std::set<EDA_ITEM*>      m_changedItems;
    std::set<EDA_ITEM*>      m_deletedItems;
    std::vector<COMMIT_LINE> m_changes;
};

#endif
