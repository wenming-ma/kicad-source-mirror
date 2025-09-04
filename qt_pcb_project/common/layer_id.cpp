#include <layer_ids.h>
#include <magic_enum.hpp>
#include <QString>
#include <QObject>

QString LayerName( int aLayer )
{
    switch( aLayer )
    {
    // PCB_LAYER_ID
    case UNDEFINED_LAYER:   return QObject::tr( "undefined" );

    // Copper
    case PCB_LAYER_ID::F_Cu:              return "F.Cu";
    case PCB_LAYER_ID::B_Cu:              return "B.Cu";

    // Technicals
    case PCB_LAYER_ID::B_Adhes:           return "B.Adhesive";
    case PCB_LAYER_ID::F_Adhes:           return "F.Adhesive";
    case PCB_LAYER_ID::B_Paste:           return "B.Paste";
    case PCB_LAYER_ID::F_Paste:           return "F.Paste";
    case PCB_LAYER_ID::B_SilkS:           return "B.Silkscreen";
    case PCB_LAYER_ID::F_SilkS:           return "F.Silkscreen";
    case PCB_LAYER_ID::B_Mask:            return "B.Mask";
    case PCB_LAYER_ID::F_Mask:            return "F.Mask";

    // Users
    case PCB_LAYER_ID::Dwgs_User:         return "User.Drawings";
    case PCB_LAYER_ID::Cmts_User:         return "User.Comments";
    case PCB_LAYER_ID::Eco1_User:         return "User.Eco1";
    case PCB_LAYER_ID::Eco2_User:         return "User.Eco2";
    case PCB_LAYER_ID::Edge_Cuts:         return "Edge.Cuts";
    case PCB_LAYER_ID::Margin:            return "Margin";

    // Footprint
    case PCB_LAYER_ID::F_CrtYd:           return "F.Courtyard";
    case PCB_LAYER_ID::B_CrtYd:           return "B.Courtyard";
    case PCB_LAYER_ID::F_Fab:             return "F.Fab";
    case PCB_LAYER_ID::B_Fab:             return "B.Fab";

    // Rescue
    case PCB_LAYER_ID::Rescue:            return QObject::tr( "Rescue" );

    // SCH_LAYER_ID

    case LAYER_WIRE:                    return QObject::tr( "Wires" );
    case LAYER_BUS:                     return QObject::tr( "Buses" );
    case LAYER_BUS_JUNCTION:            return QObject::tr( "Bus junctions" );
    case LAYER_JUNCTION:                return QObject::tr( "Junctions" );
    case LAYER_LOCLABEL:                return QObject::tr( "Labels" );
    case LAYER_GLOBLABEL:               return QObject::tr( "Global labels" );
    case LAYER_HIERLABEL:               return QObject::tr( "Hierarchical labels" );
    case LAYER_PINNUM:                  return QObject::tr( "Pin numbers" );
    case LAYER_PINNAM:                  return QObject::tr( "Pin names" );
    case LAYER_REFERENCEPART:           return QObject::tr( "Symbol references" );
    case LAYER_VALUEPART:               return QObject::tr( "Symbol values" );
    case LAYER_FIELDS:                  return QObject::tr( "Symbol fields" );
    case LAYER_INTERSHEET_REFS:         return QObject::tr( "Sheet references" );
    case LAYER_NETCLASS_REFS:           return QObject::tr( "Net class references" );
    case LAYER_RULE_AREAS:              return QObject::tr( "Rule areas" );
    case LAYER_DEVICE:                  return QObject::tr( "Symbol body outlines" );
    case LAYER_DEVICE_BACKGROUND:       return QObject::tr( "Symbol body fills" );
    case LAYER_SHAPES_BACKGROUND:       return QObject::tr( "Shape fills" );
    case LAYER_NOTES:                   return QObject::tr( "Schematic text && graphics" );
    case LAYER_PRIVATE_NOTES:           return QObject::tr( "Symbol private text && graphics" );
    case LAYER_NOTES_BACKGROUND:        return QObject::tr( "Schematic text && graphics backgrounds" );
    case LAYER_PIN:                     return QObject::tr( "Pins" );
    case LAYER_SHEET:                   return QObject::tr( "Sheet borders" );
    case LAYER_SHEET_BACKGROUND:        return QObject::tr( "Sheet backgrounds" );
    case LAYER_SHEETNAME:               return QObject::tr( "Sheet names" );
    case LAYER_SHEETFIELDS:             return QObject::tr( "Sheet fields" );
    case LAYER_SHEETFILENAME:           return QObject::tr( "Sheet file names" );
    case LAYER_SHEETLABEL:              return QObject::tr( "Sheet pins" );
    case LAYER_NOCONNECT:               return QObject::tr( "No-connect symbols" );
    case LAYER_DNP_MARKER:              return QObject::tr( "DNP markers" );
    case LAYER_EXCLUDED_FROM_SIM:       return QObject::tr( "Excluded-from-simulation markers" );
    case LAYER_ERC_WARN:                return QObject::tr( "ERC warnings" );
    case LAYER_ERC_ERR:                 return QObject::tr( "ERC errors" );
    case LAYER_ERC_EXCLUSION:           return QObject::tr( "ERC exclusions" );
    case LAYER_SCHEMATIC_ANCHOR:        return QObject::tr( "Anchors" );
    case LAYER_SCHEMATIC_AUX_ITEMS:     return QObject::tr( "Helper items" );
    case LAYER_SCHEMATIC_GRID:          return QObject::tr( "Grid" );
    case LAYER_SCHEMATIC_GRID_AXES:     return QObject::tr( "Axes" );
    case LAYER_SCHEMATIC_BACKGROUND:    return QObject::tr( "Background" );
    case LAYER_SCHEMATIC_CURSOR:        return QObject::tr( "Cursor" );
    case LAYER_HOVERED:                 return QObject::tr( "Hovered items" );
    case LAYER_BRIGHTENED:              return QObject::tr( "Highlighted items" );
    case LAYER_HIDDEN:                  return QObject::tr( "Hidden items" );
    case LAYER_SELECTION_SHADOWS:       return QObject::tr( "Selection highlight" );
    case LAYER_NET_COLOR_HIGHLIGHT:     return QObject::tr( "Net color highlight" );
    case LAYER_SCHEMATIC_DRAWINGSHEET:  return QObject::tr( "Drawing sheet" );
    case LAYER_SCHEMATIC_PAGE_LIMITS:   return QObject::tr( "Page limits" );
    case LAYER_OP_VOLTAGES:             return QObject::tr( "Operating point voltages" );
    case LAYER_OP_CURRENTS:             return QObject::tr( "Operating point currents" );

    // GAL_LAYER_ID

    case LAYER_FOOTPRINTS_FR:           return QObject::tr( "Footprints front" );
    case LAYER_FOOTPRINTS_BK:           return QObject::tr( "Footprints back" );
    case LAYER_FP_VALUES:               return QObject::tr( "Values" );
    case LAYER_FP_REFERENCES:           return QObject::tr( "Reference designators" );
    case LAYER_FP_TEXT:                 return QObject::tr( "Footprint text" );
    case LAYER_TRACKS:                  return QObject::tr( "Tracks" );
    case LAYER_VIA_THROUGH:             return QObject::tr( "Through vias" );
    case LAYER_VIA_BBLIND:              return QObject::tr( "Blind/Buried vias" );
    case LAYER_VIA_MICROVIA:            return QObject::tr( "Micro-vias" );
    case LAYER_VIA_HOLES:               return QObject::tr( "Via holes" );
    case LAYER_VIA_HOLEWALLS:           return QObject::tr( "Via hole walls" );
    case LAYER_PAD_PLATEDHOLES:         return QObject::tr( "Plated holes" );
    case LAYER_PAD_HOLEWALLS:           return QObject::tr( "Plated hole walls" );
    case LAYER_NON_PLATEDHOLES:         return QObject::tr( "Non-plated holes" );
    case LAYER_RATSNEST:                return QObject::tr( "Ratsnest" );
    case LAYER_DRC_WARNING:             return QObject::tr( "DRC warnings" );
    case LAYER_DRC_ERROR:               return QObject::tr( "DRC errors" );
    case LAYER_DRC_SHAPE1:              return QObject::tr( "DRC shape 1" );
    case LAYER_DRC_SHAPE2:              return QObject::tr( "DRC shape 2" );
    case LAYER_DRC_EXCLUSION:           return QObject::tr( "DRC exclusions" );
    case LAYER_MARKER_SHADOWS:          return QObject::tr( "DRC marker shadows" );
    case LAYER_ANCHOR:                  return QObject::tr( "Anchors" );
    case LAYER_DRAWINGSHEET:            return QObject::tr( "Drawing sheet" );
    case LAYER_PAGE_LIMITS:             return QObject::tr( "Page limits" );
    case LAYER_CURSOR:                  return QObject::tr( "Cursor" );
    case LAYER_AUX_ITEMS:               return QObject::tr( "Helper items" );
    case LAYER_GRID:                    return QObject::tr( "Grid" );
    case LAYER_GRID_AXES:               return QObject::tr( "Grid axes" );
    case LAYER_PCB_BACKGROUND:          return QObject::tr( "Background" );
    case LAYER_SELECT_OVERLAY:          return QObject::tr( "Selection highlight" );
    case LAYER_LOCKED_ITEM_SHADOW:      return QObject::tr( "Locked item shadow" );
    case LAYER_CONFLICTS_SHADOW:        return QObject::tr( "Courtyard collision shadow" );
    case NETNAMES_LAYER_ID_START:       return QObject::tr( "Track net names" );
    case LAYER_PAD_NETNAMES:            return QObject::tr( "Pad net names" );
    case LAYER_VIA_NETNAMES:            return QObject::tr( "Via net names" );

    default:
        // Catch the general board layers that have numerically increasing names
        if( aLayer > 0 && aLayer < PCB_LAYER_ID_COUNT && aLayer & 1 )
            return QString( "User.%1" ).arg( ( aLayer - PCB_LAYER_ID::User_1 ) / 2 + 1 );

        return QString( "In%1.Cu" ).arg( ( aLayer - PCB_LAYER_ID::In1_Cu ) / 2 + 1 );
    }
}


PCB_LAYER_ID FlipLayer( PCB_LAYER_ID aLayerId, int aCopperLayersCount )
{
    switch( aLayerId )
    {
    case B_Cu:              return F_Cu;
    case F_Cu:              return B_Cu;

    case B_SilkS:           return F_SilkS;
    case F_SilkS:           return B_SilkS;

    case B_Adhes:           return F_Adhes;
    case F_Adhes:           return B_Adhes;

    case B_Mask:            return F_Mask;
    case F_Mask:            return B_Mask;

    case B_Paste:           return F_Paste;
    case F_Paste:           return B_Paste;

    case B_CrtYd:           return F_CrtYd;
    case F_CrtYd:           return B_CrtYd;

    case B_Fab:             return F_Fab;
    case F_Fab:             return B_Fab;

    default:    // change internal layer if aCopperLayersCount is >= 4
        if( IsCopperLayer( aLayerId ) && aCopperLayersCount >= 4 )
        {
            // internal copper layers count is aCopperLayersCount-2
            PCB_LAYER_ID fliplayer = PCB_LAYER_ID(aCopperLayersCount - 2 - ( aLayerId - In1_Cu ) );
            // Ensure fliplayer has a value which does not crash Pcbnew:
            if( fliplayer < F_Cu )
                fliplayer = F_Cu;

            if( fliplayer > B_Cu )
                fliplayer = B_Cu;

            return fliplayer;
        }

        // No change for the other layers
        return aLayerId;
    }
}


PCB_LAYER_ID BoardLayerFromLegacyId( int aLegacyId )
{
    switch( aLegacyId )
    {
    case 0:  return F_Cu;
    case 31: return B_Cu;

    default:
        if( aLegacyId < 0 )
            return magic_enum::enum_cast<PCB_LAYER_ID>( aLegacyId ).value_or( UNDEFINED_LAYER );

        if( aLegacyId < 31 )
            return static_cast<PCB_LAYER_ID>( In1_Cu + ( aLegacyId - 1 ) * 2 );

        switch( aLegacyId )
        {
        case 32: return B_Adhes;
        case 33: return F_Adhes;
        case 34: return B_Paste;
        case 35: return F_Paste;
        case 36: return B_SilkS;
        case 37: return F_SilkS;
        case 38: return B_Mask;
        case 39: return F_Mask;
        case 40: return Dwgs_User;
        case 41: return Cmts_User;
        case 42: return Eco1_User;
        case 43: return Eco2_User;
        case 44: return Edge_Cuts;
        case 45: return Margin;
        case 46: return B_CrtYd;
        case 47: return F_CrtYd;
        case 48: return B_Fab;
        case 49: return F_Fab;
        case 50: return User_1;
        case 51: return User_2;
        case 52: return User_3;
        case 53: return User_4;
        case 54: return User_5;
        case 55: return User_6;
        case 56: return User_7;
        case 57: return User_8;
        case 58: return User_9;
        case 59: return Rescue;
        default: return UNDEFINED_LAYER;
        }
    }
}


PCB_LAYER_ID Map3DLayerToPCBLayer( int aLayer )
{
    // NOTE: User_1..User45 are NOT consecutive numbers!

    switch( aLayer )
    {
    case LAYER_3D_COPPER_TOP:        return F_Cu;
    case LAYER_3D_COPPER_BOTTOM:     return B_Cu;
    case LAYER_3D_SILKSCREEN_BOTTOM: return B_SilkS;
    case LAYER_3D_SILKSCREEN_TOP:    return F_SilkS;
    case LAYER_3D_SOLDERMASK_BOTTOM: return B_Mask;
    case LAYER_3D_SOLDERMASK_TOP:    return F_Mask;
    case LAYER_3D_USER_COMMENTS:     return Cmts_User;
    case LAYER_3D_USER_DRAWINGS:     return Dwgs_User;
    case LAYER_3D_USER_ECO1:         return Eco1_User;
    case LAYER_3D_USER_ECO2:         return Eco2_User;
    case LAYER_3D_USER_1:            return User_1;
    case LAYER_3D_USER_2:            return User_2;
    case LAYER_3D_USER_3:            return User_3;
    case LAYER_3D_USER_4:            return User_4;
    case LAYER_3D_USER_5:            return User_5;
    case LAYER_3D_USER_6:            return User_6;
    case LAYER_3D_USER_7:            return User_7;
    case LAYER_3D_USER_8:            return User_8;
    case LAYER_3D_USER_9:            return User_9;
    case LAYER_3D_USER_10:           return User_10;
    case LAYER_3D_USER_11:           return User_11;
    case LAYER_3D_USER_12:           return User_12;
    case LAYER_3D_USER_13:           return User_13;
    case LAYER_3D_USER_14:           return User_14;
    case LAYER_3D_USER_15:           return User_15;
    case LAYER_3D_USER_16:           return User_16;
    case LAYER_3D_USER_17:           return User_17;
    case LAYER_3D_USER_18:           return User_18;
    case LAYER_3D_USER_19:           return User_19;
    case LAYER_3D_USER_20:           return User_20;
    case LAYER_3D_USER_21:           return User_21;
    case LAYER_3D_USER_22:           return User_22;
    case LAYER_3D_USER_23:           return User_23;
    case LAYER_3D_USER_24:           return User_24;
    case LAYER_3D_USER_25:           return User_25;
    case LAYER_3D_USER_26:           return User_26;
    case LAYER_3D_USER_27:           return User_27;
    case LAYER_3D_USER_28:           return User_28;
    case LAYER_3D_USER_29:           return User_29;
    case LAYER_3D_USER_30:           return User_30;
    case LAYER_3D_USER_31:           return User_31;
    case LAYER_3D_USER_32:           return User_32;
    case LAYER_3D_USER_33:           return User_33;
    case LAYER_3D_USER_34:           return User_34;
    case LAYER_3D_USER_35:           return User_35;
    case LAYER_3D_USER_36:           return User_36;
    case LAYER_3D_USER_37:           return User_37;
    case LAYER_3D_USER_38:           return User_38;
    case LAYER_3D_USER_39:           return User_39;
    case LAYER_3D_USER_40:           return User_40;
    case LAYER_3D_USER_41:           return User_41;
    case LAYER_3D_USER_42:           return User_42;
    case LAYER_3D_USER_43:           return User_43;
    case LAYER_3D_USER_44:           return User_44;
    case LAYER_3D_USER_45:           return User_45;
    default:               return UNDEFINED_LAYER;
    }
}


int MapPCBLayerTo3DLayer( PCB_LAYER_ID aLayer )
{
    // NOTE: User_1..User45 are NOT consecutive numbers!

    switch( aLayer )
    {
    case F_Cu:      return LAYER_3D_COPPER_TOP;
    case B_Cu:      return LAYER_3D_COPPER_BOTTOM;
    case B_SilkS:   return LAYER_3D_SILKSCREEN_BOTTOM;
    case F_SilkS:   return LAYER_3D_SILKSCREEN_TOP;
    case B_Mask:    return LAYER_3D_SOLDERMASK_BOTTOM;
    case F_Mask:    return LAYER_3D_SOLDERMASK_TOP;
    case Cmts_User: return LAYER_3D_USER_COMMENTS;
    case Dwgs_User: return LAYER_3D_USER_DRAWINGS;
    case Eco1_User: return LAYER_3D_USER_ECO1;
    case Eco2_User: return LAYER_3D_USER_ECO2;
    case User_1:    return LAYER_3D_USER_1;
    case User_2:    return LAYER_3D_USER_2;
    case User_3:    return LAYER_3D_USER_3;
    case User_4:    return LAYER_3D_USER_4;
    case User_5:    return LAYER_3D_USER_5;
    case User_6:    return LAYER_3D_USER_6;
    case User_7:    return LAYER_3D_USER_7;
    case User_8:    return LAYER_3D_USER_8;
    case User_9:    return LAYER_3D_USER_9;
    case User_10:   return LAYER_3D_USER_10;
    case User_11:   return LAYER_3D_USER_11;
    case User_12:   return LAYER_3D_USER_12;
    case User_13:   return LAYER_3D_USER_13;
    case User_14:   return LAYER_3D_USER_14;
    case User_15:   return LAYER_3D_USER_15;
    case User_16:   return LAYER_3D_USER_16;
    case User_17:   return LAYER_3D_USER_17;
    case User_18:   return LAYER_3D_USER_18;
    case User_19:   return LAYER_3D_USER_19;
    case User_20:   return LAYER_3D_USER_20;
    case User_21:   return LAYER_3D_USER_21;
    case User_22:   return LAYER_3D_USER_22;
    case User_23:   return LAYER_3D_USER_23;
    case User_24:   return LAYER_3D_USER_24;
    case User_25:   return LAYER_3D_USER_25;
    case User_26:   return LAYER_3D_USER_26;
    case User_27:   return LAYER_3D_USER_27;
    case User_28:   return LAYER_3D_USER_28;
    case User_29:   return LAYER_3D_USER_29;
    case User_30:   return LAYER_3D_USER_30;
    case User_31:   return LAYER_3D_USER_31;
    case User_32:   return LAYER_3D_USER_32;
    case User_33:   return LAYER_3D_USER_33;
    case User_34:   return LAYER_3D_USER_34;
    case User_35:   return LAYER_3D_USER_35;
    case User_36:   return LAYER_3D_USER_36;
    case User_37:   return LAYER_3D_USER_37;
    case User_38:   return LAYER_3D_USER_38;
    case User_39:   return LAYER_3D_USER_39;
    case User_40:   return LAYER_3D_USER_40;
    case User_41:   return LAYER_3D_USER_41;
    case User_42:   return LAYER_3D_USER_42;
    case User_43:   return LAYER_3D_USER_43;
    case User_44:   return LAYER_3D_USER_44;
    case User_45:   return LAYER_3D_USER_45;
    default:        return UNDEFINED_LAYER;
    }
}
