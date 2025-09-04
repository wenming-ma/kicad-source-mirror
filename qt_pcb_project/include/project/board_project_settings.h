
#ifndef KICAD_BOARD_PROJECT_SETTINGS_H
#define KICAD_BOARD_PROJECT_SETTINGS_H

#include <layer_ids.h>
#include <lset.h>
#include <settings/parameters.h>

// Can be removed by refactoring PARAM_LAYER_PRESET
#include <json_common.h>
#include <math/box2.h>
#include <glm/glm.hpp>



struct KICOMMON_API PCB_SELECTION_FILTER_OPTIONS
{
    bool lockedItems;   ///< Allow selecting locked items
    bool footprints;    ///< Allow selecting entire footprints
    bool text;          ///< Text (free or attached to a footprint)
    bool tracks;        ///< Copper tracks
    bool vias;          ///< Vias (all types>
    bool pads;          ///< Footprint pads
    bool graphics;      ///< Graphic lines, shapes, polygons
    bool zones;         ///< Copper zones
    bool keepouts;      ///< Keepout zones
    bool dimensions;    ///< Dimension items
    bool otherItems;    ///< Anything not fitting one of the above categories

    PCB_SELECTION_FILTER_OPTIONS()
    {
        lockedItems = true;
        footprints  = true;
        text        = true;
        tracks      = true;
        vias        = true;
        pads        = true;
        graphics    = true;
        zones       = true;
        keepouts    = true;
        dimensions  = true;
        otherItems  = true;
    }

    bool Any()
    {
        return ( footprints || text || tracks || vias || pads || graphics || zones
                 || keepouts || dimensions || otherItems );
    }

    bool All()
    {
        return ( footprints && text && tracks && vias && pads && graphics && zones
                 && keepouts && dimensions && otherItems );
    }
};

enum class HIGH_CONTRAST_MODE
{
    NORMAL = 0,     ///< Inactive layers are shown normally (no high-contrast mode)
    DIMMED,         ///< Inactive layers are dimmed (old high-contrast mode)
    HIDDEN          ///< Inactive layers are hidden
};

enum class ZONE_DISPLAY_MODE
{
    SHOW_FILLED,
    SHOW_ZONE_OUTLINE,

    // Debug modes

    SHOW_FRACTURE_BORDERS,
    SHOW_TRIANGULATION
};

enum class NET_COLOR_MODE
{
    OFF,        ///< Net (and netclass) colors are not shown
    RATSNEST,   ///< Net/netclass colors are shown on ratsnest lines only
    ALL         ///< Net/netclass colors are shown on all net copper
};

enum class RATSNEST_MODE
{
    ALL,        ///< Ratsnest lines are drawn to items on all layers (default)
    VISIBLE     ///< Ratsnest lines are drawn to items on visible layers only
};

struct KICOMMON_API IP2581_BOM
{
    QString mfg;       ///< Manufacturer name column
    QString MPN;     ///< Manufacturer part number column
    QString dist;      ///< Distributor name column
    QString distPN;    ///< Distributor part number column
    QString id;        ///< Internal ID column
};

struct KICOMMON_API LAYER_PRESET
{
    LAYER_PRESET( const QString& aName = QString() ) :
            name( aName ),
            layers( LSET::AllLayersMask() ),
            renderLayers( GAL_SET::DefaultVisible() ),
            flipBoard( false ),
            activeLayer( UNSELECTED_LAYER )
    {
        readOnly     = false;
    }

    LAYER_PRESET( const QString& aName, const LSET& aVisibleLayers, bool aFlipBoard ) :
            name( aName ),
            layers( aVisibleLayers ),
            renderLayers( GAL_SET::DefaultVisible() ),
            flipBoard( aFlipBoard ),
            activeLayer( UNSELECTED_LAYER )
    {
        readOnly     = false;
    }

    LAYER_PRESET( const QString& aName, const LSET& aVisibleLayers, const GAL_SET& aVisibleObjects,
                  PCB_LAYER_ID aActiveLayer, bool aFlipBoard ) :
            name( aName ),
            layers( aVisibleLayers ),
            renderLayers( aVisibleObjects ),
            flipBoard( aFlipBoard ),
            activeLayer( aActiveLayer )
    {
        readOnly  = false;
    }

    bool LayersMatch( const LAYER_PRESET& aOther )
    {
        return aOther.layers == layers && aOther.renderLayers == renderLayers;
    }

    QString     name;          ///< A name for this layer set
    LSET         layers;        ///< Board layers that are visible
    GAL_SET      renderLayers;  ///< Render layers (e.g. object types) that are visible
    bool         flipBoard;     ///< True if the flip board is enabled
    PCB_LAYER_ID activeLayer;   ///< Optional layer to set active when this preset is loaded
    bool         readOnly;      ///< True if this is a read-only (built-in) preset
};


class KICOMMON_API PARAM_LAYER_PRESET : public PARAM_LAMBDA<nlohmann::json>
{
public:
    PARAM_LAYER_PRESET( const std::string& aPath, QVector<LAYER_PRESET>* aPresetList );

    static void MigrateToV9Layers( nlohmann::json& aJson );

    static void MigrateToNamedRenderLayers( nlohmann::json& aJson );

private:
    nlohmann::json presetsToJson();

    void jsonToPresets( const nlohmann::json& aJson );

    QVector<LAYER_PRESET>* m_presets;
};


struct KICOMMON_API VIEWPORT
{
    VIEWPORT( const QString& aName = QString() ) :
            name( aName )
    { }

    VIEWPORT( const QString& aName, const BOX2D& aRect ) :
            name( aName ),
            rect( aRect )
    { }

    QString name;
    BOX2D    rect;
};


class KICOMMON_API PARAM_VIEWPORT : public PARAM_LAMBDA<nlohmann::json>
{
public:
    PARAM_VIEWPORT( const std::string& aPath, QVector<VIEWPORT>* aViewportList );

private:
    nlohmann::json viewportsToJson();

    void jsonToViewports( const nlohmann::json& aJson );

    QVector<VIEWPORT>* m_viewports;
};


struct KICOMMON_API VIEWPORT3D
{
    VIEWPORT3D( const QString& aName = QString() ) :
            name( aName )
    { }

    VIEWPORT3D( const QString& aName, glm::mat4 aViewMatrix ) :
            name( aName ),
            matrix( std::move( aViewMatrix ) )
    { }

    QString  name;
    glm::mat4 matrix;
};


class KICOMMON_API PARAM_VIEWPORT3D : public PARAM_LAMBDA<nlohmann::json>
{
public:
    PARAM_VIEWPORT3D( const std::string& aPath, QVector<VIEWPORT3D>* aViewportList );

private:
    nlohmann::json viewportsToJson();

    void jsonToViewports( const nlohmann::json & aJson );

    QVector<VIEWPORT3D>* m_viewports;
};


class KICOMMON_API LAYER_PAIR
{
public:
    LAYER_PAIR() :
            m_layerA( UNDEFINED_LAYER ), m_layerB( UNDEFINED_LAYER )
    {
    }

    LAYER_PAIR( PCB_LAYER_ID a, PCB_LAYER_ID b ) :
            m_layerA( a ), m_layerB( b )
    {
    }

    PCB_LAYER_ID GetLayerA() const { return m_layerA; }
    PCB_LAYER_ID GetLayerB() const { return m_layerB; }

    void SetLayerA( PCB_LAYER_ID aLayer ) { m_layerA = aLayer; }
    void SetLayerB( PCB_LAYER_ID aLayer ) { m_layerB = aLayer; }

    /**
     * @return true if the two layer pairs have the same layers, regardless of order
     */
    bool HasSameLayers( const LAYER_PAIR& aOther ) const
    {
        return ( m_layerA == aOther.m_layerA && m_layerB == aOther.m_layerB )
               || ( m_layerA == aOther.m_layerB && m_layerB == aOther.m_layerA );
    }

private:
    PCB_LAYER_ID m_layerA;
    PCB_LAYER_ID m_layerB;
};


/**
 * All information about a layer pair as stored in the layer pair store.
 */
class KICOMMON_API LAYER_PAIR_INFO
{
public:
    LAYER_PAIR_INFO( LAYER_PAIR aPair, bool aEnabled, std::optional<QString> aName ) :
            m_pair( std::move( aPair ) ), m_enabled( aEnabled), m_name( std::move( aName ) )
    {
    }

    const LAYER_PAIR& GetLayerPair() const { return m_pair; }

    const std::optional<QString>& GetName() const { return m_name; }

    void SetName( const QString& aNewName ) { m_name = aNewName; }
    void UnsetName() { m_name = std::nullopt; }

    bool IsEnabled() const { return m_enabled; }
    void SetEnabled( bool aNewEnabled ) { m_enabled = aNewEnabled; }

private:
    LAYER_PAIR              m_pair;
    bool                    m_enabled = true;
    std::optional<QString> m_name;
};


class KICOMMON_API PARAM_LAYER_PAIRS : public PARAM_LAMBDA<nlohmann::json>
{
public:
    PARAM_LAYER_PAIRS( const std::string& aPath, QVector<LAYER_PAIR_INFO>& m_layerPairInfos );

private:
    nlohmann::json layerPairsToJson();

    void jsonToLayerPairs( const nlohmann::json& aJson );

    QVector<LAYER_PAIR_INFO>& m_layerPairInfos;
};


struct KICOMMON_API PANEL_NET_INSPECTOR_SETTINGS
{
    QString              filter_text;
    bool                  filter_by_net_name;
    bool                  filter_by_netclass;
    bool                  group_by_netclass;
    bool                  group_by_constraint;
    QVector<QString> custom_group_rules;
    bool                  show_zero_pad_nets;
    bool                  show_unconnected_nets;
    int                   sorting_column;
    bool                  sort_order_asc;
    QVector<int>      col_order;
    QVector<int>      col_widths;
    QVector<bool>     col_hidden;

    QVector<QString> expanded_rows;

    PANEL_NET_INSPECTOR_SETTINGS()
    {
        filter_text = "";
        filter_by_net_name = true;
        filter_by_netclass = true;
        group_by_netclass = false;
        group_by_constraint = false;
        show_zero_pad_nets = false;
        show_unconnected_nets = false;
        sorting_column = -1;
        sort_order_asc = true;
    }
};


#endif // KICAD_BOARD_PROJECT_SETTINGS_H
