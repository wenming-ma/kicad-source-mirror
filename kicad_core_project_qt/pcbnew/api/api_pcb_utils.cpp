#include <api/api_pcb_utils.h>
#include <api/api_enums.h>
#include <board.h>
#include <board_item_container.h>
#include <footprint.h>
#include <lset.h>
#include <pad.h>
#include <pcb_group.h>
#include <pcb_reference_image.h>
#include <pcb_shape.h>
#include <pcb_track.h>
#include <pcb_text.h>
#include <pcb_textbox.h>
#include <zone.h>


std::unique_ptr<BOARD_ITEM> CreateItemForType( KICAD_T aType, BOARD_ITEM_CONTAINER* aContainer )
{
    switch( aType )
    {
    case PCB_TRACE_T:   return std::make_unique<PCB_TRACK>( aContainer );
    case PCB_ARC_T:     return std::make_unique<PCB_ARC>( aContainer );
    case PCB_VIA_T:     return std::make_unique<PCB_VIA>( aContainer );
    case PCB_TEXT_T:    return std::make_unique<PCB_TEXT>( aContainer );
    case PCB_TEXTBOX_T: return std::make_unique<PCB_TEXTBOX>( aContainer );
    case PCB_SHAPE_T:   return std::make_unique<PCB_SHAPE>( aContainer );
    case PCB_ZONE_T:    return std::make_unique<ZONE>( aContainer );
    case PCB_GROUP_T:   return std::make_unique<PCB_GROUP>( aContainer );
    case PCB_REFERENCE_IMAGE_T: return std::make_unique<PCB_REFERENCE_IMAGE>( aContainer );

    case PCB_PAD_T:
    {
        FOOTPRINT* footprint = dynamic_cast<FOOTPRINT*>( aContainer );

        if( !footprint )
            return nullptr;

        return std::make_unique<PAD>( footprint );
    }

    case PCB_FOOTPRINT_T:
    {
        BOARD* board = dynamic_cast<BOARD*>( aContainer );

        if( !board )
            return nullptr;

        return std::make_unique<FOOTPRINT>( board );
    }

    default:
        return nullptr;
    }
}

namespace kiapi::board
{

// void PackLayerSet( google::protobuf::RepeatedField<int>& aOutput, const LSET& aLayerSet )
// {
//     for( const PCB_LAYER_ID& layer : aLayerSet.Seq() )
//         aOutput.Add( ToProtoEnum<PCB_LAYER_ID, types::BoardLayer>( layer ) );
// }


// LSET UnpackLayerSet( const google::protobuf::RepeatedField<int>& aProtoLayerSet )
// {
//     LSET set;
// 
//     for( int layer : aProtoLayerSet )
//     {
//         Q_ASSERT( layer >= F_Cu && layer < PCB_LAYER_ID_COUNT ); if( !( layer >= F_Cu && layer < PCB_LAYER_ID_COUNT ) ) continue;
//         PCB_LAYER_ID boardLayer =
//                 FromProtoEnum<PCB_LAYER_ID>( static_cast<types::BoardLayer>( layer ) );
//         set.set( boardLayer );
//     }
// 
//     return set;
// }

}   // namespace kiapi::board
