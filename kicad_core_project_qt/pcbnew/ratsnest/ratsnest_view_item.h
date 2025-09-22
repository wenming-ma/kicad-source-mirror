
/**
 * @brief Class that draws missing connections on a PCB.
 */

#ifndef RATSNEST_VIEW_ITEM_H
#define RATSNEST_VIEW_ITEM_H

#include <memory>
#include <eda_item.h>
#include <math/vector2d.h>
#include <project/net_settings.h>

class GAL;
class CONNECTIVITY_DATA;


class RATSNEST_VIEW_ITEM : public EDA_ITEM
{
public:
    RATSNEST_VIEW_ITEM( std::shared_ptr<CONNECTIVITY_DATA> aData );

    /// @copydoc VIEW_ITEM::ViewBBox()
    const BOX2I ViewBBox() const override;

    /// @copydoc VIEW_ITEM::ViewDraw()
    void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override;

    /// @copydoc VIEW_ITEM::ViewGetLayers()
    std::vector<int> ViewGetLayers() const override;

    bool HitTest( const VECTOR2I& aPoint, int aAccuracy = 0 ) const override
    {
        return false;   // Not selectable
    }

#if defined(DEBUG)
    /// @copydoc EDA_ITEM::Show()
    void Show( int x, std::ostream& st ) const override { }
#endif

    virtual QString GetClass() const override
    {
        return "RATSNEST_VIEW_ITEM";
    }

protected:
    std::shared_ptr<CONNECTIVITY_DATA> m_data;      ///< Object containing ratsnest data.
};


#endif /* RATSNEST_VIEW_ITEM_H */
