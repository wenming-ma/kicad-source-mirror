
#ifndef __PNS_INDEX_H
#define __PNS_INDEX_H

#include <deque>
#include <list>
#include <map>
#include <unordered_set>

#include <layer_ids.h>
#include <geometry/shape_index.h>

#include "pns_item.h"
#include "pns_node.h"

namespace PNS {


// Custom spatial index for board items with fast spatial searches
class INDEX
{
public:
    typedef std::list<ITEM*>            NET_ITEMS_LIST;
    typedef SHAPE_INDEX<ITEM*>          ITEM_SHAPE_INDEX;
    typedef std::unordered_set<ITEM*>   ITEM_SET;

    INDEX(){};

    void Add( ITEM* aItem );

    void Remove( ITEM* aItem );

    void Replace( ITEM* aOldItem, ITEM* aNewItem );

    template<class Visitor>
    int Query( const ITEM* aItem, int aMinDistance, Visitor& aVisitor ) const;

    template<class Visitor>
    int Query( const SHAPE* aShape, int aMinDistance, Visitor& aVisitor ) const;

    NET_ITEMS_LIST* GetItemsForNet( NET_HANDLE aNet );

    bool Contains( ITEM* aItem ) const
    {
        return m_allItems.find( aItem ) != m_allItems.end();
    }

    int Size() const { return m_allItems.size(); }

    ITEM_SET::iterator begin() { return m_allItems.begin(); }
    ITEM_SET::iterator end() { return m_allItems.end(); }

private:
    template <class Visitor>
    int querySingle( std::size_t aIndex, const SHAPE* aShape, int aMinDistance, Visitor& aVisitor ) const;

private:
    std::deque<std::unique_ptr<ITEM_SHAPE_INDEX>> m_subIndices;
    std::map<NET_HANDLE, NET_ITEMS_LIST> m_netMap;
    ITEM_SET                             m_allItems;
};


template<class Visitor>
int INDEX::querySingle( std::size_t aIndex, const SHAPE* aShape, int aMinDistance, Visitor& aVisitor ) const
{
    if( aIndex >= m_subIndices.size() )
        return 0;

    LAYER_CONTEXT_SETTER layerContext( aVisitor, aIndex );
    return m_subIndices[aIndex]->Query( aShape, aMinDistance, aVisitor);
}

template<class Visitor>
int INDEX::Query( const ITEM* aItem, int aMinDistance, Visitor& aVisitor ) const
{
    int total = 0;

    Q_ASSERT( aItem->Kind() != ITEM::INVALID_T );
    if( aItem->Kind() == ITEM::INVALID_T ) return 0;

    const PNS_LAYER_RANGE& layers = aItem->Layers();

    for( int i = layers.Start(); i <= layers.End(); ++i )
        total += querySingle( i, aItem->Shape( i ), aMinDistance, aVisitor );

    return total;
}

template<class Visitor>
int INDEX::Query( const SHAPE* aShape, int aMinDistance, Visitor& aVisitor ) const
{
    int total = 0;

    for( std::size_t i = 0; i < m_subIndices.size(); ++i )
        total += querySingle( i, aShape, aMinDistance, aVisitor );

    return total;
}

};

#endif
