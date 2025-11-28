#ifndef TOOLS_SCHEMATIC_LAYOUT_TOOL_H
#define TOOLS_SCHEMATIC_LAYOUT_TOOL_H

#include <tools/pcb_tool_base.h>
#include <map>
#include <vector>

class KIID_PATH;
class FOOTPRINT;

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

    /**
     * Aggressively compact footprints by scaling toward centroid.
     * Uses binary search to find optimal compression while avoiding overlaps.
     *
     * @param aFootprints List of footprints to compact
     * @param aGroupName Name for logging (e.g., "Sheet1")
     * @return Compression ratio (0.0 = 100% compressed, 1.0 = no compression)
     */
    double compactFootprintsAggressively( std::vector<FOOTPRINT*>& aFootprints,
                                          const wxString& aGroupName );

    /**
     * Calculate geometric centroid (center point) of footprint positions.
     *
     * @param aFootprints List of footprints
     * @return Centroid position in PCB_IU units
     */
    VECTOR2I calculateCentroid( const std::vector<FOOTPRINT*>& aFootprints ) const;

    /**
     * Check if any footprints have overlapping bounding boxes.
     * Uses brute-force O(N²) algorithm with 0.1mm clearance tolerance.
     *
     * @param aFootprints List of footprints to check
     * @return true if any overlaps detected
     */
    bool hasOverlaps( const std::vector<FOOTPRINT*>& aFootprints ) const;

    /**
     * Scale all footprint positions toward centroid by given factor.
     *
     * @param aFootprints List of footprints to scale
     * @param aCentroid Center point for scaling
     * @param aScaleFactor Scale factor (0.0 to 1.0)
     * @param aOriginalPositions Original positions before scaling
     */
    void scaleTowardCentroid( std::vector<FOOTPRINT*>& aFootprints,
                              const VECTOR2I& aCentroid,
                              double aScaleFactor,
                              const std::map<FOOTPRINT*, VECTOR2I>& aOriginalPositions );

    /**
     * Calculate the bounding box of a group of footprints.
     *
     * @param aFootprints List of footprints
     * @return Bounding box in PCB_IU units
     */
    BOX2I calculateGroupBoundingBox( const std::vector<FOOTPRINT*>& aFootprints ) const;

    /**
     * Pack sheet groups using rectangle packing algorithm.
     * Each sheet is treated as a rectangle and packed efficiently.
     *
     * @param aSheetGroups Map of sheet paths to footprint lists
     */
    void packSheetGroups( std::map<KIID_PATH, std::vector<FOOTPRINT*>>& aSheetGroups );
};


#endif // TOOLS_SCHEMATIC_LAYOUT_TOOL_H
