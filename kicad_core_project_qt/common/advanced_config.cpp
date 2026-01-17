
#include <advanced_config.h>
#include <trace_helpers.h>
#include <config_params.h>
#include <paths.h>

#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QSettings>
#include <QCoreApplication>
#include <QDebug>
#include <QtCore/Qt>

static const QString AdvancedConfigMask = "KICAD_ADVANCED_CONFIG";

namespace AC_STACK
{
    static constexpr int min_stack = 32 * 4096;
    static constexpr int default_stack = 256 * 4096;
    static constexpr int max_stack = 4096 * 4096;
}

namespace AC_KEYS
{

static const QString IncrementalConnectivity = "IncrementalConnectivity";
static const QString Use3DConnexionDriver = "3DConnexionDriver";
static const QString ExtraFillMargin = "ExtraFillMargin";
static const QString EnableCreepageSlot = "EnableCreepageSlot";
static const QString DRCEpsilon = "DRCEpsilon";
static const QString DRCSliverWidthTolerance = "DRCSliverWidthTolerance";
static const QString DRCSliverMinimumLength = "DRCSliverMinimumLength";
static const QString DRCSliverAngleTolerance = "DRCSliverAngleTolerance";
static const QString HoleWallThickness = "HoleWallPlatingThickness";
static const QString CoroutineStackSize = "CoroutineStackSize";
static const QString ShowRouterDebugGraphics = "ShowRouterDebugGraphics";
static const QString EnableRouterDump = "EnableRouterDump";
static const QString HyperZoom = "HyperZoom";
static const QString CompactFileSave = "CompactSave";
static const QString DrawArcAccuracy = "DrawArcAccuracy";
static const QString DrawArcCenterStartEndMaxAngle = "DrawArcCenterStartEndMaxAngle";
static const QString MaxTangentTrackAngleDeviation = "MaxTangentTrackAngleDeviation";
static const QString MaxTrackLengthToKeep = "MaxTrackLengthToKeep";
static const QString StrokeTriangulation = "StrokeTriangulation";
static const QString ExtraZoneDisplayModes = "ExtraZoneDisplayModes";
static const QString MinPlotPenWidth = "MinPlotPenWidth";
static const QString DebugZoneFiller = "DebugZoneFiller";
static const QString DebugPDFWriter = "DebugPDFWriter";
static const QString SmallDrillMarkSize = "SmallDrillMarkSize";
static const QString HotkeysDumper = "HotkeysDumper";
static const QString DrawBoundingBoxes = "DrawBoundingBoxes";
static const QString ShowPcbnewExportNetlist = "ShowPcbnewExportNetlist";
static const QString Skip3DModelFileCache = "Skip3DModelFileCache";
static const QString Skip3DModelMemoryCache = "Skip3DModelMemoryCache";
static const QString HideVersionFromTitle = "HideVersionFromTitle";
static const QString TraceMasks = "TraceMasks";
static const QString ShowRepairSchematic = "ShowRepairSchematic";
static const QString ShowEventCounters = "ShowEventCounters";
static const QString AllowManualCanvasScale = "AllowManualCanvasScale";
static const QString UpdateUIEventInterval = "UpdateUIEventInterval";
static const QString V3DRT_BevelHeight_um = "V3DRT_BevelHeight_um";
static const QString V3DRT_BevelExtentFactor = "V3DRT_BevelExtentFactor";
static const QString EnableDesignBlocks = "EnableDesignBlocks";
static const QString EnableGenerators = "EnableGenerators";
static const QString EnableLibWithText = "EnableLibWithText";
static const QString EnableLibDir = "EnableLibDir";
static const QString EnableEeschemaPrintCairo = "EnableEeschemaPrintCairo";
static const QString EnableEeschemaExportClipboardCairo = "EnableEeschemaExportClipboardCairo";
static const QString DisambiguationTime = "DisambiguationTime";
static const QString PcbSelectionVisibilityRatio = "PcbSelectionVisibilityRatio";
static const QString FontErrorSize = "FontErrorSize";
static const QString OcePluginLinearDeflection = "OcePluginLinearDeflection";
static const QString OcePluginAngularDeflection = "OcePluginAngularDeflection";
static const QString TriangulateSimplificationLevel = "TriangulateSimplificationLevel";
static const QString TriangulateMinimumArea = "TriangulateMinimumArea";
static const QString EnableCacheFriendlyFracture = "EnableCacheFriendlyFracture";
static const QString EnableAPILogging = "EnableAPILogging";
static const QString MaxFileSystemWatchers = "MaxFileSystemWatchers";
static const QString MinorSchematicGraphSize = "MinorSchematicGraphSize";
static const QString ResolveTextRecursionDepth = "ResolveTextRecursionDepth";
static const QString EnableExtensionSnaps = "EnableExtensionSnaps";
static const QString ExtensionSnapTimeoutMs = "ExtensionSnapTimeoutMs";
static const QString ExtensionSnapActivateOnHover = "ExtensionSnapActivateOnHover";
static const QString EnableSnapAnchorsDebug = "EnableSnapAnchorsDebug";
static const QString MinParallelAngle = "MinParallelAngle";
static const QString HoleWallPaintingMultiplier = "HoleWallPaintingMultiplier";
static const QString MsgPanelShowUuids = "MsgPanelShowUuids";
static const QString MaximumThreads = "MaximumThreads";
static const QString NetInspectorBulkUpdateOptimisationThreshold =
        "NetInspectorBulkUpdateOptimisationThreshold";
static const QString ExcludeFromSimulationLineWidth = "ExcludeFromSimulationLineWidth";
static const QString GitIconRefreshInterval = "GitIconRefreshInterval";
static const QString MaxPastedTextLength = "MaxPastedTextLength";
static const QString PNSProcessClusterTimeout = "PNSProcessClusterTimeout";

} // namespace KEYS

namespace AC_GROUPS
{
static const QString V3D_RayTracing = "G_3DV_RayTracing";
}
QString dumpParamCfg( const PARAM_CFG& aParam )
{
    QString s = aParam.m_Ident + ": ";

    switch( aParam.m_Type )
    {
    case paramcfg_id::PARAM_INT:
    case paramcfg_id::PARAM_INT_WITH_SCALE:
        s += QString::number(*static_cast<const PARAM_CFG_INT&>( aParam ).m_Pt_param);
        break;
    case paramcfg_id::PARAM_DOUBLE:
        s += QString::number(*static_cast<const PARAM_CFG_DOUBLE&>( aParam ).m_Pt_param);
        break;
    case paramcfg_id::PARAM_WXSTRING:
        s += *static_cast<const PARAM_CFG_QSTRING&>( aParam ).m_Pt_param;
        break;
    case paramcfg_id::PARAM_FILENAME:
        s += *static_cast<const PARAM_CFG_FILENAME&>( aParam ).m_Pt_param;
        break;
    case paramcfg_id::PARAM_BOOL:
        s += ( *static_cast<const PARAM_CFG_BOOL&>( aParam ).m_Pt_param ? "true" : "false" );
        break;
    default: s += "Unsupported PARAM_CFG variant: " + QString::number(aParam.m_Type);
    }

    return s;
}

static void dumpCfg( const std::vector<PARAM_CFG*>& aArray )
{
    for( const PARAM_CFG* param : aArray )
    {
        // Debug output handled by Qt logging
    }
}


static QFileInfo getAdvancedCfgFilename()
{
    const static QString cfg_filename{ "kicad_advanced" };
    return QFileInfo( PATHS::GetUserSettingsPath() + "/" + cfg_filename );
}


ADVANCED_CFG::ADVANCED_CFG()
{
    // Initialize advanced configuration parameters

    // Init defaults - this is done in case the config doesn't exist,
    // then the values will remain as set here.
    m_CoroutineStackSize        = AC_STACK::default_stack;
    m_ShowRouterDebugGraphics   = false;
    m_EnableRouterDump          = false;
    m_HyperZoom                 = false;
    m_DrawArcAccuracy           = 10.0;
    m_DrawArcCenterMaxAngle     = 50.0;
    m_MaxTangentAngleDeviation  = 1.0;
    m_MaxTrackLengthToKeep      = 0.0005;
    m_ExtraZoneDisplayModes     = false;
    m_DrawTriangulationOutlines = false;

    m_ExtraClearance            = 0.0005;
    m_EnableCreepageSlot        = false;
    m_DRCEpsilon                = 0.0005;   // 0.5um is small enough not to materially violate
                                            // any constraints.
    m_SliverWidthTolerance      = 0.08;
    m_SliverMinimumLength       = 0.0008;
    m_SliverAngleTolerance      = 20.0;

    m_HoleWallThickness         = 0.020;    // IPC-6012 says 15-18um; Cadence says at least
                                            // 0.020 for a Class 2 board and at least 0.025
                                            // for Class 3.

    m_MinPlotPenWidth           = 0.0212;   // 1 pixel at 1200dpi.

    m_DebugZoneFiller           = false;
    m_DebugPDFWriter            = false;
    m_SmallDrillMarkSize        = 0.35;
    m_HotkeysDumper             = false;
    m_DrawBoundingBoxes         = false;
    m_MsgPanelShowUuids         = 0;
    m_ShowPcbnewExportNetlist   = false;
    m_Skip3DModelFileCache      = false;
    m_Skip3DModelMemoryCache    = false;
    m_HideVersionFromTitle      = false;
    m_ShowEventCounters         = false;
    m_AllowManualCanvasScale    = false;
    m_CompactSave               = false;
    m_UpdateUIEventInterval     = 0;
    m_ShowRepairSchematic       = false;
    m_EnableDesignBlocks        = true;
    m_EnableGenerators          = false;
    m_EnableLibWithText         = false;
    m_EnableLibDir              = false;

    m_EnableEeschemaPrintCairo  = true;
    m_EnableEeschemaExportClipboardCairo = true;

    m_3DRT_BevelHeight_um       = 30;
    m_3DRT_BevelExtentFactor    = 1.0 / 16.0;

    m_EnableAPILogging          = false;

    m_Use3DConnexionDriver      = true;

    m_IncrementalConnectivity   = true;

    m_DisambiguationMenuDelay   = 500;

    m_PcbSelectionVisibilityRatio = 1.0;

    m_FontErrorSize             = 2;

    m_OcePluginLinearDeflection = 0.14;
    m_OcePluginAngularDeflection = 30;

    m_TriangulateSimplificationLevel = 50;
    m_TriangulateMinimumArea = 1000;

    m_EnableCacheFriendlyFracture = true;

    m_MaxFilesystemWatchers = 16384;

    m_MinorSchematicGraphSize = 10000;

    m_ResolveTextRecursionDepth = 3;

    m_EnableExtensionSnaps = true;
    m_ExtensionSnapTimeoutMs = 500;
    m_ExtensionSnapActivateOnHover = true;
    m_EnableSnapAnchorsDebug = false;

    m_MinParallelAngle = 0.001;
    m_HoleWallPaintingMultiplier = 1.5;

    m_MaximumThreads = 0;

    m_MinimumMarkerSeparationDistance = 0.15;

    m_NetInspectorBulkUpdateOptimisationThreshold = 25;

    m_ExcludeFromSimulationLineWidth = 25;

    m_GitIconRefreshInterval = 10000;

    m_MaxPastedTextLength = 100;

    m_PNSProcessClusterTimeout = 100; // Default: 100 ms

    loadFromConfigFile();
}


const ADVANCED_CFG& ADVANCED_CFG::GetCfg()
{
    static ADVANCED_CFG instance;
    return instance;
}


void ADVANCED_CFG::loadFromConfigFile()
{
    const QFileInfo k_advanced = getAdvancedCfgFilename();

    // If we are running headless, use the class defaults because we cannot instantiate QSettings
    if( !QCoreApplication::instance() )
        return;

    if( !k_advanced.exists() )
    {
        // Configuration file does not exist, using defaults

        // load the defaults
        QSettings emptyConfig;
        loadSettings( emptyConfig );

        return;
    }

    // Loading advanced configuration from file

    QSettings file_cfg( k_advanced.absoluteFilePath(), QSettings::IniFormat );
    loadSettings( file_cfg );
}


void ADVANCED_CFG::loadSettings( QSettings& aCfg )
{
    std::vector<PARAM_CFG*> configParams;

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::ExtraFillMargin,
                                                  &m_ExtraClearance,
                                                  m_ExtraClearance, 0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableCreepageSlot,
                                                &m_EnableCreepageSlot, m_EnableCreepageSlot ) );


    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DRCEpsilon,
                                                  &m_DRCEpsilon, m_DRCEpsilon, 0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DRCSliverWidthTolerance,
                                                  &m_SliverWidthTolerance, m_SliverWidthTolerance,
                                                  0.01, 0.25 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DRCSliverMinimumLength,
                                                  &m_SliverMinimumLength, m_SliverMinimumLength,
                                                  1e-9, 10 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DRCSliverAngleTolerance,
                                                  &m_SliverAngleTolerance, m_SliverAngleTolerance,
                                                  1.0, 90.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::HoleWallThickness,
                                                  &m_HoleWallThickness, m_HoleWallThickness,
                                                  0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::CoroutineStackSize,
                                               &m_CoroutineStackSize, AC_STACK::default_stack,
                                               AC_STACK::min_stack, AC_STACK::max_stack ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::UpdateUIEventInterval,
                                               &m_UpdateUIEventInterval, m_UpdateUIEventInterval,
                                               -1, 100000 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ShowRouterDebugGraphics,
                                                &m_ShowRouterDebugGraphics,
                                                m_ShowRouterDebugGraphics ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableRouterDump,
                                                &m_EnableRouterDump, m_EnableRouterDump ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::HyperZoom,
                                                &m_HyperZoom, m_HyperZoom ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::CompactFileSave,
                                                &m_CompactSave, m_CompactSave ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DrawArcAccuracy,
                                                  &m_DrawArcAccuracy, m_DrawArcAccuracy,
                                                  0.0, 100000.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::DrawArcCenterStartEndMaxAngle,
                                                  &m_DrawArcCenterMaxAngle,
                                                  m_DrawArcCenterMaxAngle, 0.0, 100000.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::MaxTangentTrackAngleDeviation,
                                                  &m_MaxTangentAngleDeviation,
                                                  m_MaxTangentAngleDeviation, 0.0, 90.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::MaxTrackLengthToKeep,
                                                  &m_MaxTrackLengthToKeep, m_MaxTrackLengthToKeep,
                                                  0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ExtraZoneDisplayModes,
                                                &m_ExtraZoneDisplayModes,
                                                m_ExtraZoneDisplayModes ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::StrokeTriangulation,
                                                &m_DrawTriangulationOutlines,
                                                m_DrawTriangulationOutlines ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::MinPlotPenWidth,
                                                  &m_MinPlotPenWidth, m_MinPlotPenWidth,
                                                  0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::DebugZoneFiller,
                                                &m_DebugZoneFiller, m_DebugZoneFiller ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::DebugPDFWriter,
                                                &m_DebugPDFWriter, m_DebugPDFWriter ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::SmallDrillMarkSize,
                                                  &m_SmallDrillMarkSize, m_SmallDrillMarkSize,
                                                  0.0, 3.0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::HotkeysDumper,
                                                &m_HotkeysDumper, m_HotkeysDumper ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::DrawBoundingBoxes,
                                                &m_DrawBoundingBoxes, m_DrawBoundingBoxes ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ShowPcbnewExportNetlist,
                                                &m_ShowPcbnewExportNetlist,
                                                m_ShowPcbnewExportNetlist ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::Skip3DModelFileCache,
                                                &m_Skip3DModelFileCache, m_Skip3DModelFileCache ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::Skip3DModelMemoryCache,
                                                &m_Skip3DModelMemoryCache,
                                                m_Skip3DModelMemoryCache ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::HideVersionFromTitle,
                                                &m_HideVersionFromTitle, m_HideVersionFromTitle ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ShowRepairSchematic,
                                                &m_ShowRepairSchematic, m_ShowRepairSchematic ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ShowEventCounters,
                                                &m_ShowEventCounters, m_ShowEventCounters ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::AllowManualCanvasScale,
                                                &m_AllowManualCanvasScale,
                                                m_AllowManualCanvasScale ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::V3DRT_BevelHeight_um,
                                               &m_3DRT_BevelHeight_um, m_3DRT_BevelHeight_um,
                                               0, std::numeric_limits<int>::max() ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::V3DRT_BevelExtentFactor,
                                                  &m_3DRT_BevelExtentFactor,
                                                  m_3DRT_BevelExtentFactor, 0.0, 100.0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::Use3DConnexionDriver,
                                                &m_Use3DConnexionDriver, m_Use3DConnexionDriver ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::IncrementalConnectivity,
                                                &m_IncrementalConnectivity,
                                                m_IncrementalConnectivity ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::DisambiguationTime,
                                               &m_DisambiguationMenuDelay,
                                               m_DisambiguationMenuDelay,
                                               50, 10000 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableDesignBlocks,
                                                &m_EnableDesignBlocks, m_EnableDesignBlocks ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableGenerators,
                                                &m_EnableGenerators, m_EnableGenerators ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableAPILogging,
                                                &m_EnableAPILogging, m_EnableAPILogging ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableLibWithText,
                                                &m_EnableLibWithText, m_EnableLibWithText ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableLibDir,
                                                &m_EnableLibDir, m_EnableLibDir ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableEeschemaPrintCairo,
                                                &m_EnableEeschemaPrintCairo,
                                                m_EnableEeschemaPrintCairo ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableEeschemaExportClipboardCairo,
                                                &m_EnableEeschemaExportClipboardCairo,
                                                m_EnableEeschemaExportClipboardCairo ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::PcbSelectionVisibilityRatio,
                                                  &m_PcbSelectionVisibilityRatio,
                                                  m_PcbSelectionVisibilityRatio, 0.0, 1.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::FontErrorSize,
                                                  &m_FontErrorSize,
                                                  m_FontErrorSize, 0.01, 100 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::OcePluginLinearDeflection,
                                                    &m_OcePluginLinearDeflection,
                                                    m_OcePluginLinearDeflection, 0.01, 1.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::OcePluginAngularDeflection,
                                                    &m_OcePluginAngularDeflection,
                                                    m_OcePluginAngularDeflection, 0.01, 360.0 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::TriangulateSimplificationLevel,
                                                    &m_TriangulateSimplificationLevel,
                                                    m_TriangulateSimplificationLevel, 0, 1000 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::TriangulateMinimumArea,
                                                    &m_TriangulateMinimumArea,
                                                    m_TriangulateMinimumArea, 0, 100000 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableCacheFriendlyFracture,
                                                &m_EnableCacheFriendlyFracture,
                                                m_EnableCacheFriendlyFracture ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::MaxFileSystemWatchers,
                                                  &m_MaxFilesystemWatchers, m_MaxFilesystemWatchers,
                                                  0, 2147483647 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::MinorSchematicGraphSize,
                                               &m_MinorSchematicGraphSize,
                                               m_MinorSchematicGraphSize,
                                               0, 2147483647 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::ResolveTextRecursionDepth,
                                               &m_ResolveTextRecursionDepth,
                                               m_ResolveTextRecursionDepth, 0, 10 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableExtensionSnaps,
                                                &m_EnableExtensionSnaps,
                                                m_EnableExtensionSnaps ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::ExtensionSnapTimeoutMs,
                                               &m_ExtensionSnapTimeoutMs,
                                               m_ExtensionSnapTimeoutMs, 0 ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::ExtensionSnapActivateOnHover,
                                                &m_ExtensionSnapActivateOnHover,
                                                m_ExtensionSnapActivateOnHover ) );

    configParams.push_back( new PARAM_CFG_BOOL( true, AC_KEYS::EnableSnapAnchorsDebug,
                                                &m_EnableSnapAnchorsDebug,
                                                m_EnableSnapAnchorsDebug ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::MinParallelAngle,
                                                  &m_MinParallelAngle, m_MinParallelAngle,
                                                  0.0, 45.0 ) );

    configParams.push_back( new PARAM_CFG_DOUBLE( true, AC_KEYS::HoleWallPaintingMultiplier,
                                                  &m_HoleWallPaintingMultiplier,
                                                  m_HoleWallPaintingMultiplier,
                                                  0.1, 100.0 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::MsgPanelShowUuids,
                                               &m_MsgPanelShowUuids,
                                               m_MsgPanelShowUuids ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::MaximumThreads,
                                                  &m_MaximumThreads, m_MaximumThreads,
                                                  0, 500 ) );

    configParams.push_back(
            new PARAM_CFG_INT( true, AC_KEYS::NetInspectorBulkUpdateOptimisationThreshold,
                               &m_NetInspectorBulkUpdateOptimisationThreshold,
                               m_NetInspectorBulkUpdateOptimisationThreshold, 0, 1000 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::ExcludeFromSimulationLineWidth,
                                               &m_ExcludeFromSimulationLineWidth,
                                               m_ExcludeFromSimulationLineWidth, 1, 100 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::GitIconRefreshInterval,
                                               &m_GitIconRefreshInterval,
                                               m_GitIconRefreshInterval, 0, 100000 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::MaxPastedTextLength,
                                                  &m_MaxPastedTextLength,
                                                  m_MaxPastedTextLength, 0, 100000 ) );

    configParams.push_back( new PARAM_CFG_INT( true, AC_KEYS::PNSProcessClusterTimeout,
                                               &m_PNSProcessClusterTimeout, 100, 10, 10000 ) );

    QString traceMasks;
    configParams.push_back( new PARAM_CFG_QSTRING( true, AC_KEYS::TraceMasks, &traceMasks,
                                                   QString("") ) );

    // Load configuration parameters from QSettings
    for( PARAM_CFG* param : configParams )
    {
        if( param && param->m_Setup )
        {
            param->ReadParam( &aCfg );
        }
    }

    // Set trace masks using Qt string processing
    QStringList traceMaskList = traceMasks.split(',', Qt::SkipEmptyParts);

    for( const QString& mask : traceMaskList )
    {
        // Configure Qt logging categories based on trace masks
        if( !mask.trimmed().isEmpty() )
        {
            // Trace mask configuration would be handled here
        }
    }

    dumpCfg( configParams );

    for( PARAM_CFG* param : configParams )
        delete param;

    // Coroutine stack size configured
}


