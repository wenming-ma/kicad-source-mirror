#include <project.h>
#include <project/net_settings.h>
#include <settings/json_settings_internals.h>
#include <project/project_file.h>
#include <settings/common_settings.h>
#include <settings/parameters.h>
#include <wildcards_and_files_ext.h>
#include <vector>
#include <QtCore/QSettings>
#include <QtCore/QDebug>


const int projectFileSchemaVersion = 3;


PROJECT_FILE::PROJECT_FILE( const QString& aFullPath ) :
        JSON_SETTINGS( aFullPath, SETTINGS_LOC::PROJECT, projectFileSchemaVersion ),
        m_ErcSettings( nullptr ),
        m_SchematicSettings( nullptr ),
        m_BoardSettings(),
        m_sheets(),
        m_boards(),
        m_project( nullptr ),
        m_wasMigrated( false )
{
    m_deleteLegacyAfterMigration = false;

    m_params.emplace_back( new PARAM_LIST<FILE_INFO_PAIR>( "sheets", &m_sheets, {} ) );

    m_params.emplace_back( new PARAM_LIST<FILE_INFO_PAIR>( "boards", &m_boards, {} ) );

    m_params.emplace_back( new PARAM_STRING_MAP( "text_variables",
            &m_TextVars, {}, false, true ) );

    m_params.emplace_back( new PARAM_LIST<std::string>( "libraries.pinned_symbol_libs",
            &m_PinnedSymbolLibs, {} ) );

    m_params.emplace_back( new PARAM_LIST<std::string>( "libraries.pinned_footprint_libs",
            &m_PinnedFootprintLibs, {} ) );

    m_params.emplace_back( new PARAM_PATH_LIST( "cvpcb.equivalence_files",
            &m_EquivalenceFiles, {} ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.page_layout_descr_file",
            &m_BoardDrawingSheetFile, "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.netlist",
            &m_PcbLastPath[LAST_PATH_NETLIST], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.step",
            &m_PcbLastPath[LAST_PATH_STEP], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.idf",
            &m_PcbLastPath[LAST_PATH_IDF], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.vrml",
            &m_PcbLastPath[LAST_PATH_VRML], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.specctra_dsn",
            &m_PcbLastPath[LAST_PATH_SPECCTRADSN], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.gencad",
            &m_PcbLastPath[LAST_PATH_GENCAD], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.pos_files",
            &m_PcbLastPath[LAST_PATH_POS_FILES], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.svg",
            &m_PcbLastPath[LAST_PATH_SVG], "" ) );

    m_params.emplace_back( new PARAM_PATH( "pcbnew.last_paths.plot",
            &m_PcbLastPath[LAST_PATH_PLOT], "" ) );

    m_params.emplace_back( new PARAM<std::string>( "schematic.legacy_lib_dir",
            &m_LegacyLibDir, "" ) );

    m_params.emplace_back( new PARAM_LAMBDA<nlohmann::json>( "schematic.legacy_lib_list",
            [&]() -> nlohmann::json
            {
                nlohmann::json ret = nlohmann::json::array();

                for( const std::string& libName : m_LegacyLibNames )
                    ret.push_back( libName );

                return ret;
            },
            [&]( const nlohmann::json& aJson )
            {
                if( aJson.empty() || !aJson.is_array() )
                    return;

                m_LegacyLibNames.clear();

                for( const nlohmann::json& entry : aJson )
                    m_LegacyLibNames.push_back( entry.get<std::string>() );
            }, {} ) );

    m_NetSettings = std::make_shared<NET_SETTINGS>( this, "net_settings" );

    m_params.emplace_back( new PARAM_LAYER_PRESET( "board.layer_presets", &m_LayerPresets ) );

    m_params.emplace_back( new PARAM_VIEWPORT( "board.viewports", &m_Viewports ) );

    m_params.emplace_back( new PARAM_VIEWPORT3D( "board.3dviewports", &m_Viewports3D ) );

    m_params.emplace_back( new PARAM_LAYER_PAIRS( "board.layer_pairs", m_LayerPairInfos ) );

    m_params.emplace_back( new PARAM<std::string>( "board.ipc2581.internal_id",
            &m_IP2581Bom.id, std::string() ) );

    m_params.emplace_back( new PARAM<std::string>( "board.ipc2581.mpn",
            &m_IP2581Bom.MPN, std::string() ) );

    m_params.emplace_back( new PARAM<std::string>( "board.ipc2581.mfg",
            &m_IP2581Bom.mfg, std::string() ) );

    m_params.emplace_back( new PARAM<std::string>( "board.ipc2581.distpn",
            &m_IP2581Bom.distPN, std::string() ) );

    m_params.emplace_back( new PARAM<std::string>( "board.ipc2581.dist",
            &m_IP2581Bom.dist, std::string() ) );

    registerMigration( 1, 2, std::bind( &PROJECT_FILE::migrateSchema1To2, this ) );
    registerMigration( 2, 3, std::bind( &PROJECT_FILE::migrateSchema2To3, this ) );
}


bool PROJECT_FILE::migrateSchema1To2()
{
    auto p( "/board/layer_presets"_json_pointer );

    if( !m_internals->contains( p ) || !m_internals->at( p ).is_array() )
        return true;

    nlohmann::json& presets = m_internals->at( p );

    for( nlohmann::json& entry : presets )
        PARAM_LAYER_PRESET::MigrateToV9Layers( entry );

    m_wasMigrated = true;

    return true;
}


bool PROJECT_FILE::migrateSchema2To3()
{
    auto p( "/board/layer_presets"_json_pointer );

    if( !m_internals->contains( p ) || !m_internals->at( p ).is_array() )
        return true;

    nlohmann::json& presets = m_internals->at( p );

    for( nlohmann::json& entry : presets )
        PARAM_LAYER_PRESET::MigrateToNamedRenderLayers( entry );

    m_wasMigrated = true;

    return true;
}


bool PROJECT_FILE::MigrateFromLegacy( QSettings* aCfg )
{
    bool     ret = true;
    QString  str;
    int      index = 0;

    std::set<std::string> group_blacklist;

    auto loadPinnedLibs =
            [&]( const std::string& aDest )
            {
                int      libIndex = 1;
                QString  libKey   = QString( "PinnedItems" );
                libKey += QString::number( libIndex );

                nlohmann::json libs = nlohmann::json::array();

                while( aCfg->contains( libKey ) )
                {
                    str = aCfg->value( libKey ).toString();
                    libs.push_back( str.toStdString() );

                    aCfg->remove( libKey );

                    libKey = QString( "PinnedItems" );
                    libKey += QString::number( ++libIndex );
                }

                Set( aDest, libs );
            };

    aCfg->beginGroup( "LibeditFrame" );
    loadPinnedLibs( "libraries.pinned_symbol_libs" );
    aCfg->endGroup();

    aCfg->beginGroup( "ModEditFrame" );
    loadPinnedLibs( "libraries.pinned_footprint_libs" );
    aCfg->endGroup();

    aCfg->beginGroup( "cvpcb/equfiles" );

    {
        int      eqIdx = 1;
        QString  eqKey = QString( "EquName" );
        eqKey += QString::number( eqIdx );

        nlohmann::json eqs = nlohmann::json::array();

        while( aCfg->contains( eqKey ) )
        {
            str = aCfg->value( eqKey ).toString();
            eqs.push_back( str.toStdString() );

            eqKey = QString( "EquName" );
            eqKey += QString::number( ++eqIdx );
        }

        Set( "cvpcb.equivalence_files", eqs );
    }

    aCfg->endGroup();

    group_blacklist.insert( "cvpcb" );

    aCfg->beginGroup( "eeschema" );
    fromLegacyString( aCfg, "LibDir", "schematic.legacy_lib_dir" );
    aCfg->endGroup();

    aCfg->beginGroup( "eeschema/libraries" );

    {
        int      libIdx = 1;
        QString  libKey = QString( "LibName" );
        libKey += QString::number( libIdx );

        nlohmann::json libs = nlohmann::json::array();

        while( aCfg->contains( libKey ) )
        {
            str = aCfg->value( libKey ).toString();
            libs.push_back( str.toStdString() );

            libKey = QString( "LibName" );
            libKey += QString::number( ++libIdx );
        }

        Set( "schematic.legacy_lib_list", libs );
    }

    aCfg->endGroup();

    group_blacklist.insert( "eeschema" );

    aCfg->beginGroup( "text_variables" );

    {
        int      txtIdx = 1;
        QString  txtKey;
        txtKey = QString::number( txtIdx );

        nlohmann::json vars = nlohmann::json();

        while( aCfg->contains( txtKey ) )
        {
            str = aCfg->value( txtKey ).toString();
            QStringList tokens = str.split( ':' );

            if( tokens.size() == 2 )
                vars[ tokens[0].toStdString() ] = tokens[1].toStdString();

            txtKey = QString::number( ++txtIdx );
        }

        Set( "text_variables", vars );
    }

    aCfg->endGroup();

    group_blacklist.insert( "text_variables" );

    aCfg->beginGroup( "schematic_editor" );

    fromLegacyString( aCfg, "PageLayoutDescrFile",     "schematic.page_layout_descr_file" );
    fromLegacyString( aCfg, "PlotDirectoryName",       "schematic.plot_directory" );
    fromLegacyString( aCfg, "NetFmtName",              "schematic.net_format_name" );
    fromLegacy<bool>( aCfg, "SpiceAjustPassiveValues", "schematic.spice_adjust_passive_values" );
    fromLegacy<int>(  aCfg, "SubpartIdSeparator",      "schematic.subpart_id_separator" );
    fromLegacy<int>(  aCfg, "SubpartFirstId",          "schematic.subpart_first_id" );

    fromLegacy<int>( aCfg, "LineThickness",            "schematic.drawing.default_line_thickness" );
    fromLegacy<int>( aCfg, "WireThickness",            "schematic.drawing.default_wire_thickness" );
    fromLegacy<int>( aCfg, "BusThickness",             "schematic.drawing.default_bus_thickness" );
    fromLegacy<int>( aCfg, "LabSize",                  "schematic.drawing.default_text_size" );

    if( !fromLegacy<int>( aCfg, "PinSymbolSize",       "schematic.drawing.pin_symbol_size" ) )
    {
        Set( "schematic.drawing.pin_symbol_size", 0 );
    }

    fromLegacy<int>( aCfg, "JunctionSize",             "schematic.drawing.default_junction_size" );

    fromLegacyString( aCfg, "FieldNameTemplates",    "schematic.drawing.field_names" );

    if( !fromLegacy<double>( aCfg, "TextOffsetRatio",  "schematic.drawing.text_offset_ratio" ) )
    {
        Set( "schematic.drawing.text_offset_ratio", 0.08 );
        Set( "schematic.drawing.label_size_ratio", 0.25 );
    }

    aCfg->endGroup();

    group_blacklist.insert( "schematic_editor" );

    aCfg->beginGroup( "pcbnew" );

    fromLegacyString( aCfg, "PageLayoutDescrFile",       "pcbnew.page_layout_descr_file" );
    fromLegacyString( aCfg, "LastNetListRead",           "pcbnew.last_paths.netlist" );
    fromLegacyString( aCfg, "LastSTEPExportPath",        "pcbnew.last_paths.step" );
    fromLegacyString( aCfg, "LastIDFExportPath",         "pcbnew.last_paths.idf" );
    fromLegacyString( aCfg, "LastVRMLExportPath",        "pcbnew.last_paths.vmrl" );
    fromLegacyString( aCfg, "LastSpecctraDSNExportPath", "pcbnew.last_paths.specctra_dsn" );
    fromLegacyString( aCfg, "LastGenCADExportPath",      "pcbnew.last_paths.gencad" );

    std::string bp = "board.design_settings.";

    {
        int      idx = 1;
        QString  key = QString( "DRCExclusion" );
        key += QString::number( idx );

        nlohmann::json exclusions = nlohmann::json::array();

        while( aCfg->contains( key ) )
        {
            str = aCfg->value( key ).toString();
            exclusions.push_back( str.toStdString() );

            key = QString( "DRCExclusion" );
            key += QString::number( ++idx );
        }

        Set( bp + "drc_exclusions", exclusions );
    }

    fromLegacy<bool>( aCfg,   "AllowMicroVias",  bp + "rules.allow_microvias" );
    fromLegacy<bool>( aCfg,   "AllowBlindVias",  bp + "rules.allow_blind_buried_vias" );
    fromLegacy<double>( aCfg, "MinClearance",    bp + "rules.min_clearance" );
    fromLegacy<double>( aCfg, "MinTrackWidth",   bp + "rules.min_track_width" );
    fromLegacy<double>( aCfg, "MinViaAnnulus",   bp + "rules.min_via_annulus" );
    fromLegacy<double>( aCfg, "MinViaDiameter",  bp + "rules.min_via_diameter" );

    if( !fromLegacy<double>( aCfg, "MinThroughDrill", bp + "rules.min_through_hole_diameter" ) )
        fromLegacy<double>( aCfg, "MinViaDrill", bp + "rules.min_through_hole_diameter" );

    fromLegacy<double>( aCfg, "MinMicroViaDiameter",  bp + "rules.min_microvia_diameter" );
    fromLegacy<double>( aCfg, "MinMicroViaDrill",     bp + "rules.min_microvia_drill" );
    fromLegacy<double>( aCfg, "MinHoleToHole",        bp + "rules.min_hole_to_hole" );
    fromLegacy<double>( aCfg, "CopperEdgeClearance",  bp + "rules.min_copper_edge_clearance" );
    fromLegacy<double>( aCfg, "SolderMaskClearance",  bp + "rules.solder_mask_clearance" );
    fromLegacy<double>( aCfg, "SolderMaskMinWidth",   bp + "rules.solder_mask_min_width" );
    fromLegacy<double>( aCfg, "SolderPasteClearance", bp + "rules.solder_paste_clearance" );
    fromLegacy<double>( aCfg, "SolderPasteRatio",     bp + "rules.solder_paste_margin_ratio" );

    if( !fromLegacy<double>( aCfg, "SilkLineWidth", bp + "defaults.silk_line_width" ) )
        fromLegacy<double>( aCfg, "ModuleOutlineThickness", bp + "defaults.silk_line_width" );

    if( !fromLegacy<double>( aCfg, "SilkTextSizeV", bp + "defaults.silk_text_size_v" ) )
        fromLegacy<double>( aCfg, "ModuleTextSizeV", bp + "defaults.silk_text_size_v" );

    if( !fromLegacy<double>( aCfg, "SilkTextSizeH", bp + "defaults.silk_text_size_h" ) )
        fromLegacy<double>( aCfg, "ModuleTextSizeH", bp + "defaults.silk_text_size_h" );

    if( !fromLegacy<double>( aCfg, "SilkTextSizeThickness", bp + "defaults.silk_text_thickness" ) )
        fromLegacy<double>( aCfg, "ModuleTextSizeThickness", bp + "defaults.silk_text_thickness" );

    fromLegacy<bool>( aCfg, "SilkTextItalic",   bp + "defaults.silk_text_italic" );
    fromLegacy<bool>( aCfg, "SilkTextUpright",  bp + "defaults.silk_text_upright" );

    if( !fromLegacy<double>( aCfg, "CopperLineWidth", bp + "defaults.copper_line_width" ) )
        fromLegacy<double>( aCfg, "DrawSegmentWidth", bp + "defaults.copper_line_width" );

    if( !fromLegacy<double>( aCfg, "CopperTextSizeV", bp + "defaults.copper_text_size_v" ) )
        fromLegacy<double>( aCfg, "PcbTextSizeV", bp + "defaults.copper_text_size_v" );

    if( !fromLegacy<double>( aCfg, "CopperTextSizeH", bp + "defaults.copper_text_size_h" ) )
        fromLegacy<double>( aCfg, "PcbTextSizeH", bp + "defaults.copper_text_size_h" );

    if( !fromLegacy<double>( aCfg, "CopperTextThickness", bp + "defaults.copper_text_thickness" ) )
        fromLegacy<double>( aCfg, "PcbTextThickness", bp + "defaults.copper_text_thickness" );

    fromLegacy<bool>( aCfg, "CopperTextItalic",   bp + "defaults.copper_text_italic" );
    fromLegacy<bool>( aCfg, "CopperTextUpright",  bp + "defaults.copper_text_upright" );

    if( !fromLegacy<double>( aCfg, "EdgeCutLineWidth", bp + "defaults.board_outline_line_width" ) )
        fromLegacy<double>( aCfg, "BoardOutlineThickness",
                            bp + "defaults.board_outline_line_width" );

    fromLegacy<double>( aCfg, "CourtyardLineWidth",   bp + "defaults.courtyard_line_width" );

    fromLegacy<double>( aCfg, "FabLineWidth",         bp + "defaults.fab_line_width" );
    fromLegacy<double>( aCfg, "FabTextSizeV",         bp + "defaults.fab_text_size_v" );
    fromLegacy<double>( aCfg, "FabTextSizeH",         bp + "defaults.fab_text_size_h" );
    fromLegacy<double>( aCfg, "FabTextSizeThickness", bp + "defaults.fab_text_thickness" );
    fromLegacy<bool>(   aCfg, "FabTextItalic",        bp + "defaults.fab_text_italic" );
    fromLegacy<bool>(   aCfg, "FabTextUpright",       bp + "defaults.fab_text_upright" );

    if( !fromLegacy<double>( aCfg, "OthersLineWidth", bp + "defaults.other_line_width" ) )
        fromLegacy<double>( aCfg, "ModuleOutlineThickness", bp + "defaults.other_line_width" );

    fromLegacy<double>( aCfg, "OthersTextSizeV",         bp + "defaults.other_text_size_v" );
    fromLegacy<double>( aCfg, "OthersTextSizeH",         bp + "defaults.other_text_size_h" );
    fromLegacy<double>( aCfg, "OthersTextSizeThickness", bp + "defaults.other_text_thickness" );
    fromLegacy<bool>(   aCfg, "OthersTextItalic",        bp + "defaults.other_text_italic" );
    fromLegacy<bool>(   aCfg, "OthersTextUpright",       bp + "defaults.other_text_upright" );

    fromLegacy<int>( aCfg, "DimensionUnits",     bp + "defaults.dimension_units" );
    fromLegacy<int>( aCfg, "DimensionPrecision", bp + "defaults.dimension_precision" );

    std::string sev = bp + "rule_severities";

    fromLegacy<bool>( aCfg, "RequireCourtyardDefinitions", sev + "legacy_no_courtyard_defined" );

    fromLegacy<bool>( aCfg, "ProhibitOverlappingCourtyards", sev + "legacy_courtyards_overlap" );

    {
        int      idx     = 1;
        QString  keyBase = "TrackWidth";
        QString  key     = keyBase;
        double   val;

        nlohmann::json widths = nlohmann::json::array();

        key += QString::number( idx );

        while( aCfg->contains( key ) )
        {
            val = aCfg->value( key ).toDouble();
            widths.push_back( val );
            key = keyBase;
            key += QString::number( ++idx );
        }

        Set( bp + "track_widths", widths );
    }

    {
        int      idx     = 1;
        QString  keyBase = "ViaDiameter";
        QString  key     = keyBase;
        double   diameter;
        double   drill   = 1.0;

        nlohmann::json vias = nlohmann::json::array();

        key += QString::number( idx );

        while( aCfg->contains( key ) )
        {
            diameter = aCfg->value( key ).toDouble();
            key = "ViaDrill";
            key += QString::number( idx );
            if( aCfg->contains( key ) )
                drill = aCfg->value( key ).toDouble();

            nlohmann::json via = { { "diameter", diameter }, { "drill", drill } };
            vias.push_back( via );

            key = keyBase;
            key += QString::number( ++idx );
        }

        Set( bp + "via_dimensions", vias );
    }

    {
        int      idx     = 1;
        QString  keyBase = "dPairWidth";
        QString  key     = keyBase;
        double   width;
        double   gap     = 1.0;
        double   via_gap = 1.0;

        nlohmann::json pairs = nlohmann::json::array();

        key += QString::number( idx );

        while( aCfg->contains( key ) )
        {
            width = aCfg->value( key ).toDouble();
            key = "dPairGap";
            key += QString::number( idx );
            if( aCfg->contains( key ) )
                gap = aCfg->value( key ).toDouble();

            key = "dPairViaGap";
            key += QString::number( idx );
            if( aCfg->contains( key ) )
                via_gap = aCfg->value( key ).toDouble();

            nlohmann::json pair = { { "width", width }, { "gap", gap }, { "via_gap", via_gap } };
            pairs.push_back( pair );

            key = keyBase;
            key += QString::number( ++idx );
        }

        Set( bp + "diff_pair_dimensions",  pairs );
    }

    aCfg->endGroup();

    group_blacklist.insert( "pcbnew" );

    group_blacklist.insert( "general" );

    auto loadSheetNames =
            [&]() -> bool
            {
                int            sheet = 1;
                QString        entry;
                nlohmann::json arr   = nlohmann::json::array();

                aCfg->beginGroup( "sheetnames" );

                while( aCfg->contains( QString::number( sheet ) ) )
                {
                    entry = aCfg->value( QString::number( sheet++ ) ).toString();
                    QStringList tokens = entry.split( ':' );

                    if( tokens.size() == 2 )
                    {
                        arr.push_back( nlohmann::json::array( { tokens[0].toStdString(), tokens[1].toStdString() } ) );
                    }
                }

                Set( "sheets", arr );

                aCfg->endGroup();

                return true;
            };

    std::vector<std::string> groups;

    groups.push_back( std::string() );

    auto loadLegacyPairs =
            [&]( const std::string& aGroup ) -> bool
            {
                bool success = true;
                std::string keyStr;
                std::string val;

                QStringList keys = aCfg->childKeys();

                for( const QString& key : keys )
                {
                    val = aCfg->value( key ).toString().toStdString();

                    try
                    {
                        Set( "legacy." + aGroup + "." + key.toStdString(), val );
                    }
                    catch( ... )
                    {
                        success = false;
                    }
                }

                return success;
            };

    for( int i = 0; i < groups.size(); i++ )
    {
        if( !groups[i].empty() )
            aCfg->beginGroup( QString::fromStdString( groups[i] ) );

        if( groups[i] == "sheetnames" )
        {
            ret |= loadSheetNames();
            if( !groups[i].empty() )
                aCfg->endGroup();
            continue;
        }

        if( aCfg->contains( "last_client" ) )
            aCfg->remove( "last_client" );
        if( aCfg->contains( "update" ) )
            aCfg->remove( "update" );
        if( aCfg->contains( "version" ) )
            aCfg->remove( "version" );

        ret &= loadLegacyPairs( groups[i] );

        QStringList childGroups = aCfg->childGroups();

        for( const QString& childGroup : childGroups )
        {
            std::string group = groups[i].empty() ? childGroup.toStdString() : groups[i] + "/" + childGroup.toStdString();

            if( !group_blacklist.count( group ) )
                groups.push_back( group );
        }

        if( !groups[i].isEmpty() )
            aCfg->endGroup();
    }

    return ret;
}


bool PROJECT_FILE::SaveToFile( const QString& aDirectory, bool aForce )
{
    Q_ASSERT( m_project );

    Set( "meta.filename", m_project->GetProjectName() + "." + FILEEXT::ProjectFileExtension );

    bool force = aForce || m_wasMigrated;

    m_wasMigrated = false;

    return JSON_SETTINGS::SaveToFile( aDirectory, force );
}


bool PROJECT_FILE::SaveAs( const QString& aDirectory, const QString& aFile )
{
    QString oldFilename = QString::fromStdString( GetFilename() );
    QString oldProjectName = QFileInfo( oldFilename ).baseName();
    QString oldProjectPath = QFileInfo( oldFilename ).path();

    Set( "meta.filename", aFile + "." + FILEEXT::ProjectFileExtension );
    SetFilename( aFile );

    auto updatePath =
            [&]( std::string& aPath )
            {
                QString qPath = QString::fromStdString( aPath );
                if( qPath.startsWith( oldProjectName + "." ) )
                    qPath.replace( oldProjectName, aFile );
                else if( qPath.startsWith( oldProjectPath + "/" ) )
                    qPath.replace( oldProjectPath, aDirectory );
                aPath = qPath.toStdString();
            };

    updatePath( m_BoardDrawingSheetFile );

    for( int ii = LAST_PATH_FIRST; ii < (int) LAST_PATH_SIZE; ++ii )
        updatePath( m_PcbLastPath[ ii ] );

    auto updatePathByPtr =
            [&]( const std::string& aPtr )
            {
                if( std::optional<std::string> path = Get<std::string>( aPtr ) )
                {
                    updatePath( path.value() );
                    Set( aPtr, path.value() );
                }
            };

    updatePathByPtr( "schematic.page_layout_descr_file" );
    updatePathByPtr( "schematic.plot_directory" );
    updatePathByPtr( "schematic.ngspice.workbook_filename" );
    updatePathByPtr( "pcbnew.page_layout_descr_file" );

    m_wasMigrated = false;

    SetReadOnly( false );
    return JSON_SETTINGS::SaveToFile( aDirectory, true );
}


QString PROJECT_FILE::getFileExt() const
{
    return FILEEXT::ProjectFileExtension;
}


QString PROJECT_FILE::getLegacyFileExt() const
{
    return FILEEXT::LegacyProjectFileExtension;
}


void to_json( nlohmann::json& aJson, const FILE_INFO_PAIR& aPair )
{
    aJson = nlohmann::json::array( { aPair.first.AsString().toStdString(), aPair.second } );
}


void from_json( const nlohmann::json& aJson, FILE_INFO_PAIR& aPair )
{
    if( !aJson.is_array() || aJson.size() != 2 )
        return;
    aPair.first  = KIID( QString::fromStdString( aJson[0].get<std::string>() ) );
    aPair.second = aJson[1].get<std::string>();
}