

#ifndef SCH_PREVIEW_PANEL_H
#define SCH_PREVIEW_PANEL_H

#include <class_draw_panel_gal.h>
#include <QtGui/QPaintEvent>
#include <QtCore/QRect>


class SCH_RENDER_SETTINGS;

namespace KIGFX {
    class SCH_VIEW;
    namespace PREVIEW {
        class SELECTION_AREA;
    };
};


class SCH_PREVIEW_PANEL : public EDA_DRAW_PANEL_GAL
{
public:
    SCH_PREVIEW_PANEL( QWidget* aParentWindow, int aWindowId, const QPoint& aPosition,
                       const QSize& aSize, KIGFX::GAL_DISPLAY_OPTIONS& aOptions,
                       GAL_TYPE aGalType = GAL_TYPE_OPENGL );

    ~SCH_PREVIEW_PANEL() override;

    ///< @copydoc EDA_DRAW_PANEL_GAL::OnShow()
    void OnShow() override;

    /// @copydoc QWidget::repaint()
    void Refresh( bool aEraseBackground, const QRect* aRect ) override;

    SCH_RENDER_SETTINGS* GetRenderSettings() const;

protected:

    void onPaint( QPaintEvent& aEvent ) override;

    KIGFX::SCH_VIEW* view() const;

    ///< Reassign layer order to the initial settings.
    void setDefaultLayerOrder();

    ///< Set rendering targets & dependencies for layers.
    void setDefaultLayerDeps();
};

#endif
