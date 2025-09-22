
#ifndef __DIALOG_IMPORT_GFX_SCH_H__
#define __DIALOG_IMPORT_GFX_SCH_H__

#include "dialog_import_gfx_sch_base.h"

#include <import_gfx/graphics_importer.h>
#include <widgets/unit_binder.h>

class SCH_BASE_FRAME;
class GRAPHICS_IMPORT_MGR;


class DIALOG_IMPORT_GFX_SCH : public DIALOG_IMPORT_GFX_SCH_BASE
{
public:
    DIALOG_IMPORT_GFX_SCH( SCH_BASE_FRAME* aParent );
    ~DIALOG_IMPORT_GFX_SCH();

    /**
     * @return a list of items imported from a vector graphics file.
     */
    std::list<std::unique_ptr<EDA_ITEM>>& GetImportedItems()
    {
        return m_importer->GetItems();
    }

    /**
     * @return true if the placement is interactive, i.e. all imported
     * items must be moved by the mouse cursor to the final position
     * false means the imported items are placed to the final position after import.
     */
    bool IsPlacementInteractive() { return m_placementInteractive; }

    bool TransferDataFromWindow() override;

private:
    // Virtual event handlers
    void onBrowseFiles() override;
    void onFilename();
    void originOptionOnUpdateUI() override;

	void onInteractivePlacement() override
    {
        m_placementInteractive = true;
    }

	void onAbsolutePlacement() override
    {
        m_placementInteractive = false;
    }

private:
    SCH_BASE_FRAME*                      m_parent;
    std::unique_ptr<GRAPHICS_IMPORTER>   m_importer;
    std::unique_ptr<GRAPHICS_IMPORT_MGR> m_gfxImportMgr;

    UNIT_BINDER          m_xOrigin;
    UNIT_BINDER          m_yOrigin;
    UNIT_BINDER          m_defaultLineWidth;

    static bool          m_placementInteractive;
    static double        m_importScale;         // a scale factor to change the size of imported
                                                // items m_importScale =1.0 means keep original size
};

#endif    //  __DIALOG_IMPORT_GFX_SCH_H__
