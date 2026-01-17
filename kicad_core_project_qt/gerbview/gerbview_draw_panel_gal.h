
#ifndef GERBVIEW_DRAW_PANEL_GAL_H_
#define GERBVIEW_DRAW_PANEL_GAL_H_

#include <class_draw_panel_gal.h>

class DS_PROXY_VIEW_ITEM;


class GERBVIEW_DRAW_PANEL_GAL : public EDA_DRAW_PANEL_GAL
{
public:
    GERBVIEW_DRAW_PANEL_GAL( QWidget* aParentWindow, int aWindowId,
                             const QPoint& aPosition, const QSize& aSize,
                             KIGFX::GAL_DISPLAY_OPTIONS& aOptions,
                             GAL_TYPE aGalType = GAL_TYPE_OPENGL );

    virtual ~GERBVIEW_DRAW_PANEL_GAL();

    ///< @copydoc EDA_DRAW_PANEL_GAL::SetHighContrastLayer()
    virtual void SetHighContrastLayer( int aLayer ) override;

    ///< @copydoc EDA_DRAW_PANEL_GAL::GetMsgPanelInfo()
    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    ///< @copydoc EDA_DRAW_PANEL_GAL::OnShow()
    void OnShow() override;

    bool SwitchBackend( GAL_TYPE aGalType ) override;

    ///< @copydoc EDA_DRAW_PANEL_GAL::SetTopLayer
    virtual void SetTopLayer( int aLayer ) override;

    ///< @copydoc EDA_DRAW_PANEL_GAL::GetDefaultViewBBox()
    BOX2I GetDefaultViewBBox() const override;

    /**
     * Set or update the drawing-sheet (borders and title block) used by the draw panel.
     *
     * @param aDrawingSheet is the drawing-sheet to be used.
     *        The object is then owned by GERBVIEW_DRAW_PANEL_GAL.
     */
    void SetDrawingSheet( DS_PROXY_VIEW_ITEM* aDrawingSheet );

    /**
     * @return the current drawing-sheet
     */
    DS_PROXY_VIEW_ITEM* GetDrawingSheet() const { return m_drawingSheet.get(); }

protected:
    ///< Set rendering targets & dependencies for layers.
    void setDefaultLayerDeps();

    ///< Currently used drawing-sheet (borders and title block)
    std::unique_ptr<DS_PROXY_VIEW_ITEM> m_drawingSheet;
};


#endif /* GERBVIEW_DRAW_PANEL_GAL_H_ */
