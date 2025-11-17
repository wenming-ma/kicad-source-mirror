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
                                  std::map<wxString, int>& aSheetOffsets );
};


#endif // TOOLS_SCHEMATIC_LAYOUT_TOOL_H
