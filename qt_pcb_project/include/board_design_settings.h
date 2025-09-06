#ifndef BOARD_DESIGN_SETTINGS_H_
#define BOARD_DESIGN_SETTINGS_H_

#include <memory>
#include <vector>
#include <map>
#include <set>

#include <netclass.h>
#include <project/net_settings.h>
#include <board_stackup_manager/board_stackup.h>
#include <drc/drc_engine.h>
#include <lset.h>
#include <settings/nested_settings.h>
#include <widgets/kiui_common.h>
#include <zone_settings.h>
#include <teardrop/teardrop_parameters.h>
#include <router/pns_meander.h>


#define DEFAULT_SILK_LINE_WIDTH       0.1
#define DEFAULT_COPPER_LINE_WIDTH     0.2
#define DEFAULT_EDGE_WIDTH            0.05
#define DEFAULT_COURTYARD_WIDTH       0.05
#define DEFAULT_LINE_WIDTH            0.10

#define DEFAULT_SILK_TEXT_SIZE        1.0
#define DEFAULT_COPPER_TEXT_SIZE      1.5
#define DEFAULT_TEXT_SIZE             1.0

#define DEFAULT_SILK_TEXT_WIDTH       0.1
#define DEFAULT_COPPER_TEXT_WIDTH     0.30
#define DEFAULT_TEXT_WIDTH            0.15

#define DEFAULT_DIMENSION_ARROW_LENGTH         50 // mils, for legacy purposes
#define DEFAULT_DIMENSION_EXTENSION_OFFSET     0.5

// Board thickness, mainly for 3D view:
#define DEFAULT_BOARD_THICKNESS_MM             1.6

#define DEFAULT_PCB_EDGE_THICKNESS             0.15

// soldermask to pad clearance. The default is 0 because usually board houses
// create a clearance depending on their fab process: mask material, color, price, etc.
#define DEFAULT_SOLDERMASK_EXPANSION           0.0

#define DEFAULT_SOLDERMASK_TO_COPPER_CLEARANCE 0.0

#define DEFAULT_SOLDERMASK_MIN_WIDTH           0.0

#define DEFAULT_SOLDERPASTE_CLEARANCE          0.0
#define DEFAULT_SOLDERPASTE_RATIO              0.0

#define DEFAULT_CUSTOMTRACKWIDTH      0.2
#define DEFAULT_CUSTOMDPAIRWIDTH      0.125
#define DEFAULT_CUSTOMDPAIRGAP        0.18
#define DEFAULT_CUSTOMDPAIRVIAGAP     0.18

#define DEFAULT_MEANDER_SPACING       0.6
#define DEFAULT_DP_MEANDER_SPACING    1.0

#define DEFAULT_MINCLEARANCE          0.0     // overall min clearance
#define DEFAULT_MINCONNECTION         0.0     // overall min connection width
#define DEFAULT_TRACKMINWIDTH         0.0     // track width min value
#define DEFAULT_VIASMINSIZE           0.5     // vias (not micro vias) min diameter
#define DEFAULT_MINTHROUGHDRILL       0.3     // through holes (not micro vias) min drill diameter
#define DEFAULT_MICROVIASMINSIZE      0.2     // micro vias (not vias) min diameter
#define DEFAULT_MICROVIASMINDRILL     0.1     // micro vias (not vias) min drill diameter
#define DEFAULT_HOLETOHOLEMIN         0.25    // minimum web thickness between two drilled holes
#define DEFAULT_HOLECLEARANCE         0.25    // copper-to-hole clearance (from IPC level A)

#define DEFAULT_COPPEREDGECLEARANCE   0.5     // clearance between copper items and edge cuts
#define LEGACY_COPPEREDGECLEARANCE   -0.01    // A flag to indicate the legacy method (based
                                              // on edge cut line thicknesses) should be used.
#define DEFAULT_SILKCLEARANCE         0.0
#define DEFAULT_MINGROOVEWIDTH        0.0

#define DEFAULT_MINRESOLVEDSPOKES     2       // Fewer resolved spokes indicates a starved thermal

#define MINIMUM_ERROR_SIZE_MM         0.001   // For arc approximation
#define MAXIMUM_ERROR_SIZE_MM         0.1     // For arc approximation

// Min/max values used in dialogs to validate settings
#define MINIMUM_LINE_WIDTH_MM         0.005   // minimal line width entered in a dialog
#define MAXIMUM_LINE_WIDTH_MM         100.0   // max line width entered in a dialog

// Default pad properties
#define DEFAULT_PAD_WIDTH_MM 2.54         // master pad width
#define DEFAULT_PAD_HEIGTH_MM 1.27        // master pad height
#define DEFAULT_PAD_DRILL_DIAMETER_MM 0.8 // master pad drill diameter for PTH
#define DEFAULT_PAD_RR_RADIUS_RATIO 0.15  // master pad corner radius ratio

struct VIA_DIMENSION
{
    int m_Diameter;     // <= 0 means use Netclass via diameter
    int m_Drill;        // <= 0 means use Netclass via drill

    VIA_DIMENSION()
    {
        m_Diameter = 0;
        m_Drill    = 0;
    }

    VIA_DIMENSION( int aDiameter, int aDrill )
    {
        m_Diameter = aDiameter;
        m_Drill    = aDrill;
    }

    bool operator==( const VIA_DIMENSION& aOther ) const
    {
        return ( m_Diameter == aOther.m_Diameter ) && ( m_Drill == aOther.m_Drill );
    }

    bool operator!=( const VIA_DIMENSION& aOther ) const { return !operator==( aOther ); }

    bool operator<( const VIA_DIMENSION& aOther ) const
    {
        if( m_Diameter != aOther.m_Diameter )
            return m_Diameter < aOther.m_Diameter;

        return m_Drill < aOther.m_Drill;
    }
};


struct DIFF_PAIR_DIMENSION
{
    int m_Width;         // <= 0 means use Netclass differential pair width
    int m_Gap;           // <= 0 means use Netclass differential pair gap
    int m_ViaGap;        // <= 0 means use Netclass differential pair via gap

    DIFF_PAIR_DIMENSION()
    {
        m_Width  = 0;
        m_Gap    = 0;
        m_ViaGap = 0;
    }

    DIFF_PAIR_DIMENSION( int aWidth, int aGap, int aViaGap )
    {
        m_Width  = aWidth;
        m_Gap    = aGap;
        m_ViaGap = aViaGap;
    }

    bool operator==( const DIFF_PAIR_DIMENSION& aOther ) const
    {
        return ( m_Width == aOther.m_Width )
                && ( m_Gap == aOther.m_Gap )
                && ( m_ViaGap == aOther.m_ViaGap );
    }

    bool operator!=( const DIFF_PAIR_DIMENSION& aOther ) const { return !operator==( aOther ); }

    bool operator<( const DIFF_PAIR_DIMENSION& aOther ) const
    {
        if( m_Width != aOther.m_Width )
            return m_Width < aOther.m_Width;

        if( m_Gap != aOther.m_Gap )
            return m_Gap < aOther.m_Gap;

        return m_ViaGap < aOther.m_ViaGap;
    }
};


enum
{
    LAYER_CLASS_SILK = 0,
    LAYER_CLASS_COPPER,
    LAYER_CLASS_EDGES,
    LAYER_CLASS_COURTYARD,
    LAYER_CLASS_FAB,
    LAYER_CLASS_OTHERS,

    LAYER_CLASS_COUNT
};


struct TEXT_ITEM_INFO
{
    QString      m_Text;
    bool         m_Visible;
    PCB_LAYER_ID m_Layer;

    TEXT_ITEM_INFO( const QString& aText, bool aVisible, PCB_LAYER_ID aLayer )
    {
        m_Text = aText;
        m_Visible = aVisible;
        m_Layer = aLayer;
    }

    bool operator==( const TEXT_ITEM_INFO& aOther ) const
    {
        return m_Text == aOther.m_Text
                && ( m_Visible == aOther.m_Visible )
                && ( m_Layer == aOther.m_Layer );
    }
};


// forward declaration from class_track.h
enum class VIATYPE : int;

// forward declarations from dimension.h
enum class DIM_UNITS_FORMAT : int;
enum class DIM_TEXT_POSITION : int;
enum class DIM_UNITS_MODE : int;
enum class DIM_PRECISION : int;

class PAD;

class BOARD_DESIGN_SETTINGS : public NESTED_SETTINGS
{
public:
    BOARD_DESIGN_SETTINGS( JSON_SETTINGS* aParent, const std::string& aPath );

    virtual ~BOARD_DESIGN_SETTINGS();

    bool operator==( const BOARD_DESIGN_SETTINGS& aOther ) const;
    bool operator!=( const BOARD_DESIGN_SETTINGS& aOther ) const
    {
        return !operator==( aOther );
    }

    BOARD_DESIGN_SETTINGS( const BOARD_DESIGN_SETTINGS& aOther);

    BOARD_DESIGN_SETTINGS& operator=( const BOARD_DESIGN_SETTINGS& aOther );

    bool LoadFromFile( const QString& aDirectory = "" ) override;

    BOARD_STACKUP& GetStackupDescriptor() { return m_stackup; }
    const BOARD_STACKUP& GetStackupDescriptor() const { return m_stackup; }

    TEARDROP_PARAMETERS_LIST* GetTeadropParamsList()
    {
        return &m_TeardropParamsList;
    }

    SEVERITY GetSeverity( int aDRCErrorCode );

    bool Ignore( int aDRCErrorCode );

    ZONE_SETTINGS& GetDefaultZoneSettings()
    {
        return m_defaultZoneSettings;
    }

    void SetDefaultZoneSettings( const ZONE_SETTINGS& aSettings )
    {
        m_defaultZoneSettings = aSettings;
    }

    inline const QString& GetCurrentNetClassName() const
    {
        return m_currentNetClassName;
    }

    inline bool UseNetClassTrack() const
    {
        return ( m_trackWidthIndex == 0 && !m_useCustomTrackVia );
    }

    inline bool UseNetClassVia() const
    {
        return ( m_viaSizeIndex == 0 && !m_useCustomTrackVia );
    }

    inline bool UseNetClassDiffPair() const
    {
        return ( m_diffPairIndex == 0 && !m_useCustomDiffPair );
    }

    int GetBiggestClearanceValue() const;

    int GetSmallestClearanceValue() const;

    inline unsigned GetTrackWidthIndex() const { return m_trackWidthIndex; }

    void SetTrackWidthIndex( unsigned aIndex );

    int GetCurrentTrackWidth() const;

    inline void SetCustomTrackWidth( int aWidth )
    {
        m_customTrackWidth = aWidth;
    }

    inline int GetCustomTrackWidth() const
    {
        return m_customTrackWidth;
    }

    inline unsigned GetViaSizeIndex() const
    {
        return m_viaSizeIndex;
    }

    void SetViaSizeIndex( unsigned aIndex );

    int GetCurrentViaSize() const;

    inline void SetCustomViaSize( int aSize )
    {
        m_customViaSize.m_Diameter = aSize;
    }

    inline int GetCustomViaSize() const
    {
        return m_customViaSize.m_Diameter;
    }

    int GetCurrentViaDrill() const;

    inline void SetCustomViaDrill( int aDrill )
    {
        m_customViaSize.m_Drill = aDrill;
    }

    inline int GetCustomViaDrill() const
    {
        return m_customViaSize.m_Drill;
    }

    inline void UseCustomTrackViaSize( bool aEnabled )
    {
        m_useCustomTrackVia = aEnabled;
    }

    inline bool UseCustomTrackViaSize() const
    {
        return m_useCustomTrackVia;
    }

    inline unsigned GetDiffPairIndex() const { return m_diffPairIndex; }

    void SetDiffPairIndex( unsigned aIndex );

    inline void SetCustomDiffPairWidth( int aWidth )
    {
        m_customDiffPair.m_Width = aWidth;
    }

    inline int GetCustomDiffPairWidth()
    {
        return m_customDiffPair.m_Width;
    }

    inline void SetCustomDiffPairGap( int aGap )
    {
        m_customDiffPair.m_Gap = aGap;
    }

    inline int GetCustomDiffPairGap()
    {
        return m_customDiffPair.m_Gap;
    }

    inline void SetCustomDiffPairViaGap( int aGap )
    {
        m_customDiffPair.m_ViaGap = aGap;
    }

    inline int GetCustomDiffPairViaGap()
    {
        return m_customDiffPair.m_ViaGap > 0 ? m_customDiffPair.m_ViaGap : m_customDiffPair.m_Gap;
    }

    inline void UseCustomDiffPairDimensions( bool aEnabled )
    {
        m_useCustomDiffPair = aEnabled;
    }

    inline bool UseCustomDiffPairDimensions() const
    {
        return m_useCustomDiffPair;
    }

    int GetCurrentDiffPairWidth() const;

    int GetCurrentDiffPairGap() const;

    int GetCurrentDiffPairViaGap() const;

    inline const LSET& GetEnabledLayers() const
    {
        return m_enabledLayers;
    }

    void SetEnabledLayers( LSET aMask );

    inline bool IsLayerEnabled( PCB_LAYER_ID aLayerId ) const
    {
        if( aLayerId >= 0 && aLayerId < PCB_LAYER_ID_COUNT )
            return m_enabledLayers[aLayerId];

        return false;
    }

    inline int GetCopperLayerCount() const
    {
        return m_copperLayerCount;
    }

    void SetCopperLayerCount( int aNewLayerCount );

    inline int GetUserDefinedLayerCount() const
    {
        return m_userDefinedLayerCount;
    }

    void SetUserDefinedLayerCount( int aNewLayerCount );

    inline int GetBoardThickness() const { return m_boardThickness; }
    inline void SetBoardThickness( int aThickness ) { m_boardThickness = aThickness; }

    int GetDRCEpsilon() const;

    int GetHolePlatingThickness() const;

    int GetLineThickness( PCB_LAYER_ID aLayer ) const;

    VECTOR2I GetTextSize( PCB_LAYER_ID aLayer ) const;

    int GetTextThickness( PCB_LAYER_ID aLayer ) const;

    bool GetTextItalic( PCB_LAYER_ID aLayer ) const;
    bool GetTextUpright( PCB_LAYER_ID aLayer ) const;

    int GetLayerClass( PCB_LAYER_ID aLayer ) const;

    void            SetAuxOrigin( const VECTOR2I& aOrigin ) { m_auxOrigin = aOrigin; }
    const VECTOR2I& GetAuxOrigin() const { return m_auxOrigin; }

    void            SetGridOrigin( const VECTOR2I& aOrigin ) { m_gridOrigin = aOrigin; }
    const VECTOR2I& GetGridOrigin() const { return m_gridOrigin; }

    void SetDefaultMasterPad();

private:
    void initFromOther( const BOARD_DESIGN_SETTINGS& aOther );

    bool migrateSchema0to1();

public:
    // Note: the first value in each dimensions list is the current netclass value
    std::vector<int>                     m_TrackWidthList;
    std::vector<VIA_DIMENSION>           m_ViasDimensionsList;
    std::vector<DIFF_PAIR_DIMENSION>     m_DiffPairDimensionsList;

    TEARDROP_PARAMETERS_LIST         m_TeardropParamsList;

    PNS::MEANDER_SETTINGS            m_SingleTrackMeanderSettings;
    PNS::MEANDER_SETTINGS            m_DiffPairMeanderSettings;
    PNS::MEANDER_SETTINGS            m_SkewMeanderSettings;

    VIATYPE    m_CurrentViaType;            ///< (VIA_BLIND_BURIED, VIA_THROUGH, VIA_MICROVIA)

    bool       m_UseConnectedTrackWidth;    // use width of existing track when creating a new,
                                            // connected track
    bool       m_TempOverrideTrackWidth;    // use selected track width temporarily even when
                                            // using connected track width
    int        m_MinClearance;              // overall min
    int        m_MinGrooveWidth;            // Minimum groove width for creepage checks
    int        m_MinConn;                   // overall min connection width
    int        m_TrackMinWidth;             // overall min track width
    int        m_ViasMinAnnularWidth;       // overall minimum width of the via copper ring
    int        m_ViasMinSize;               // overall vias (not micro vias) min diameter
    int        m_MinThroughDrill;           // through hole (not micro vias) min drill diameter
    int        m_MicroViasMinSize;          // micro vias min diameter
    int        m_MicroViasMinDrill;         // micro vias min drill diameter
    int        m_CopperEdgeClearance;
    int        m_HoleClearance;             // Hole to copper clearance
    int        m_HoleToHoleMin;             // Min width of web between two drilled holes
    int        m_SilkClearance;             // Min dist between two silk items
    int        m_MinResolvedSpokes;         // Min spoke count to not be a starved thermal
    int        m_MinSilkTextHeight;         // Min text height for silkscreen layers
    int        m_MinSilkTextThickness;      // Min text thickness for silkscreen layers

    std::shared_ptr<DRC_ENGINE>  m_DRCEngine;
    std::map<int, SEVERITY>         m_DRCSeverities;
    std::set<QString>            m_DrcExclusions;
    std::map<QString, QString>   m_DrcExclusionComments;

    // When smoothing the zone's outline there's the question of external fillets (that is, those
    // applied to concave corners).  While it seems safer to never have copper extend outside the
    // zone outline, 5.1.x and prior did indeed fill them so we leave the mode available.
    bool       m_ZoneKeepExternalFillets;

    // Maximum error allowed when approximating circles and arcs to segments
    int        m_MaxError;

    // Global mask margins:
    int        m_SolderMaskExpansion;         // Solder mask inflation around the pad or via
    int        m_SolderMaskMinWidth;          // Solder mask min width (2 areas closer than this
                                              //   width are merged)
    int        m_SolderMaskToCopperClearance; // Min distance allowed from copper to a mask
                                              //   aperture of another net

    int        m_SolderPasteMargin;           // Solder paste margin absolute value
    double     m_SolderPasteMarginRatio;      // Solder mask margin ratio value of pad size
                                              // The final margin is the sum of these 2 values
    bool       m_AllowSoldermaskBridgesInFPs;
    bool       m_TentViasFront;               // The default tenting option if not overridden on an
    bool       m_TentViasBack;                // individual via

    std::shared_ptr<NET_SETTINGS> m_NetSettings;

    std::vector<TEXT_ITEM_INFO>      m_DefaultFPTextItems;

    std::map<std::string, QString> m_UserLayerNames;

    // Arrays of default values for the various layer classes.
    int        m_LineThickness[ LAYER_CLASS_COUNT ];
    VECTOR2I   m_TextSize[LAYER_CLASS_COUNT];
    int        m_TextThickness[ LAYER_CLASS_COUNT ];
    bool       m_TextItalic[ LAYER_CLASS_COUNT ];
    bool       m_TextUpright[ LAYER_CLASS_COUNT ];

    // Default values for dimension objects
    DIM_UNITS_MODE    m_DimensionUnitsMode;
    DIM_PRECISION     m_DimensionPrecision; ///< Number of digits after the decimal
    DIM_UNITS_FORMAT  m_DimensionUnitsFormat;
    bool              m_DimensionSuppressZeroes;
    DIM_TEXT_POSITION m_DimensionTextPosition;
    bool              m_DimensionKeepTextAligned;
    int               m_DimensionArrowLength;
    int               m_DimensionExtensionOffset;

    bool              m_StyleFPFields;
    bool              m_StyleFPText;
    bool              m_StyleFPShapes;

    // Miscellaneous
    std::unique_ptr<PAD> m_Pad_Master; // A dummy pad to store all default parameters
                                       // when importing values or creating a new pad

    // Set to true if the board has a stackup management.
    // If not set a default basic stackup will be used to generate the gbrjob file.
    // Could be removed later, or at least always set to true
    bool       m_HasStackup;

    /// Enable inclusion of stackup height in track length measurements and length tuning
    bool       m_UseHeightForLengthCalcs;

private:
    VECTOR2I m_auxOrigin;  ///< origin for plot exports
    VECTOR2I m_gridOrigin; ///< origin for grid offsets

    // Indices into the trackWidth, viaSizes and diffPairDimensions lists.
    // The 0 index is always the current netclass value(s)
    unsigned   m_trackWidthIndex;
    unsigned   m_viaSizeIndex;
    unsigned   m_diffPairIndex;

    // Custom values for track/via sizes (specified via dialog instead of netclass or lists)
    bool       m_useCustomTrackVia;
    int        m_customTrackWidth;
    VIA_DIMENSION m_customViaSize;

    // Custom values for differential pairs (specified via dialog instead of netclass/lists)
    bool       m_useCustomDiffPair;
    DIFF_PAIR_DIMENSION m_customDiffPair;

    int        m_copperLayerCount; ///< Number of copper layers for this design

    int        m_userDefinedLayerCount; ///< Number of user defined layers for this design

    LSET       m_enabledLayers;    ///< Bit-mask for layer enabling

    int        m_boardThickness;   ///< Board thickness for 3D viewer

    QString    m_currentNetClassName;

    BOARD_STACKUP m_stackup;

    /// The default settings that will be used for new zones.
    ZONE_SETTINGS m_defaultZoneSettings;
};

#endif  // BOARD_DESIGN_SETTINGS_H_
