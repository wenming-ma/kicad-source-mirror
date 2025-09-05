
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#ifndef PCB_DRAW_PANEL_GAL_H_
#define PCB_DRAW_PANEL_GAL_H_

#include <class_draw_panel_gal.h>
#include <layer_ids.h>
#include <pcb_view.h>

class DS_PROXY_VIEW_ITEM;
class RATSNEST_VIEW_ITEM;
class PROGRESS_REPORTER;

class PCB_DRAW_PANEL_GAL : public EDA_DRAW_PANEL_GAL
{
public:
    PCB_DRAW_PANEL_GAL( QWidget* aParentWindow, int aWindowId, const QPoint& aPosition,
                        const QSize& aSize, KIGFX::GAL_DISPLAY_OPTIONS& aOptions,
                        GAL_TYPE aGalType = GAL_TYPE_OPENGL );

    virtual ~PCB_DRAW_PANEL_GAL();

    // Add all items from the current board to the VIEW, so they can be displayed by GAL.
    void DisplayBoard( BOARD* aBoard, PROGRESS_REPORTER* aReporter = nullptr );

    // Sets (or updates) drawing-sheet used by the draw panel.
    void SetDrawingSheet( DS_PROXY_VIEW_ITEM* aDrawingSheet );

    DS_PROXY_VIEW_ITEM* GetDrawingSheet() const { return m_drawingSheet.get(); }

    // Update the color settings in the painter and GAL.
    void UpdateColors();

    // @copydoc EDA_DRAW_PANEL_GAL::SetHighContrastLayer()
    virtual void SetHighContrastLayer( int aLayer ) override
    {
        SetHighContrastLayer( static_cast< PCB_LAYER_ID >( aLayer ) );
    }

    // SetHighContrastLayer(), with some extra smarts for PCB.
    void SetHighContrastLayer( PCB_LAYER_ID aLayer );

    // @copydoc EDA_DRAW_PANEL_GAL::SetTopLayer()
    virtual void SetTopLayer( int aLayer ) override
    {
        SetTopLayer( static_cast< PCB_LAYER_ID >( aLayer ) );
    }

    // SetTopLayer(), with some extra smarts for PCB.
    void SetTopLayer( PCB_LAYER_ID aLayer );

    // Update "visibility" property of each layer of a given BOARD.
    void SyncLayersVisibility( const BOARD* aBoard );

    // @copydoc EDA_DRAW_PANEL_GAL::GetMsgPanelInfo()
    void GetMsgPanelInfo( EDA_DRAW_FRAME* aFrame, std::vector<MSG_PANEL_ITEM>& aList ) override;

    // @copydoc EDA_DRAW_PANEL_GAL::OnShow()
    void OnShow() override;

    bool SwitchBackend( GAL_TYPE aGalType ) override;

    // Force refresh of the ratsnest visual representation.
    void RedrawRatsnest();

    // @copydoc EDA_DRAW_PANEL_GAL::GetDefaultViewBBox()
    BOX2I GetDefaultViewBBox() const override;

    virtual KIGFX::PCB_VIEW* GetView() const override;

protected:
    // Reassign layer order to the initial settings.
    void setDefaultLayerOrder();

    // Set rendering targets & dependencies for layers.
    void setDefaultLayerDeps();

protected:
    std::unique_ptr<DS_PROXY_VIEW_ITEM> m_drawingSheet;  // Currently used drawing-sheet
    std::unique_ptr<RATSNEST_VIEW_ITEM> m_ratsnest;      // Ratsnest view item
};

#endif /* PCB_DRAW_PANEL_GAL_H_ */
