

// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#ifndef DIALOG_LAYERS_SELECT_TO_PCB_H_
#define DIALOG_LAYERS_SELECT_TO_PCB_H_

#include <dialogs/dialog_layers_select_to_pcb_base.h>
#include <layer_ids.h>

class GERBVIEW_FRAME;

/**
 * Show the Gerber files loaded and allow the user to choose between Gerber layers and pcb layers.
 */
class LAYERS_MAP_DIALOG : public LAYERS_MAP_DIALOG_BASE
{
public: LAYERS_MAP_DIALOG( GERBVIEW_FRAME* parent );
    ~LAYERS_MAP_DIALOG() {};

    int* GetLayersLookUpTable() { return m_layersLookUpTable; }
    static int GetCopperLayersCount() { return m_exportBoardCopperLayersCount; }

protected:
    bool TransferDataFromWindow() override;

private:
    void initDialog();
    void normalizeBrdLayersCount();
    void OnBrdLayersCountSelection( QEvent* event ) override;
    void OnSelectLayer( QEvent* event );

    void OnStoreSetup( QEvent* event ) override;
    void OnGetSetup( QEvent* event ) override;
    void OnResetClick( QEvent* event ) override;

    /**
     * Find number of loaded Gerbers where the matching KiCad layer can be identified.
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have its entry set to the KiCad PCB layer
     * number.  Gerbers that can be identified or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber.
     *
     * @return The number of loaded Gerbers that have Altium extensions.
     */
    int findKnownGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Find number of loaded Gerbers using Altium file extensions.
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have its entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using Altium extensions or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber.
     *
     * @return The number of loaded Gerbers that have Altium extensions.
     */
    int findNumAltiumGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Find number of loaded Gerbers using KiCad naming convention.
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have its entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using KiCad naming or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber.
     *
     * @return The number of loaded Gerbers using KiCad naming conventions.
     */
    int findNumKiCadGerbersLoaded( std::vector<int>& aGerber2KicadMapping );

    /**
     * Find number of loaded Gerbers using X2 File Functions to define layers.
     *
     * The passed vector<int> will be returned with the same number of elements
     * as there are Gerber files.  The indices into it are 1:1 with the loaded Gerber
     * files.  Any Gerber that maps will have its entry set to the KiCad PCB layer
     * number.  Gerbers that aren't using X2 File functions or which don't map to an
     * equivalent KiCad PCB layer will be set to UNSELECTED_LAYER.
     *
     * @param aGerber2KicadMapping passed to return KiCad PCB layer number for each Gerber.
     *
     * @return The number of loaded Gerbers with X2 File Functions.
     */
    int findNumX2GerbersLoaded( std::vector<int>& aGerber2KicadMapping );


private:
    GERBVIEW_FRAME* m_Parent;
    int             m_gerberActiveLayersCount;             // Number of initialized Gerber layers
    static int      m_exportBoardCopperLayersCount;

    // Indexes Gerber layers to PCB file layers; the last value in table is the number of
    // copper layers.
    int             m_layersLookUpTable[ GERBER_DRAWLAYERS_COUNT ];
    int             m_buttonTable[ int(GERBER_DRAWLAYERS_COUNT) + 1 ];
    QLabel*         m_layersList[ int(GERBER_DRAWLAYERS_COUNT) + 1 ];
};

#endif      // DIALOG_LAYERS_SELECT_TO_PCB_H_
