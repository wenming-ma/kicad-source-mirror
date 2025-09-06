// QT_TRANSFORMATION_COMPLETED

#ifndef LAYER_SETTINGS_UTILS_H
#define LAYER_SETTINGS_UTILS_H

#include <optional>
#include <string>
#include <layer_ids.h>

enum class VISIBILITY_LAYER
{
    TRACKS,
    VIAS,
    PADS,
    ZONES,
    SHAPES,
    BITMAPS,
    FOOTPRINTS_FRONT,
    FOOTPRINTS_BACK,
    FOOTPRINT_VALUES,
    FOOTPRINT_REFERENCES,
    FOOTPRINT_TEXT,
    FOOTPRINT_ANCHORS,
    RATSNEST,
    DRC_WARNINGS,
    DRC_ERRORS,
    DRC_EXCLUSIONS,
    LOCKED_ITEM_SHADOWS,
    CONFLICT_SHADOWS,
    DRAWING_SHEET,
    GRID
};

GAL_SET UserVisbilityLayers();

GAL_LAYER_ID RenderLayerFromVisibilityLayer( VISIBILITY_LAYER aLayer );
std::optional<VISIBILITY_LAYER> VisibilityLayerFromRenderLayer( GAL_LAYER_ID aLayerId );

std::optional<GAL_LAYER_ID> RenderLayerFromVisbilityString( const std::string& aLayer );
std::string VisibilityLayerToString( VISIBILITY_LAYER aLayerId );


#endif //LAYER_SETTINGS_UTILS_H
