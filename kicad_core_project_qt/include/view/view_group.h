
#pragma once

#include <gal/gal.h>
#include <view/view_item.h>
#include <deque>
#include <vector>
#include <QString>

namespace KIGFX
{
class GAL_API VIEW_GROUP : public VIEW_ITEM
{
public:
    VIEW_GROUP( VIEW* aView = nullptr );
    virtual ~VIEW_GROUP();

    // We own at least one list of raw pointers.  Don't let the compiler fill in copy c'tors that
    // will only land us in trouble.
    VIEW_GROUP( const VIEW_GROUP& ) = delete;
    VIEW_GROUP& operator=( const VIEW_GROUP& ) = delete;

    QString GetClass() const override;

    virtual unsigned int GetSize() const;
    virtual void Add( VIEW_ITEM* aItem );
    virtual void Remove( VIEW_ITEM* aItem );
    virtual void Clear();
    virtual VIEW_ITEM* GetItem( unsigned int aIdx ) const;
    virtual const BOX2I ViewBBox() const override;
    virtual void ViewDraw( int aLayer, VIEW* aView ) const override;
    std::vector<int> ViewGetLayers() const override;

    inline virtual void SetLayer( int aLayer )
    {
        m_layer = aLayer;
    }

    void FreeItems();

protected:
    virtual const std::vector<VIEW_ITEM*> updateDrawList() const;

protected:
    int                     m_layer;
    std::vector<VIEW_ITEM*> m_groupItems;       // No ownership.
};

} // namespace KIGFX
