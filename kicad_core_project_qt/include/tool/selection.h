
#ifndef SELECTION_H
#define SELECTION_H

#include <optional>
#include <QString>
#include <core/typeinfo.h>
#include <deque>
#include <view/view_group.h>

class EDA_ITEM;


class SELECTION : public KIGFX::VIEW_GROUP
{
public:
    SELECTION() :
            KIGFX::VIEW_GROUP::VIEW_GROUP()
    {
        m_isHover = false;
        m_lastAddedItem = nullptr;
        m_orderCounter = 0;
    }

    SELECTION( const SELECTION& aOther ) :
            KIGFX::VIEW_GROUP::VIEW_GROUP()
    {
        m_items = aOther.m_items;
        m_itemsOrders = aOther.m_itemsOrders;
        m_isHover = aOther.m_isHover;
        m_lastAddedItem = aOther.m_lastAddedItem;
        m_orderCounter = aOther.m_orderCounter;
    }

    SELECTION& operator= ( const SELECTION& aOther )
    {
        m_items = aOther.m_items;
        m_itemsOrders = aOther.m_itemsOrders;
        m_isHover = aOther.m_isHover;
        m_lastAddedItem = aOther.m_lastAddedItem;
        m_orderCounter = aOther.m_orderCounter;
        return *this;
    }

    QString GetClass() const override
    {
        return "SELECTION";
    }

    bool operator==( const SELECTION& aOther ) const;

    using ITER = std::deque<EDA_ITEM*>::iterator;
    using CITER = std::deque<EDA_ITEM*>::const_iterator;

    ITER begin() { return m_items.begin(); }
    ITER end() { return m_items.end(); }
    CITER begin() const { return m_items.cbegin(); }
    CITER end() const { return m_items.cend(); }

    void SetIsHover( bool aIsHover )
    {
        m_isHover = aIsHover;
    }

    bool IsHover() const
    {
        return m_isHover;
    }

    virtual void Add( EDA_ITEM* aItem );

    virtual void Remove( EDA_ITEM *aItem );

    virtual void Clear() override
    {
        m_items.clear();
        m_itemsOrders.clear();
        m_orderCounter = 0;
    }

    virtual unsigned int GetSize() const override
    {
        return m_items.size();
    }

    virtual KIGFX::VIEW_ITEM* GetItem( unsigned int aIdx ) const override;

    bool Contains( EDA_ITEM* aItem ) const;

    /// Checks if there is anything selected
    bool Empty() const
    {
        return m_items.empty();
    }

    /// Returns the number of selected parts
    int Size() const
    {
        return m_items.size();
    }

    const std::deque<EDA_ITEM*> GetItems() const
    {
        return m_items;
    }

    EDA_ITEM* GetLastAddedItem() const
    {
        return m_lastAddedItem;
    }

    std::vector<EDA_ITEM*> GetItemsSortedByTypeAndXY( bool leftBeforeRight = true,
                                                      bool topBeforeBottom = true ) const;

    std::vector<EDA_ITEM*> GetItemsSortedBySelectionOrder() const;

    /// Returns the center point of the selection area bounding box.
    virtual VECTOR2I GetCenter() const;

    virtual const BOX2I ViewBBox() const override
    {
        BOX2I r;
        r.SetMaximum();
        return r;
    }

    /// Returns the top left point of the selection area bounding box.
    VECTOR2I GetPosition() const
    {
        return GetBoundingBox().GetPosition();
    }

    virtual BOX2I GetBoundingBox() const;

    virtual EDA_ITEM* GetTopLeftItem( bool onlyModules = false ) const
    {
        return nullptr;
    }

    EDA_ITEM* operator[]( const size_t aIdx ) const
    {
        if( aIdx < m_items.size() )
            return m_items[ aIdx ];

        return nullptr;
    }

    EDA_ITEM* Front() const
    {
        return m_items.size() ? m_items.front() : nullptr;
    }

    std::deque<EDA_ITEM*>& Items()
    {
        return m_items;
    }

    const std::deque<EDA_ITEM*>& Items() const
    {
        return m_items;
    }

    template<class T>
    T* FirstOfKind() const
    {
        for( auto item : m_items )
        {
            if( IsA<T, EDA_ITEM>( item ) )
                return static_cast<T*> ( item );
        }

        return nullptr;
    }

    bool HasType( KICAD_T aType ) const;

    size_t CountType( KICAD_T aType ) const;

    virtual const std::vector<KIGFX::VIEW_ITEM*> updateDrawList() const override;

    bool HasReferencePoint() const
    {
        return m_referencePoint != std::nullopt;
    }

    VECTOR2I GetReferencePoint() const;
    void SetReferencePoint( const VECTOR2I& aP );
    void ClearReferencePoint();

    bool AreAllItemsIdentical() const;

    bool OnlyContains( std::vector<KICAD_T> aList ) const;

protected:
    std::optional<VECTOR2I>         m_referencePoint;
    std::deque<EDA_ITEM*> m_items;
    std::deque<int>       m_itemsOrders;
    int                   m_orderCounter;
    EDA_ITEM*             m_lastAddedItem;
    bool                  m_isHover;

    // mute hidden overloaded virtual function warnings
    using VIEW_GROUP::Add;
    using VIEW_GROUP::Remove;
};


#endif
