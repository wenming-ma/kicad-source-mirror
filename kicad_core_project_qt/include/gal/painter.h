
#ifndef __CLASS_PAINTER_H
#define __CLASS_PAINTER_H

#include <map>
#include <set>

#include <QtGui/QPainter>

#include <gal/gal.h>
#include <gal/color4d.h>
#include <render_settings.h>
#include <layer_ids.h>
#include <memory>

namespace KIGFX
{
class GAL;
class VIEW_ITEM;

class GAL_API PAINTER
{
public:
    PAINTER( GAL* aGal );
    virtual ~PAINTER();

    void SetGAL( GAL* aGal )
    {
        m_gal = aGal;
    }

    virtual RENDER_SETTINGS* GetSettings() = 0;

    virtual bool Draw( const VIEW_ITEM* aItem, int aLayer ) = 0;

protected:
    GAL* m_gal;
};

} // namespace KIGFX

#endif /* __CLASS_PAINTER_H */
