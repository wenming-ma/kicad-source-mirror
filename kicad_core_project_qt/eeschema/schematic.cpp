
#include <bus_alias.h>
#include <commit.h>
#include <connection_graph.h>
#include <core/ignore.h>
#include <core/kicad_algo.h>
#include <core/profile.h>
#include <sch_collectors.h>
#include <erc/erc_settings.h>
#include <font/outline_font.h>
// #include <netlist_exporter_spice.h> // UNUSED_SYMBOL: SIM functionality disabled
#include <progress_reporter.h>
#include <project.h>
#include <project/net_settings.h>
#include <project/project_file.h>
#include <schematic.h>
#include <sch_bus_entry.h>
#include <sch_commit.h>
#include <sch_junction.h>
#include <sch_label.h>
#include <sch_line.h>
#include <sch_marker.h>
#include <sch_no_connect.h>
#include <sch_rule_area.h>
#include <sch_screen.h>
#include <sch_sheet_pin.h>
#include <sch_selection_tool.h>
// #include <sim/spice_settings.h> // UNUSED_SYMBOL: SIM functionality disabled
// #include <sim/spice_value.h> // UNUSED_SYMBOL: SIM functionality disabled
#include <tool/tool_manager.h>
#include <undo_redo_container.h>

#include <QDebug>
#include <QLoggingCategory>
#include <QFileInfo>

bool SCHEMATIC::m_IsSchematicExists = false;

SCHEMATIC::SCHEMATIC( PROJECT* aPrj ) :
          EDA_ITEM( nullptr, SCHEMATIC_T ),
          m_project( nullptr ),
          m_rootSheet( nullptr ),
          m_schematicHolder( nullptr )
{
    m_currentSheet    = new SCH_SHEET_PATH();
    m_connectionGraph = new CONNECTION_GRAPH( this );
    m_IsSchematicExists = true;

    SetProject( aPrj );

    PROPERTY_MANAGER::Instance().RegisterListener( TYPE_HASH( SCH_FIELD ),
            [&]( INSPECTABLE* aItem, PROPERTY_BASE* aProperty, COMMIT* aCommit )
            {
                // Special case: propagate value, footprint, and datasheet fields to other units
                // of a given symbol if they aren't in the selection

                SCH_FIELD* field = dynamic_cast<SCH_FIELD*>( aItem );

                if( !field || !IsValid() )
                    return;

                SCH_SYMBOL* symbol = dynamic_cast<SCH_SYMBOL*>( field->GetParent() );

                if( !symbol || aProperty->Name() != _HKI( "Text" ) )
                    return;

                // TODO(JE) This will need to get smarter to enable API access
                SCH_SHEET_PATH sheetPath = CurrentSheet();

                QString newValue = aItem->Get<QString>( aProperty );

                if( field->GetId() == REFERENCE_FIELD )
                {
                    symbol->SetRef( &sheetPath, newValue );

                    // The user might want to change all the units to the new ref.  Or they
                    // might not.  Since we have no way of knowing, we default to the most
                    // concrete action (change only the selected reference).
                    return;
                }

                QString ref = symbol->GetRef( &sheetPath );
                int      unit = symbol->GetUnit();
                LIB_ID   libId = symbol->GetLibId();

                for( SCH_SHEET_PATH& sheet : Hierarchy() )
                {
                    std::vector<SCH_SYMBOL*> otherUnits;

                    CollectOtherUnits( ref, unit, libId, sheet, &otherUnits );

                    for( SCH_SYMBOL* otherUnit : otherUnits )
                    {
                        switch( field->GetId() )
                        {
                        case VALUE_FIELD:
                        {
                            if( aCommit )
                                aCommit->Modify( otherUnit, sheet.LastScreen() );

                            otherUnit->SetValueFieldText( newValue );
                            break;
                        }

                        case FOOTPRINT_FIELD:
                        {
                            if( aCommit )
                                aCommit->Modify( otherUnit, sheet.LastScreen() );

                            otherUnit->SetFootprintFieldText( newValue );
                            break;
                        }

                        case DATASHEET_FIELD:
                        {
                            if( aCommit )
                                aCommit->Modify( otherUnit, sheet.LastScreen() );

                            otherUnit->GetField( DATASHEET_FIELD )->SetText( newValue );
                            break;
                        }

                        default:
                            break;
                        }
                    }
                }
            } );
}


SCHEMATIC::~SCHEMATIC()
{
    PROPERTY_MANAGER::Instance().UnregisterListeners( TYPE_HASH( SCH_FIELD ) );

    delete m_currentSheet;
    delete m_connectionGraph;

    m_IsSchematicExists = false;
}


void SCHEMATIC::Reset()
{
    if( m_project )
    {
        PROJECT_FILE& project = m_project->GetProjectFile();

        // d'tor will save settings to file
        delete project.m_ErcSettings;
        project.m_ErcSettings = nullptr;

        // d'tor will save settings to file
        delete project.m_SchematicSettings;
        project.m_SchematicSettings = nullptr;

        m_project = nullptr; // clear the project, so we don't do this again when setting a new one
    }

    delete m_rootSheet;

    m_rootSheet = nullptr;

    m_connectionGraph->Reset();
    m_currentSheet->clear();
}


void SCHEMATIC::SetProject( PROJECT* aPrj )
{
    if( m_project )
    {
        PROJECT_FILE& project = m_project->GetProjectFile();

        // d'tor will save settings to file
        delete project.m_ErcSettings;
        project.m_ErcSettings = nullptr;

        // d'tor will save settings to file
        delete project.m_SchematicSettings;
        project.m_SchematicSettings = nullptr;
    }

    m_project = aPrj;

    if( m_project )
    {
        PROJECT_FILE& project       = m_project->GetProjectFile();
        project.m_ErcSettings       = new ERC_SETTINGS( &project, "erc" );
        project.m_SchematicSettings = new SCHEMATIC_SETTINGS( &project, "schematic" );

        project.m_SchematicSettings->LoadFromFile();
        // UNUSED_SYMBOL: NGSPICE_SETTINGS LoadFromFile - Commented out unused NGSPICE settings load
        // project.m_SchematicSettings->m_NgspiceSettings->LoadFromFile();
        project.m_ErcSettings->LoadFromFile();
    }
}


void SCHEMATIC::SetRoot( SCH_SHEET* aRootSheet )
{
    Q_ASSERT( aRootSheet && "Call to SetRoot with null SCH_SHEET!" );

    m_rootSheet = aRootSheet;

    m_currentSheet->clear();
    m_currentSheet->push_back( m_rootSheet );

    m_hierarchy = BuildSheetListSortedByPageNumbers();
    m_connectionGraph->Reset();
}


SCH_SCREEN* SCHEMATIC::RootScreen() const
{
    return IsValid() ? m_rootSheet->GetScreen() : nullptr;
}


SCH_SHEET_LIST SCHEMATIC::Hierarchy() const
{
    Q_ASSERT( !m_hierarchy.empty() );

    return m_hierarchy;
}


void SCHEMATIC::RefreshHierarchy()
{
    m_hierarchy = BuildSheetListSortedByPageNumbers();
}


void SCHEMATIC::GetContextualTextVars( QStringList* aVars ) const
{
    auto add =
            [&]( const QString& aVar )
            {
                if( !alg::contains( *aVars, aVar ) )
                    aVars->append( aVar );
            };

    add( "#" );
    add( "##" );
    add( "SHEETPATH" );
    add( "SHEETNAME" );
    add( "FILENAME" );
    add( "FILEPATH" );
    add( "PROJECTNAME" );

    if( !CurrentSheet().empty() )
        CurrentSheet().LastScreen()->GetTitleBlock().GetContextualTextVars( aVars );

    for( std::pair<QString, QString> entry : m_project->GetTextVars() )
        add( entry.first );
}


bool SCHEMATIC::ResolveTextVar( const SCH_SHEET_PATH* aSheetPath, QString* token,
                                int aDepth ) const
{
    Q_ASSERT( aSheetPath );
    if( !aSheetPath ) return false;

    if( *token == "#" )
    {
        *token = aSheetPath->GetPageNumber();
        return true;
    }
    else if( *token == "##" )
    {
        *token = QString::asprintf( "%i", Root().CountSheets() );
        return true;
    }
    else if( *token == "SHEETPATH" )
    {
        *token = aSheetPath->PathHumanReadable();
        return true;
    }
    else if( *token == "SHEETNAME" )
    {
        *token = aSheetPath->Last()->GetName();
        return true;
    }
    else if( *token == "FILENAME" )
    {
        QFileInfo fn( GetFileName() );
        *token = fn.fileName();
        return true;
    }
    else if( *token == "FILEPATH" )
    {
        QFileInfo fn( GetFileName() );
        *token = fn.absoluteFilePath();
        return true;
    }
    else if( *token == "PROJECTNAME" )
    {
        *token = m_project->GetProjectName();
        return true;
    }

    if( aSheetPath->LastScreen()->GetTitleBlock().TextVarResolver( token, m_project ) )
        return true;

    if( m_project->TextVarResolver( token ) )
        return true;

    return false;
}


QString SCHEMATIC::GetFileName() const
{
    return IsValid() ? m_rootSheet->GetScreen()->GetFileName() : QString();
}


SCHEMATIC_SETTINGS& SCHEMATIC::Settings() const
{
    Q_ASSERT( m_project );
    return *m_project->GetProjectFile().m_SchematicSettings;
}


ERC_SETTINGS& SCHEMATIC::ErcSettings() const
{
    Q_ASSERT( m_project );
    return *m_project->GetProjectFile().m_ErcSettings;
}


std::vector<SCH_MARKER*> SCHEMATIC::ResolveERCExclusions()
{
    SCH_SHEET_LIST sheetList = Hierarchy();
    ERC_SETTINGS&  settings  = ErcSettings();

    // Migrate legacy marker exclusions to new format to ensure exclusion matching functions across
    // file versions. Silently drops any legacy exclusions which can not be mapped to the new format
    // without risking an incorrect exclusion - this is preferable to silently dropping
    // new ERC errors / warnings due to an incorrect match between a legacy and new
    // marker serialization format
    std::set<QString> migratedExclusions;

    for( auto it = settings.m_ErcExclusions.begin(); it != settings.m_ErcExclusions.end(); )
    {
        SCH_MARKER* testMarker = SCH_MARKER::DeserializeFromString( sheetList, *it );

        if( !testMarker )
        {
            it = settings.m_ErcExclusions.erase( it );
            continue;
        }

        if( testMarker->IsLegacyMarker() )
        {
            const QString settingsKey = testMarker->GetRCItem()->GetSettingsKey();

            if(    settingsKey != "pin_to_pin"
                && settingsKey != "hier_label_mismatch"
                && settingsKey != "different_unit_net" )
            {
                migratedExclusions.insert( testMarker->SerializeToString() );
            }

            it = settings.m_ErcExclusions.erase( it );
        }
        else
        {
            ++it;
        }

        delete testMarker;
    }

    settings.m_ErcExclusions.insert( migratedExclusions.begin(), migratedExclusions.end() );

    // End of legacy exclusion removal / migrations

    for( const SCH_SHEET_PATH& sheet : sheetList )
    {
        for( SCH_ITEM* item : sheet.LastScreen()->Items().OfType( SCH_MARKER_T ) )
        {
            SCH_MARKER*                  marker = static_cast<SCH_MARKER*>( item );
            QString                     serialized = marker->SerializeToString();
            std::set<QString>::iterator it = settings.m_ErcExclusions.find( serialized );

            if( it != settings.m_ErcExclusions.end() )
            {
                marker->SetExcluded( true, settings.m_ErcExclusionComments[serialized] );
                settings.m_ErcExclusions.erase( it );
            }
        }
    }

    std::vector<SCH_MARKER*> newMarkers;

    for( const QString& serialized : settings.m_ErcExclusions )
    {
        SCH_MARKER* marker = SCH_MARKER::DeserializeFromString( sheetList, serialized );

        if( marker )
        {
            marker->SetExcluded( true, settings.m_ErcExclusionComments[serialized] );
            newMarkers.push_back( marker );
        }
    }

    settings.m_ErcExclusions.clear();

    return newMarkers;
}


std::shared_ptr<BUS_ALIAS> SCHEMATIC::GetBusAlias( const QString& aLabel ) const
{
    for( const SCH_SHEET_PATH& sheet : Hierarchy() )
    {
        for( const std::shared_ptr<BUS_ALIAS>& alias : sheet.LastScreen()->GetBusAliases() )
        {
            if( alias->GetName() == aLabel )
                return alias;
        }
    }

    return nullptr;
}


std::set<QString> SCHEMATIC::GetNetClassAssignmentCandidates()
{
    std::set<QString> names;

    for( const auto& [ key, subgraphList ] : m_connectionGraph->GetNetMap() )
    {
        CONNECTION_SUBGRAPH* firstSubgraph = subgraphList[0];

        if( !firstSubgraph->GetDriverConnection()->IsBus()
                && firstSubgraph->GetDriverPriority() >= CONNECTION_SUBGRAPH::PRIORITY::PIN )
        {
            names.insert( key.Name );
        }
    }

    return names;
}


bool SCHEMATIC::ResolveCrossReference( QString* token, int aDepth ) const
{
    QString       remainder;
    QString       ref = token->section( ':', 0, 0 );
    remainder = token->section( ':', 1 );
    KIID_PATH      path( ref );
    KIID           uuid = path.back();
    SCH_SHEET_PATH sheetPath;
    SCH_ITEM*      refItem = GetItem( KIID( uuid ), &sheetPath );

    if( path.size() > 1 )
    {
        path.pop_back();
        sheetPath = Hierarchy().GetSheetPathByKIIDPath( path ).value_or( sheetPath );
    }

    if( refItem && refItem->Type() == SCH_SYMBOL_T )
    {
        SCH_SYMBOL* refSymbol = static_cast<SCH_SYMBOL*>( refItem );

        if( refSymbol->ResolveTextVar( &sheetPath, &remainder, aDepth + 1 ) )
            *token = remainder;
        else
            *token = refSymbol->GetRef( &sheetPath, true ) + ":" + remainder;

        return true;    // Cross-reference is resolved whether or not the actual textvar was
    }
    else if( refItem && refItem->Type() == SCH_SHEET_T )
    {
        SCH_SHEET* refSheet = static_cast<SCH_SHEET*>( refItem );

        sheetPath.push_back( refSheet );

        if( refSheet->ResolveTextVar( &sheetPath, &remainder, aDepth + 1 ) )
            *token = remainder;

        return true;    // Cross-reference is resolved whether or not the actual textvar was
    }

    return false;
}


std::map<int, QString> SCHEMATIC::GetVirtualPageToSheetNamesMap() const
{
    std::map<int, QString> namesMap;

    for( const SCH_SHEET_PATH& sheet : Hierarchy() )
    {
        if( sheet.size() == 1 )
            namesMap[sheet.GetVirtualPageNumber()] = _( "<root sheet>" );
        else
            namesMap[sheet.GetVirtualPageNumber()] = sheet.Last()->GetName();
    }

    return namesMap;
}


std::map<int, QString> SCHEMATIC::GetVirtualPageToSheetPagesMap() const
{
    std::map<int, QString> pagesMap;

    for( const SCH_SHEET_PATH& sheet : Hierarchy() )
        pagesMap[sheet.GetVirtualPageNumber()] = sheet.GetPageNumber();

    return pagesMap;
}


QString SCHEMATIC::ConvertRefsToKIIDs( const QString& aSource ) const
{
    QString newbuf;
    size_t   sourceLen = aSource.length();

    for( size_t i = 0; i < sourceLen; ++i )
    {
        if( aSource[i] == '$' && i + 1 < sourceLen && aSource[i+1] == '{' )
        {
            QString token;
            bool     isCrossRef = false;
            int      nesting = 0;

            for( i = i + 2; i < sourceLen; ++i )
            {
                if( aSource[i] == '{'
                        && ( aSource[i-1] == '_' || aSource[i-1] == '^' || aSource[i-1] == '~' ) )
                {
                    nesting++;
                }

                if( aSource[i] == '}' )
                {
                    nesting--;

                    if( nesting < 0 )
                        break;
                }

                if( aSource[i] == ':' )
                    isCrossRef = true;

                token.append( aSource[i] );
            }

            if( isCrossRef )
            {
                QString           remainder;
                QString           ref = token.section( ':', 0, 0 );
                remainder = token.section( ':', 1 );
                SCH_REFERENCE_LIST references;

                Hierarchy().GetSymbols( references );

                for( size_t jj = 0; jj < references.GetCount(); jj++ )
                {
                    SCH_SYMBOL* refSymbol = references[ jj ].GetSymbol();

                    if( ref == refSymbol->GetRef( &references[ jj ].GetSheetPath(), true ) )
                    {
                        KIID_PATH path = references[ jj ].GetSheetPath().Path();
                        path.push_back( refSymbol->m_Uuid );

                        token = path.AsString() + ":" + remainder;
                        break;
                    }
                }
            }

            newbuf.append( "${" + token + "}" );
        }
        else
        {
            newbuf.append( aSource[i] );
        }
    }

    return newbuf;
}


QString SCHEMATIC::ConvertKIIDsToRefs( const QString& aSource ) const
{
    QString newbuf;
    size_t   sourceLen = aSource.length();

    for( size_t i = 0; i < sourceLen; ++i )
    {
        if( aSource[i] == '$' && i + 1 < sourceLen && aSource[i+1] == '{' )
        {
            QString token;
            bool     isCrossRef = false;

            for( i = i + 2; i < sourceLen; ++i )
            {
                if( aSource[i] == '}' )
                    break;

                if( aSource[i] == ':' )
                    isCrossRef = true;

                token.append( aSource[i] );
            }

            if( isCrossRef )
            {
                QString       remainder;
                QString       ref = token.section( ':', 0, 0 );
                remainder = token.section( ':', 1 );
                KIID_PATH      path( ref );
                KIID           uuid = path.back();
                SCH_SHEET_PATH sheetPath;
                SCH_ITEM*      refItem = GetItem( uuid, &sheetPath );

                if( path.size() > 1 )
                {
                    path.pop_back();
                    sheetPath = Hierarchy().GetSheetPathByKIIDPath( path ).value_or( sheetPath );
                }

                if( refItem && refItem->Type() == SCH_SYMBOL_T )
                {
                    SCH_SYMBOL* refSymbol = static_cast<SCH_SYMBOL*>( refItem );
                    token = refSymbol->GetRef( &sheetPath, true ) + ":" + remainder;
                }
            }

            newbuf.append( "${" + token + "}" );
        }
        else
        {
            newbuf.append( aSource[i] );
        }
    }

    return newbuf;
}


void SCHEMATIC::SetLegacySymbolInstanceData()
{
    SCH_SCREENS screens( m_rootSheet );

    screens.SetLegacySymbolInstanceData();
}


QString SCHEMATIC::GetUniqueFilenameForCurrentSheet()
{
    // Filename is rootSheetName-sheetName-...-sheetName
    // Note that we need to fetch the rootSheetName out of its filename, as the root SCH_SHEET's
    // name is just a timestamp.

    QFileInfo rootFn( CurrentSheet().at( 0 )->GetFileName() );
    QString   filename = rootFn.baseName();

    for( unsigned i = 1; i < CurrentSheet().size(); i++ )
        filename += "-" + CurrentSheet().at( i )->GetName();

    return filename;
}


void SCHEMATIC::SetSheetNumberAndCount()
{
    SCH_SCREEN* screen;
    SCH_SCREENS s_list( Root() );

    // Set the sheet count, and the sheet number (1 for root sheet)
    int              sheet_count = Root().CountSheets();
    int              sheet_number = 1;
    const KIID_PATH& current_sheetpath = CurrentSheet().Path();

    // @todo Remove all pseudo page number system is left over from prior to real page number
    //       implementation.
    for( const SCH_SHEET_PATH& sheet : Hierarchy() )
    {
        if( sheet.Path() == current_sheetpath ) // Current sheet path found
            break;

        sheet_number++; // Not found, increment before this current path
    }

    for( screen = s_list.GetFirst(); screen != nullptr; screen = s_list.GetNext() )
        screen->SetPageCount( sheet_count );

    CurrentSheet().SetVirtualPageNumber( sheet_number );
    CurrentSheet().LastScreen()->SetVirtualPageNumber( sheet_number );
    CurrentSheet().LastScreen()->SetPageNumber( CurrentSheet().GetPageNumber() );
}


void SCHEMATIC::RecomputeIntersheetRefs()
{
    std::map<QString, std::set<int>>& pageRefsMap = GetPageRefsMap();

    pageRefsMap.clear();

    for( const SCH_SHEET_PATH& sheet : Hierarchy() )
    {
        for( SCH_ITEM* item : sheet.LastScreen()->Items().OfType( SCH_GLOBAL_LABEL_T ) )
        {
            SCH_GLOBALLABEL* global = static_cast<SCH_GLOBALLABEL*>( item );
            QString         resolvedLabel = global->GetShownText( &sheet, false );

            pageRefsMap[ resolvedLabel ].insert( sheet.GetVirtualPageNumber() );
        }
    }

    bool show = Settings().m_IntersheetRefsShow;

    // Refresh all visible global labels.  Note that we have to collect them first as the
    // SCH_SCREEN::Update() call is going to invalidate the RTree iterator.

    std::vector<SCH_GLOBALLABEL*> currentSheetGlobalLabels;

    for( EDA_ITEM* item : CurrentSheet().LastScreen()->Items().OfType( SCH_GLOBAL_LABEL_T ) )
        currentSheetGlobalLabels.push_back( static_cast<SCH_GLOBALLABEL*>( item ) );

    for( SCH_GLOBALLABEL* globalLabel : currentSheetGlobalLabels )
    {
        std::vector<SCH_FIELD>& fields = globalLabel->GetFields();

        fields[0].SetVisible( show );

        if( show )
        {
            if( fields.size() == 1 && fields[0].GetTextPos() == globalLabel->GetPosition() )
                globalLabel->AutoplaceFields( CurrentSheet().LastScreen(), AUTOPLACE_AUTO );

            CurrentSheet().LastScreen()->Update( globalLabel );

            for( SCH_FIELD& field : globalLabel->GetFields() )
                field.ClearBoundingBoxCache();

            globalLabel->ClearBoundingBoxCache();

            if( m_schematicHolder )
                m_schematicHolder->IntersheetRefUpdate( globalLabel );
        }
    }
}


QString SCHEMATIC::GetOperatingPoint( const QString& aNetName, int aPrecision,
                                       const QString& aRange )
{
    QString spiceNetName( aNetName.toLower() );
    // UNUSED_SYMBOL: ConvertToSpiceMarkup - Method implementation not available in minimal set
    // NETLIST_EXPORTER_SPICE::ConvertToSpiceMarkup( &spiceNetName );

    if( spiceNetName == "gnd" || spiceNetName == "0" )
        return QString();

    auto it = m_operatingPoints.find( spiceNetName );

    if( it != m_operatingPoints.end() )
        // UNUSED_SYMBOL: SPICE_VALUE constructor with Normalize() - Implementation missing
        // return SPICE_VALUE( it->second ).ToString( { aPrecision, aRange } );
        return QString::asprintf( "%g", it->second );  // Simple fallback without normalization
    else if( m_operatingPoints.empty() )
        return "--";
    else
        return "?";
}


void SCHEMATIC::FixupJunctions()
{
    SCH_SCREENS screens( Root() );

    for( SCH_SCREEN* screen = screens.GetFirst(); screen; screen = screens.GetNext() )
    {
        std::deque<EDA_ITEM*> allItems;

        for( auto item : screen->Items() )
            allItems.push_back( item );

        // Add missing junctions and breakup wires as needed
        for( const VECTOR2I& point : screen->GetNeededJunctions( allItems ) )
        {
            SCH_JUNCTION* junction = new SCH_JUNCTION( point );
            screen->Append( junction );

            // Breakup wires
            for( SCH_LINE* wire : screen->GetBusesAndWires( point, true ) )
            {
                SCH_LINE* newSegment = wire->BreakAt( point );
                screen->Append( newSegment );
            }
        }
    }
}


void SCHEMATIC::OnItemsAdded( std::vector<SCH_ITEM*>& aNewItems )
{
    InvokeListeners( &SCHEMATIC_LISTENER::OnSchItemsAdded, *this, aNewItems );
}


void SCHEMATIC::OnItemsRemoved( std::vector<SCH_ITEM*>& aRemovedItems )
{
    InvokeListeners( &SCHEMATIC_LISTENER::OnSchItemsRemoved, *this, aRemovedItems );
}


void SCHEMATIC::OnItemsChanged( std::vector<SCH_ITEM*>& aItems )
{
    InvokeListeners( &SCHEMATIC_LISTENER::OnSchItemsChanged, *this, aItems );
}


void SCHEMATIC::OnSchSheetChanged()
{
    InvokeListeners( &SCHEMATIC_LISTENER::OnSchSheetChanged, *this );
}


void SCHEMATIC::AddListener( SCHEMATIC_LISTENER* aListener )
{
    if( !alg::contains( m_listeners, aListener ) )
        m_listeners.push_back( aListener );
}


void SCHEMATIC::RemoveListener( SCHEMATIC_LISTENER* aListener )
{
    auto i = std::find( m_listeners.begin(), m_listeners.end(), aListener );

    if( i != m_listeners.end() )
    {
        std::iter_swap( i, m_listeners.end() - 1 );
        m_listeners.pop_back();
    }
}


void SCHEMATIC::RemoveAllListeners()
{
    m_listeners.clear();
}


void SCHEMATIC::RecordERCExclusions()
{
    // Use a sorted sheetList to reduce file churn
    SCH_SHEET_LIST sheetList = Hierarchy();
    ERC_SETTINGS&  ercSettings = ErcSettings();

    ercSettings.m_ErcExclusions.clear();
    ercSettings.m_ErcExclusionComments.clear();

    for( unsigned i = 0; i < sheetList.size(); i++ )
    {
        for( SCH_ITEM* item : sheetList[i].LastScreen()->Items().OfType( SCH_MARKER_T ) )
        {
            SCH_MARKER* marker = static_cast<SCH_MARKER*>( item );

            if( marker->IsExcluded() )
            {
                QString serialized = marker->SerializeToString();
                ercSettings.m_ErcExclusions.insert( serialized );
                ercSettings.m_ErcExclusionComments[ serialized ] = marker->GetComment();
            }
        }
    }
}


void SCHEMATIC::ResolveERCExclusionsPostUpdate()
{
    SCH_SHEET_LIST sheetList = Hierarchy();

    for( SCH_MARKER* marker : ResolveERCExclusions() )
    {
        SCH_SHEET_PATH errorPath;
        ignore_unused( sheetList.GetItem( marker->GetRCItem()->GetMainItemID(), &errorPath ) );

        if( errorPath.LastScreen() )
            errorPath.LastScreen()->Append( marker );
        else
            RootScreen()->Append( marker );
    }
}


EMBEDDED_FILES* SCHEMATIC::GetEmbeddedFiles()
{
    return static_cast<EMBEDDED_FILES*>( this );
}


const EMBEDDED_FILES* SCHEMATIC::GetEmbeddedFiles() const
{
    return static_cast<const EMBEDDED_FILES*>( this );
}


void SCHEMATIC::RunOnNestedEmbeddedFiles( const std::function<void( EMBEDDED_FILES* )>& aFunction )
{
    SCH_SCREENS screens( Root() );

    for( SCH_SCREEN* screen = screens.GetFirst(); screen; screen = screens.GetNext() )
    {
        for( auto& [name, libSym] : screen->GetLibSymbols() )
            aFunction( libSym->GetEmbeddedFiles() );
    }
}


std::set<KIFONT::OUTLINE_FONT*> SCHEMATIC::GetFonts() const
{
    std::set<KIFONT::OUTLINE_FONT*> fonts;

    SCH_SHEET_LIST sheetList = Hierarchy();

    for( const SCH_SHEET_PATH& sheet : sheetList )
    {
        for( SCH_ITEM* item : sheet.LastScreen()->Items() )
        {
            if( EDA_TEXT* text = dynamic_cast<EDA_TEXT*>( item ) )
            {
                KIFONT::FONT* font = text->GetFont();

                if( !font || font->IsStroke() )
                    continue;

                using EMBEDDING_PERMISSION = KIFONT::OUTLINE_FONT::EMBEDDING_PERMISSION;
                auto* outline = static_cast<KIFONT::OUTLINE_FONT*>( font );

                if( outline->GetEmbeddingPermission() == EMBEDDING_PERMISSION::EDITABLE
                    || outline->GetEmbeddingPermission() == EMBEDDING_PERMISSION::INSTALLABLE )
                {
                    fonts.insert( outline );
                }
            }
        }
    }

    return fonts;
}


void SCHEMATIC::EmbedFonts()
{
    std::set<KIFONT::OUTLINE_FONT*> fonts = GetFonts();

    for( KIFONT::OUTLINE_FONT* font : fonts )
    {
        auto file = GetEmbeddedFiles()->AddFile( font->GetFileName(), false );

        if( !file )
        {
            qDebug() << "Failed to add font file:" << font->GetFileName();
            continue;
        }

        file->type = EMBEDDED_FILES::EMBEDDED_FILE::FILE_TYPE::FONT;
    }
}


std::set<const SCH_SCREEN*> SCHEMATIC::GetSchematicsSharedByMultipleProjects() const
{
    std::set<const SCH_SCREEN*> retv;

    if( !m_rootSheet ) return retv;

    SCH_SHEET_LIST hierarchy( m_rootSheet );
    SCH_SCREENS screens( m_rootSheet );

    for( const SCH_SCREEN* screen = screens.GetFirst(); screen; screen = screens.GetNext() )
    {
        for( const SCH_ITEM* item : screen->Items().OfType( SCH_SYMBOL_T ) )
        {
            const SCH_SYMBOL* symbol = static_cast<const SCH_SYMBOL*>( item );

            const std::vector<SCH_SYMBOL_INSTANCE> symbolInstances = symbol->GetInstances();

            for( const SCH_SYMBOL_INSTANCE& instance : symbolInstances )
            {
                if( !hierarchy.HasPath( instance.m_Path ) )
                {
                    retv.insert( screen );
                    break;
                }
            }

            if( retv.count( screen ) )
                break;
        }
    }

    return retv;
}


bool SCHEMATIC::IsComplexHierarchy() const
{
    if( !m_rootSheet ) return false;

    SCH_SCREENS screens( m_rootSheet );

    for( const SCH_SCREEN* screen = screens.GetFirst(); screen; screen = screens.GetNext() )
    {
        if( !screen ) continue;

        if( screen->GetRefCount() > 1 )
            return true;
    }

    return false;
}


void SCHEMATIC::BreakSegment( SCH_COMMIT* aCommit, SCH_LINE* aSegment, const VECTOR2I& aPoint,
                              SCH_LINE** aNewSegment, SCH_SCREEN* aScreen )
{
    // Save the copy of aSegment before breaking it
    aCommit->Modify( aSegment, aScreen );

    SCH_LINE* newSegment = aSegment->BreakAt( aPoint );

    aSegment->SetFlags( IS_CHANGED | IS_BROKEN );
    newSegment->SetFlags( IS_NEW | IS_BROKEN );

    if( m_schematicHolder )
        m_schematicHolder->AddToScreen( newSegment, aScreen );

    aCommit->Added( newSegment, aScreen );

    *aNewSegment = newSegment;
}


bool SCHEMATIC::BreakSegments( SCH_COMMIT* aCommit, const VECTOR2I& aPos, SCH_SCREEN* aScreen )
{
    bool      brokenSegments = false;
    SCH_LINE* new_line;

    for( SCH_LINE* wire : aScreen->GetBusesAndWires( aPos, true ) )
    {
        BreakSegment( aCommit, wire, aPos, &new_line, aScreen );
        brokenSegments = true;
    }

    return brokenSegments;
}


bool SCHEMATIC::BreakSegmentsOnJunctions( SCH_COMMIT* aCommit, SCH_SCREEN* aScreen )
{
    bool brokenSegments = false;

    std::set<VECTOR2I> point_set;

    for( SCH_ITEM* item : aScreen->Items().OfType( SCH_JUNCTION_T ) )
        point_set.insert( item->GetPosition() );

    for( SCH_ITEM* item : aScreen->Items().OfType( SCH_BUS_WIRE_ENTRY_T ) )
    {
        SCH_BUS_WIRE_ENTRY* entry = static_cast<SCH_BUS_WIRE_ENTRY*>( item );
        point_set.insert( entry->GetPosition() );
        point_set.insert( entry->GetEnd() );
    }

    for( const VECTOR2I& pt : point_set )
    {
        BreakSegments( aCommit, pt, aScreen );
        brokenSegments = true;
    }

    return brokenSegments;
}


void SCHEMATIC::CleanUp( SCH_COMMIT* aCommit, SCH_SCREEN* aScreen )
{
    SCH_SELECTION_TOOL*          selectionTool = m_schematicHolder ? m_schematicHolder->GetSelectionTool() : nullptr;
    std::vector<SCH_LINE*>       lines;
    std::vector<SCH_JUNCTION*>   junctions;
    std::vector<SCH_NO_CONNECT*> ncs;
    std::vector<SCH_ITEM*>       items_to_remove;
    bool                         changed = true;

    if( aScreen == nullptr )
        aScreen = GetCurrentScreen();

    auto remove_item = [&]( SCH_ITEM* aItem ) -> void
                       {
                           changed = true;

                           if( !( aItem->GetFlags() & STRUCT_DELETED ) )
                           {
                               aItem->SetFlags( STRUCT_DELETED );

                               // UNUSED_SYMBOL: RemoveItemFromSel - Method not implemented in minimal set
                               // if( aItem->IsSelected() && selectionTool )
                               //     selectionTool->RemoveItemFromSel( aItem, true /*quiet mode*/ );

                               if( m_schematicHolder )
                               {
                                   m_schematicHolder->RemoveFromScreen( aItem, aScreen );
                               }
                               aCommit->Removed( aItem, aScreen );
                           }
                       };

    BreakSegmentsOnJunctions( aCommit, aScreen );

    for( SCH_ITEM* item : aScreen->Items().OfType( SCH_JUNCTION_T ) )
    {
        if( !aScreen->IsExplicitJunction( item->GetPosition() ) )
            items_to_remove.push_back( item );
        else
            junctions.push_back( static_cast<SCH_JUNCTION*>( item ) );
    }

    for( SCH_ITEM* item : items_to_remove )
        remove_item( item );

    for( SCH_ITEM* item : aScreen->Items().OfType( SCH_NO_CONNECT_T ) )
        ncs.push_back( static_cast<SCH_NO_CONNECT*>( item ) );

    alg::for_all_pairs( junctions.begin(), junctions.end(),
            [&]( SCH_JUNCTION* aFirst, SCH_JUNCTION* aSecond )
            {
                if( ( aFirst->GetEditFlags() & STRUCT_DELETED )
                        || ( aSecond->GetEditFlags() & STRUCT_DELETED ) )
                {
                    return;
                }

                if( aFirst->GetPosition() == aSecond->GetPosition() )
                    remove_item( aSecond );
            } );

    alg::for_all_pairs( ncs.begin(), ncs.end(),
            [&]( SCH_NO_CONNECT* aFirst, SCH_NO_CONNECT* aSecond )
            {
                if( ( aFirst->GetEditFlags() & STRUCT_DELETED )
                        || ( aSecond->GetEditFlags() & STRUCT_DELETED ) )
                {
                    return;
                }

                if( aFirst->GetPosition() == aSecond->GetPosition() )
                    remove_item( aSecond );
            } );


    auto minX = []( const SCH_LINE* l )
                {
                    return std::min( l->GetStartPoint().x, l->GetEndPoint().x );
                };

    auto maxX = []( const SCH_LINE* l )
                {
                    return std::max( l->GetStartPoint().x, l->GetEndPoint().x );
                };

    auto minY = []( const SCH_LINE* l )
                {
                    return std::min( l->GetStartPoint().y, l->GetEndPoint().y );
                };

    auto maxY = []( const SCH_LINE* l )
                {
                    return std::max( l->GetStartPoint().y, l->GetEndPoint().y );
                };

    // Would be nice to put lines in a canonical form here by swapping
    //  start <-> end as needed but I don't know what swapping breaks.
    while( changed )
    {
        changed = false;
        lines.clear();

        for( SCH_ITEM* item : aScreen->Items().OfType( SCH_LINE_T ) )
        {
            if( item->GetLayer() == LAYER_WIRE || item->GetLayer() == LAYER_BUS )
                lines.push_back( static_cast<SCH_LINE*>( item ) );
        }

        // Sort by minimum X position
        std::sort( lines.begin(), lines.end(),
                   [&]( const SCH_LINE* a, const SCH_LINE* b )
                   {
                       return minX( a ) < minX( b );
                   } );

        for( auto it1 = lines.begin(); it1 != lines.end(); ++it1 )
        {
            SCH_LINE* firstLine = *it1;

            if( firstLine->GetEditFlags() & STRUCT_DELETED )
                continue;

            if( firstLine->IsNull() )
            {
                remove_item( firstLine );
                continue;
            }

            int  firstRightXEdge = maxX( firstLine );
            auto it2 = it1;

            for( ++it2; it2 != lines.end(); ++it2 )
            {
                SCH_LINE* secondLine = *it2;
                int       secondLeftXEdge = minX( secondLine );

                // impossible to overlap remaining lines
                if( secondLeftXEdge > firstRightXEdge )
                    break;

                // No Y axis overlap
                if( !( std::max( minY( firstLine ), minY( secondLine ) )
                       <= std::min( maxY( firstLine ), maxY( secondLine ) ) ) )
                {
                    continue;
                }

                if( secondLine->GetFlags() & STRUCT_DELETED )
                    continue;

                if( !secondLine->IsParallel( firstLine )
                        || !secondLine->IsStrokeEquivalent( firstLine )
                        || secondLine->GetLayer() != firstLine->GetLayer() )
                {
                    continue;
                }

                // Remove identical lines
                if( firstLine->IsEndPoint( secondLine->GetStartPoint() )
                        && firstLine->IsEndPoint( secondLine->GetEndPoint() ) )
                {
                    remove_item( secondLine );
                    continue;
                }

                // See if we can merge an overlap (or two colinear touching segments with
                // no junction where they meet).
                SCH_LINE* mergedLine = secondLine->MergeOverlap( aScreen, firstLine, true );

                if( mergedLine != nullptr )
                {
                    remove_item( firstLine );
                    remove_item( secondLine );

                    if( m_schematicHolder )
                    {
                        m_schematicHolder->AddToScreen( mergedLine, aScreen );
                    }

                    aCommit->Added( mergedLine, aScreen );

                    // UNUSED_SYMBOL: ?AddItemToSel@SELECTION_TOOL@@QEAAXPEAVEDA_ITEM@@_ - Method call commented out
                    // if( firstLine->IsSelected() || secondLine->IsSelected() )
                    //     selectionTool->AddItemToSel( mergedLine, true /*quiet mode*/ );

                    break;
                }
            }
        }
    }
}


void SCHEMATIC::RecalculateConnections( SCH_COMMIT* aCommit, SCH_CLEANUP_FLAGS aCleanupFlags,
                                        TOOL_MANAGER* aToolManager,
                                        KIGFX::SCH_VIEW* aSchView,
                                        std::function<void( SCH_ITEM* )>* aChangedItemHandler,
                                        PICKED_ITEMS_LIST* aLastChangeList )
{
    SCHEMATIC_SETTINGS& settings = Settings();
    SCH_SHEET_LIST      list = Hierarchy();
    // UNUSED_SYMBOL: SCH_COMMIT destructor - Commenting out localCommit creation and assignment
    // SCH_COMMIT          localCommit( aToolManager );
    //
    // if( !aCommit )
    //     aCommit = &localCommit;

    PROF_TIMER timer;

    // Ensure schematic graph is accurate
    if( aCleanupFlags == LOCAL_CLEANUP )
    {
        CleanUp( aCommit, GetCurrentScreen() );
    }
    else if( aCleanupFlags == GLOBAL_CLEANUP )
    {
        for( const SCH_SHEET_PATH& sheet : list )
            CleanUp( aCommit, sheet.LastScreen() );
    }

    timer.Stop();
    qDebug() << "SchematicCleanUp()" << timer.msecs() << "ms";

    if( settings.m_IntersheetRefsShow )
        RecomputeIntersheetRefs();

    if( !ADVANCED_CFG::GetCfg().m_IncrementalConnectivity
            || aCleanupFlags == GLOBAL_CLEANUP
            || aLastChangeList == nullptr
            || ConnectionGraph()->IsMinor() )
    {
        // Clear all resolved netclass caches in case labels have changed
        m_project->GetProjectFile().NetSettings()->ClearAllCaches();

        // Update all rule areas so we can cascade implied connectivity changes
        std::unordered_set<SCH_SCREEN*> all_screens;

        for( const SCH_SHEET_PATH& path : list )
            all_screens.insert( path.LastScreen() );

        SCH_RULE_AREA::UpdateRuleAreasInScreens( all_screens, aSchView );

        // Recalculate all connectivity
        ConnectionGraph()->Recalculate( list, true, aChangedItemHandler );
    }
    else
    {
        struct CHANGED_ITEM
        {
            SCH_ITEM*   item;
            SCH_ITEM*   linked_item;
            SCH_SCREEN* screen;
        };

        // Final change sets
        std::set<SCH_ITEM*>                            changed_items;
        std::set<VECTOR2I>                             pts;
        std::set<std::pair<SCH_SHEET_PATH, SCH_ITEM*>> item_paths;

        // Working change sets
        std::unordered_set<SCH_SCREEN*>                  changed_screens;
        std::set<std::pair<SCH_RULE_AREA*, SCH_SCREEN*>> changed_rule_areas;
        std::vector<CHANGED_ITEM>                        changed_connectable_items;

        // Lambda to add an item to the connectivity update sets
        auto addItemToChangeSet = [&changed_items, &pts, &item_paths]( CHANGED_ITEM itemData )
        {
            std::vector<SCH_SHEET_PATH>& paths = itemData.screen->GetClientSheetPaths();

            std::vector<VECTOR2I> tmp_pts = itemData.item->GetConnectionPoints();
            pts.insert( tmp_pts.begin(), tmp_pts.end() );
            changed_items.insert( itemData.item );

            for( SCH_SHEET_PATH& path : paths )
                item_paths.insert( std::make_pair( path, itemData.item ) );

            if( !itemData.linked_item || !itemData.linked_item->IsConnectable() )
                return;

            tmp_pts = itemData.linked_item->GetConnectionPoints();
            pts.insert( tmp_pts.begin(), tmp_pts.end() );
            changed_items.insert( itemData.linked_item );

            // We have to directly add the pins here because the link may not exist on the schematic
            // anymore and so won't be picked up by GetScreen()->Items().Overlapping() below.
            if( SCH_SYMBOL* symbol = dynamic_cast<SCH_SYMBOL*>( itemData.linked_item ) )
            {
                std::vector<SCH_PIN*> pins = symbol->GetPins();
                changed_items.insert( pins.begin(), pins.end() );
            }

            for( SCH_SHEET_PATH& path : paths )
                item_paths.insert( std::make_pair( path, itemData.linked_item ) );
        };

        // Get all changed connectable items and determine all changed screens
        for( unsigned ii = 0; ii < aLastChangeList->GetCount(); ++ii )
        {
            switch( aLastChangeList->GetPickedItemStatus( ii ) )
            {
            // Only care about changed, new, and deleted items, the other
            // cases are not connectivity-related
            case UNDO_REDO::CHANGED:
            case UNDO_REDO::NEWITEM:
            case UNDO_REDO::DELETED:
                break;

            default:
                continue;
            }

            SCH_ITEM* item = dynamic_cast<SCH_ITEM*>( aLastChangeList->GetPickedItem( ii ) );

            if( item )
            {
                SCH_SCREEN* screen = static_cast<SCH_SCREEN*>( aLastChangeList->GetScreenForItem( ii ) );
                changed_screens.insert( screen );

                if( item->Type() == SCH_RULE_AREA_T )
                {
                    SCH_RULE_AREA* ruleArea = static_cast<SCH_RULE_AREA*>( item );

                    // Clear item and directive associations for this rule area
                    ruleArea->ResetDirectivesAndItems( aSchView );

                    changed_rule_areas.insert( { ruleArea, screen } );
                }
                else if( item->IsConnectable() )
                {
                    SCH_ITEM* linked_item = dynamic_cast<SCH_ITEM*>( aLastChangeList->GetPickedItemLink( ii ) );
                    changed_connectable_items.push_back( { item, linked_item, screen } );
                }
            }
        }

        // Update rule areas in changed screens to propagate any directive connectivity changes
        std::vector<std::pair<SCH_RULE_AREA*, SCH_SCREEN*>> forceUpdateRuleAreas =
                SCH_RULE_AREA::UpdateRuleAreasInScreens( changed_screens, aSchView );

        std::for_each( forceUpdateRuleAreas.begin(), forceUpdateRuleAreas.end(),
                       [&]( std::pair<SCH_RULE_AREA*, SCH_SCREEN*>& updatedRuleArea )
                       {
                           changed_rule_areas.insert( updatedRuleArea );
                       } );

        // If a SCH_RULE_AREA was changed, we need to add all past and present contained items to
        // update their connectivity
        for( const std::pair<SCH_RULE_AREA*, SCH_SCREEN*>& changedRuleArea : changed_rule_areas )
        {
            for( SCH_ITEM* containedItem : changedRuleArea.first->GetPastAndPresentContainedItems() )
                addItemToChangeSet( { containedItem, nullptr, changedRuleArea.second } );
        }

        // Add all changed items, and associated items, to the change set
        for( CHANGED_ITEM& changed_item_data : changed_connectable_items )
        {
            addItemToChangeSet( changed_item_data );

            // If a SCH_DIRECTIVE_LABEL was changed which is attached to a SCH_RULE_AREA, we need
            // to add the contained items to the change set to force update of their connectivity
            if( changed_item_data.item->Type() == SCH_DIRECTIVE_LABEL_T )
            {
                const std::vector<VECTOR2I> labelConnectionPoints =
                        changed_item_data.item->GetConnectionPoints();

                auto candidateRuleAreas =
                        changed_item_data.screen->Items().Overlapping( SCH_RULE_AREA_T,
                                                                       changed_item_data.item->GetBoundingBox() );

                for( SCH_ITEM* candidateRuleArea : candidateRuleAreas )
                {
                    SCH_RULE_AREA*      ruleArea = static_cast<SCH_RULE_AREA*>( candidateRuleArea );
                    std::vector<SHAPE*> borderShapes = ruleArea->MakeEffectiveShapes( true );

                    if( ruleArea->GetPolyShape().CollideEdge( labelConnectionPoints[0], nullptr, 5 ) )
                    {
                        for( SCH_ITEM* containedItem : ruleArea->GetPastAndPresentContainedItems() )
                            addItemToChangeSet( { containedItem, nullptr, changed_item_data.screen } );
                    }
                }
            }
        }

        for( const VECTOR2I& pt: pts )
        {
            for( SCH_ITEM* item : GetCurrentScreen()->Items().Overlapping( pt ) )
            {
                // Leave this check in place.  Overlapping items are not necessarily connectable.
                if( !item->IsConnectable() )
                    continue;

                if( item->Type() == SCH_LINE_T )
                {
                    if( item->HitTest( pt ) )
                        changed_items.insert( item );
                }
                else if( item->Type() == SCH_SYMBOL_T && item->IsConnected( pt ) )
                {
                    SCH_SYMBOL*           symbol = static_cast<SCH_SYMBOL*>( item );
                    std::vector<SCH_PIN*> pins = symbol->GetPins();

                    changed_items.insert( pins.begin(), pins.end() );
                }
                else if( item->Type() == SCH_SHEET_T )
                {
                    SCH_SHEET* sheet = static_cast<SCH_SHEET*>( item );

                    if( !sheet ) continue;

                    std::vector<SCH_SHEET_PIN*> sheetPins = sheet->GetPins();
                    changed_items.insert( sheetPins.begin(), sheetPins.end() );
                }
                else
                {
                    if( item->IsConnected( pt ) )
                        changed_items.insert( item );
                }
            }
        }

        std::set<std::pair<SCH_SHEET_PATH, SCH_ITEM*>> all_items =
                ConnectionGraph()->ExtractAffectedItems( changed_items );

        all_items.insert( item_paths.begin(), item_paths.end() );

        CONNECTION_GRAPH new_graph( this );

        new_graph.SetLastCodes( ConnectionGraph() );

        std::shared_ptr<NET_SETTINGS> netSettings = m_project->GetProjectFile().NetSettings();

        std::set<QString> affectedNets;

        for( auto&[ path, item ] : all_items )
        {
            if( !item ) continue;
            item->SetConnectivityDirty();
            SCH_CONNECTION* conn = item->Connection();

            if( conn )
                affectedNets.insert( conn->Name() );
        }

        // Reset resolved netclass cache for this connection
        for( const QString& netName : affectedNets )
            netSettings->ClearCacheForNet( netName );

        new_graph.Recalculate( list, false, aChangedItemHandler );
        ConnectionGraph()->Merge( new_graph );
    }

    // UNUSED_SYMBOL: SCH_COMMIT destructor - Commenting out localCommit usage
    // if( !localCommit.Empty() )
    //     localCommit.Push( _( "Schematic Cleanup" ) );
}