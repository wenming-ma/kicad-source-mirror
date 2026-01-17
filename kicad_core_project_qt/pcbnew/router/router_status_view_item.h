// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef ROUTER_STATUS_VIEW_ITEM_H
#define ROUTER_STATUS_VIEW_ITEM_H

#include <cstdio>
#include <QString>

#include <view/view.h>
#include <view/view_item.h>
#include <view/view_group.h>

#include <math/vector2d.h>
#include <math/box2.h>

#include <geometry/shape_line_chain.h>
#include <geometry/shape_circle.h>

#include <gal/color4d.h>

#include <eda_item.h>


class ROUTER_STATUS_VIEW_ITEM : public EDA_ITEM
{
public:
    ROUTER_STATUS_VIEW_ITEM() :
            EDA_ITEM( NOT_USED )    // Never added to anything - just a preview
    { }

    QString GetClass() const override { return "ROUTER_STATUS"; }

#if defined(DEBUG)
    void Show( int nestLevel, std::ostream& os ) const override {}
#endif

    VECTOR2I GetPosition() const override { return m_pos; }
    void     SetPosition( const VECTOR2I& aPos ) override { m_pos = aPos; };

    void SetMessage( const QString& aStatus )
    {
        m_status = aStatus;
    }

    void SetHint( const QString& aHint )
    {
        m_hint = aHint;
    }

    const BOX2I ViewBBox() const override;
    std::vector<int> ViewGetLayers() const override;
    void ViewDraw( int aLayer, KIGFX::VIEW* aView ) const override;

protected:
    VECTOR2I m_pos;
    QString m_status;
    QString m_hint;
};



#endif  // ROUTER_STATUS_VIEW_ITEM_H
