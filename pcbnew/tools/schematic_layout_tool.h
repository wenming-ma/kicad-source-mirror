#ifndef TOOLS_SCHEMATIC_LAYOUT_TOOL_H
#define TOOLS_SCHEMATIC_LAYOUT_TOOL_H

#include <tools/pcb_tool_base.h>
#include <map>

class KIID_PATH;

class SCHEMATIC_LAYOUT_TOOL : public PCB_TOOL_BASE
{
public:
    SCHEMATIC_LAYOUT_TOOL();
    ~SCHEMATIC_LAYOUT_TOOL();

    void setTransitions() override;

private:
    int layoutFromSchematic( const TOOL_EVENT& aEvent );

    bool parseSchematicPositions( const wxString& aSchematicPath,
                                  std::map<KIID_PATH, VECTOR2I>& aPositions,
                                  std::map<KIID_PATH, int>& aSheetOffsets );

    /**
     * Calculate the bounding box of all items on a schematic screen.
     *
     * @param aScreen The schematic screen to calculate bounding box for.
     * @return BOX2I The bounding box in schematic internal units (SCH_IU).
     */
    BOX2I getScreenBoundingBox( SCH_SCREEN* aScreen ) const;
};


#endif // TOOLS_SCHEMATIC_LAYOUT_TOOL_H
