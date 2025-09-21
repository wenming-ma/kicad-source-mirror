
#ifndef __SCH_DRAW_PANEL_H
#define __SCH_DRAW_PANEL_H

#include <class_draw_panel_gal.h>
#include <sch_view.h>


class LIB_SYMBOL;
class SCH_SCREEN;


class SCH_DRAW_PANEL : public EDA_DRAW_PANEL_GAL
{
public:
    SCH_DRAW_PANEL( QWidget* aParentWindow, int aWindowId, const QPoint& aPosition,
                    const QSize& aSize, KIGFX::GAL_DISPLAY_OPTIONS& aOptions,
                    GAL_TYPE aGalType = GAL_TYPE_OPENGL );

    ~SCH_DRAW_PANEL();

    void DisplaySymbol( LIB_SYMBOL *aSymbol );
    void DisplaySheet( SCH_SCREEN *aScreen );

    bool SwitchBackend( GAL_TYPE aGalType ) override;

    KIGFX::SCH_VIEW* GetView() const override;

protected:
    virtual void onPaint( QPaintEvent& aEvent ) override;

    void OnShow() override;

    void setDefaultLayerOrder();    ///< Reassign layer order to the initial settings.
    void setDefaultLayerDeps();     ///< Set rendering targets & dependencies for layers.
};

#endif // __SCH_DRAW_PANEL_H
