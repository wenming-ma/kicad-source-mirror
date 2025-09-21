
#include <algorithm>

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/QBuffer>
#include <QtCore/QDebug>

#include <base_units.h>
#include <bitmap_base.h>
#include <build_version.h>
#include <sch_selection.h>
#include <font/fontconfig.h>
#include <io/kicad/kicad_io_utils.h>
#include <locale_io.h>
#include <progress_reporter.h>
#include <schematic.h>
#include <schematic_lexer.h>
#include <sch_bitmap.h>
#include <sch_bus_entry.h>
#include <sch_edit_frame.h>       // SYMBOL_ORIENTATION_T
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr_common.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr_lib_cache.h>
#include <sch_io/kicad_sexpr/sch_io_kicad_sexpr_parser.h>
#include <sch_junction.h>
#include <sch_line.h>
#include <sch_no_connect.h>
#include <sch_pin.h>
#include <sch_rule_area.h>
#include <sch_screen.h>
#include <sch_shape.h>
#include <sch_sheet.h>
#include <sch_sheet_pin.h>
#include <sch_symbol.h>
#include <sch_table.h>
#include <sch_tablecell.h>
#include <sch_text.h>
#include <sch_textbox.h>
#include <string_utils.h>
#include <symbol_lib_table.h>  // for PropPowerSymsOnly definition.
#include <trace_helpers.h>

using namespace TSCHEMATIC_T;


#define SCH_PARSE_ERROR( text, reader, pos )                         \
    THROW_PARSE_ERROR( text, reader.GetSource(), reader.Line(),      \
                       reader.LineNumber(), pos - reader.Line() )


SCH_IO_KICAD_SEXPR::SCH_IO_KICAD_SEXPR() : SCH_IO( QStringLiteral( "Eeschema s-expression" ) )
{
    init( nullptr );
}


SCH_IO_KICAD_SEXPR::~SCH_IO_KICAD_SEXPR()
{
    delete m_cache;
}


void SCH_IO_KICAD_SEXPR::init( SCHEMATIC* aSchematic, const std::map<std::string, UTF8>* aProperties )
{
    m_version         = 0;
    m_appending       = false;
    m_rootSheet       = nullptr;
    m_schematic       = aSchematic;
    m_cache           = nullptr;
    m_out             = nullptr;
    m_nextFreeFieldId = 100; // number arbitrarily > MANDATORY_FIELD_COUNT or SHEET_MANDATORY_FIELD_COUNT
}


SCH_SHEET* SCH_IO_KICAD_SEXPR::LoadSchematicFile( const QString& aFileName, SCHEMATIC* aSchematic,
                                                  SCH_SHEET*             aAppendToMe,
                                                  const std::map<std::string, UTF8>* aProperties )
{
    Q_ASSERT( !aFileName.isEmpty() || aSchematic != nullptr );

    LOCALE_IO   toggle;     // toggles on, then off, the C locale.
    SCH_SHEET*  sheet;

    QFileInfo fn( aFileName );

    // Show the font substitution warnings
    fontconfig::FONTCONFIG::SetReporter( &WXLOG_REPORTER::GetInstance() );

    // Unfortunately child sheet file names the legacy schematic file format are not fully
    // qualified and are always appended to the project path.  The aFileName attribute must
    // always be an absolute path so the project path can be used for load child sheet files.
    Q_ASSERT( fn.isAbsolute() );

    if( aAppendToMe )
    {
        m_appending = true;
        qDebug() << "Append" << aFileName << "to sheet" << aAppendToMe->GetFileName();

        QFileInfo normedFn( aAppendToMe->GetFileName() );

        if( !normedFn.isAbsolute() )
        {
            if( aFileName.right( normedFn.filePath().length() ) == normedFn.filePath() )
                m_path = aFileName.left( aFileName.length() - normedFn.filePath().length() );
        }

        if( m_path.isEmpty() )
            m_path = aSchematic->Prj().GetProjectPath();

        qDebug() << "Normalized append path" << m_path;
    }
    else
    {
        m_path = aSchematic->Prj().GetProjectPath();
    }

    m_currentPath.push( m_path );
    init( aSchematic, aProperties );

    if( aAppendToMe == nullptr )
    {
        // Clean up any allocated memory if an exception occurs loading the schematic.
        std::unique_ptr<SCH_SHEET> newSheet = std::make_unique<SCH_SHEET>( aSchematic );

        QFileInfo relPath( aFileName );

        // Do not use absolute paths where relative paths suffice. Qt automatically handles
        // relative paths correctly on Windows and other platforms.
        QString relativePath = QDir( aSchematic->Prj().GetProjectPath() ).relativeFilePath( aFileName );

        newSheet->SetFileName( relativePath );
        m_rootSheet = newSheet.get();
        loadHierarchy( SCH_SHEET_PATH(), newSheet.get() );

        // If we got here, the schematic loaded successfully.
        sheet = newSheet.release();
        m_rootSheet = nullptr;         // Quiet Coverity warning.
    }
    else
    {
        Q_ASSERT_X( aSchematic->IsValid(), "LoadSchematicFile", "Can't append to a schematic with no root!" );
        m_rootSheet = &aSchematic->Root();
        sheet = aAppendToMe;
        loadHierarchy( SCH_SHEET_PATH(), sheet );
    }

    Q_ASSERT( m_currentPath.size() == 1 );  // only the project path should remain

    m_currentPath.pop(); // Clear the path stack for next call to Load

    return sheet;
}


// Everything below this comment is recursive.  Modify with care.

void SCH_IO_KICAD_SEXPR::loadHierarchy( const SCH_SHEET_PATH& aParentSheetPath, SCH_SHEET* aSheet )
{
    m_currentSheetPath.push_back( aSheet );

    SCH_SCREEN* screen = nullptr;

    if( !aSheet->GetScreen() )
    {
        // SCH_SCREEN objects store the full path and file name where the SCH_SHEET object only
        // stores the file name and extension.  Add the project path to the file name and
        // extension to compare when calling SCH_SHEET::SearchHierarchy().
        QFileInfo fileName( aSheet->GetFileName() );

        QString absolutePath;
        if( !fileName.isAbsolute() )
            absolutePath = QDir( m_currentPath.top() ).absoluteFilePath( aSheet->GetFileName() );
        else
            absolutePath = fileName.absoluteFilePath();

        // Save the current path so that it gets restored when descending and ascending the
        // sheet hierarchy which allows for sheet schematic files to be nested in folders
        // relative to the last path a schematic was loaded from.
        qDebug() << "Saving path" << m_currentPath.top();
        m_currentPath.push( fileName.path() );
        qDebug() << "Current path" << m_currentPath.top();
        qDebug() << "Loading" << absolutePath;

        SCH_SHEET_PATH ancestorSheetPath = aParentSheetPath;

        while( !ancestorSheetPath.empty() )
        {
            if( ancestorSheetPath.LastScreen()->GetFileName() == absolutePath )
            {
                if( !m_error.isEmpty() )
                    m_error += "\n";

                m_error += QString( "Could not load sheet '%1' because it already "
                                   "appears as a direct ancestor in the schematic "
                                   "hierarchy." ).arg( absolutePath );

                absolutePath = QString();

                break;
            }

            ancestorSheetPath.pop_back();
        }

        if( ancestorSheetPath.empty() )
        {
            // Existing schematics could be either in the root sheet path or the current sheet
            // load path so we have to check both.
            if( !m_rootSheet->SearchHierarchy( absolutePath, &screen ) )
                m_currentSheetPath.at( 0 )->SearchHierarchy( absolutePath, &screen );
        }

        if( screen )
        {
            aSheet->SetScreen( screen );
            aSheet->GetScreen()->SetParent( m_schematic );
            // Do not need to load the sub-sheets - this has already been done.
        }
        else
        {
            aSheet->SetScreen( new SCH_SCREEN( m_schematic ) );
            aSheet->GetScreen()->SetFileName( absolutePath );

            try
            {
                loadFile( absolutePath, aSheet );
            }
            catch( const IO_ERROR& ioe )
            {
                // If there is a problem loading the root sheet, there is no recovery.
                if( aSheet == m_rootSheet )
                    throw;

                // For all subsheets, queue up the error message for the caller.
                if( !m_error.isEmpty() )
                    m_error += "\n";

                m_error += ioe.What();
            }

            QFileInfo fileInfo( absolutePath );
            if( fileInfo.exists() )
            {
                aSheet->GetScreen()->SetFileReadOnly( !fileInfo.isWritable() );
                aSheet->GetScreen()->SetFileExists( true );
            }
            else
            {
                aSheet->GetScreen()->SetFileReadOnly( !fileInfo.dir().isReadable() );
                aSheet->GetScreen()->SetFileExists( false );
            }

            SCH_SHEET_PATH currentSheetPath = aParentSheetPath;
            currentSheetPath.push_back( aSheet );

            // This was moved out of the try{} block so that any sheet definitions that
            // the plugin fully parsed before the exception was raised will be loaded.
            for( SCH_ITEM* aItem : aSheet->GetScreen()->Items().OfType( SCH_SHEET_T ) )
            {
                Q_ASSERT( aItem->Type() == SCH_SHEET_T );
                SCH_SHEET* sheet = static_cast<SCH_SHEET*>( aItem );

                // Recursion starts here.
                loadHierarchy( currentSheetPath, sheet );
            }
        }

        m_currentPath.pop();
        qDebug() << "Restoring path" << m_currentPath.top();
    }

    m_currentSheetPath.pop_back();
}


void SCH_IO_KICAD_SEXPR::loadFile( const QString& aFileName, SCH_SHEET* aSheet )
{
    FILE_LINE_READER reader( aFileName );

    size_t lineCount = 0;

    if( m_progressReporter )
    {
        m_progressReporter->Report( QString( "Loading %1..." ).arg( aFileName ) );

        if( !m_progressReporter->KeepRefreshing() )
            THROW_IO_ERROR( "Open cancelled by user." );

        while( reader.ReadLine() )
            lineCount++;

        reader.Rewind();
    }

    SCH_IO_KICAD_SEXPR_PARSER parser( &reader, m_progressReporter, lineCount, m_rootSheet,
                                      m_appending );

    parser.ParseSchematic( aSheet );
}


void SCH_IO_KICAD_SEXPR::LoadContent( LINE_READER& aReader, SCH_SHEET* aSheet, int aFileVersion )
{
    Q_ASSERT( aSheet != nullptr );

    LOCALE_IO toggle;
    SCH_IO_KICAD_SEXPR_PARSER parser( &aReader );

    parser.ParseSchematic( aSheet, true, aFileVersion );
}


void SCH_IO_KICAD_SEXPR::SaveSchematicFile( const QString& aFileName, SCH_SHEET* aSheet,
                                            SCHEMATIC*             aSchematic,
                                            const std::map<std::string, UTF8>* aProperties )
{
    Q_ASSERT_X( aSheet != nullptr, "SaveSchematicFile", "NULL SCH_SHEET object." );
    Q_ASSERT_X( !aFileName.isEmpty(), "SaveSchematicFile", "No schematic file name defined." );

    LOCALE_IO   toggle;     // toggles on, then off, the C locale, to write floating point values.

    init( aSchematic, aProperties );

    QFileInfo fn( aFileName );

    // File names should be absolute.  Don't assume everything relative to the project path
    // works properly.
    Q_ASSERT( fn.isAbsolute() );

    PRETTIFIED_FILE_OUTPUTFORMATTER formatter( fn.absoluteFilePath() );

    m_out = &formatter;     // no ownership

    Format( aSheet );

    if( aSheet->GetScreen() )
        aSheet->GetScreen()->SetFileExists( true );
}


void SCH_IO_KICAD_SEXPR::Format( SCH_SHEET* aSheet )
{
    Q_ASSERT_X( aSheet != nullptr, "Format", "NULL SCH_SHEET* object." );
    Q_ASSERT_X( m_schematic != nullptr, "Format", "NULL SCHEMATIC* object." );

    SCH_SHEET_LIST sheets = m_schematic->Hierarchy();
    SCH_SCREEN* screen = aSheet->GetScreen();

    Q_ASSERT( screen != nullptr );

    // If we've requested to embed the fonts in the schematic, do so.
    // Otherwise, clear the embedded fonts from the schematic.  Embedded
    // fonts will be used if available
    if( m_schematic->GetAreFontsEmbedded() )
        m_schematic->EmbedFonts();
    else
        m_schematic->GetEmbeddedFiles()->ClearEmbeddedFonts();

    m_out->Print( "(kicad_sch (version %d) (generator \"eeschema\") (generator_version %s)",
                  SEXPR_SCHEMATIC_FILE_VERSION,
                  m_out->Quotew( GetMajorMinorVersion() ).c_str() );

    KICAD_FORMAT::FormatUuid( m_out, screen->m_uuid );

    screen->GetPageSettings().Format( m_out );
    screen->GetTitleBlock().Format( m_out );

    // Save cache library.
    m_out->Print( "(lib_symbols" );

    for( const auto& [ libItemName, libSymbol ] : screen->GetLibSymbols() )
        SCH_IO_KICAD_SEXPR_LIB_CACHE::SaveSymbol( libSymbol, *m_out, libItemName );

    m_out->Print( ")" );

    for( const std::shared_ptr<BUS_ALIAS>& alias : screen->GetBusAliases() )
        saveBusAlias( alias );

    // Enforce item ordering
    auto cmp =
            []( const SCH_ITEM* a, const SCH_ITEM* b )
            {
                if( a->Type() != b->Type() )
                    return a->Type() < b->Type();

                // sorting fix specific for 9.0 to avoid format change
                if( a->Type() == SCH_TABLE_T && b->Type() == SCH_TABLE_T )
                {
                    const SCH_TABLE* aTable = static_cast<const SCH_TABLE*>( a );
                    const SCH_TABLE* bTable = static_cast<const SCH_TABLE*>( b );

                    return aTable->GetHash() < bTable->GetHash();
                }

                return a->m_Uuid < b->m_Uuid;
            };

    std::multiset<SCH_ITEM*, decltype( cmp )> save_map( cmp );

    for( SCH_ITEM* item : screen->Items() )
    {
        // Markers are not saved, so keep them from being considered below
        if( item->Type() != SCH_MARKER_T )
            save_map.insert( item );
    }

    for( SCH_ITEM* item : save_map )
    {
        switch( item->Type() )
        {
        case SCH_SYMBOL_T:
            saveSymbol( static_cast<SCH_SYMBOL*>( item ), *m_schematic, sheets, false );
            break;

        case SCH_BITMAP_T:
            saveBitmap( static_cast<SCH_BITMAP&>( *item ) );
            break;

        case SCH_SHEET_T:
            saveSheet( static_cast<SCH_SHEET*>( item ), sheets );
            break;

        case SCH_JUNCTION_T:
            saveJunction( static_cast<SCH_JUNCTION*>( item ) );
            break;

        case SCH_NO_CONNECT_T:
            saveNoConnect( static_cast<SCH_NO_CONNECT*>( item ) );
            break;

        case SCH_BUS_WIRE_ENTRY_T:
        case SCH_BUS_BUS_ENTRY_T:
            saveBusEntry( static_cast<SCH_BUS_ENTRY_BASE*>( item ) );
            break;

        case SCH_LINE_T:
            saveLine( static_cast<SCH_LINE*>( item ) );
            break;

        case SCH_SHAPE_T:
            saveShape( static_cast<SCH_SHAPE*>( item ) );
            break;

        case SCH_RULE_AREA_T:
            saveRuleArea( static_cast<SCH_RULE_AREA*>( item ) );
            break;

        case SCH_TEXT_T:
        case SCH_LABEL_T:
        case SCH_GLOBAL_LABEL_T:
        case SCH_HIER_LABEL_T:
        case SCH_DIRECTIVE_LABEL_T:
            saveText( static_cast<SCH_TEXT*>( item ) );
            break;

        case SCH_TEXTBOX_T:
            saveTextBox( static_cast<SCH_TEXTBOX*>( item ) );
            break;

        case SCH_TABLE_T:
            saveTable( static_cast<SCH_TABLE*>( item ) );
            break;

        default:
            Q_ASSERT_X( false, "Format", "Unexpected schematic object type in SCH_IO_KICAD_SEXPR::Format()" );
        }
    }

    if( aSheet->HasRootInstance() )
    {
        std::vector< SCH_SHEET_INSTANCE> instances;

        instances.emplace_back( aSheet->GetRootInstance() );
        saveInstances( instances );

        KICAD_FORMAT::FormatBool( m_out, "embedded_fonts", m_schematic->GetAreFontsEmbedded() );

        // Save any embedded files
        if( !m_schematic->GetEmbeddedFiles()->IsEmpty() )
            m_schematic->WriteEmbeddedFiles( *m_out, true );
    }

    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::Format( SCH_SELECTION* aSelection, SCH_SHEET_PATH* aSelectionPath,
                                 SCHEMATIC& aSchematic, OUTPUTFORMATTER* aFormatter,
                                 bool aForClipboard )
{
    Q_ASSERT( aSelection && aSelectionPath && aFormatter );

    LOCALE_IO toggle;
    SCH_SHEET_LIST sheets = aSchematic.Hierarchy();

    m_schematic = &aSchematic;
    m_out = aFormatter;

    std::map<QString, LIB_SYMBOL*> libSymbols;
    SCH_SCREEN*                     screen = aSelection->GetScreen();
    std::set<SCH_TABLE*>            promotedTables;

    for( EDA_ITEM* item : *aSelection )
    {
        if( item->Type() != SCH_SYMBOL_T )
            continue;

        SCH_SYMBOL* symbol = static_cast<SCH_SYMBOL*>( item );

        QString libSymbolLookup = QString::fromStdString( symbol->GetLibId().Format() );

        if( !symbol->UseLibIdLookup() )
            libSymbolLookup = symbol->GetSchSymbolLibraryName();

        auto it = screen->GetLibSymbols().find( libSymbolLookup );

        if( it != screen->GetLibSymbols().end() )
            libSymbols[ libSymbolLookup ] = it->second;
    }

    if( !libSymbols.empty() )
    {
        m_out->Print( "(lib_symbols" );

        for( const auto& [name, libSymbol] : libSymbols )
            SCH_IO_KICAD_SEXPR_LIB_CACHE::SaveSymbol( libSymbol, *m_out, name, false );

        m_out->Print( ")" );
    }

    for( EDA_ITEM* edaItem : *aSelection )
    {
        if( !edaItem->IsSCH_ITEM() )
            continue;

        SCH_ITEM* item = static_cast<SCH_ITEM*>( edaItem );

        switch( item->Type() )
        {
        case SCH_SYMBOL_T:
            saveSymbol( static_cast<SCH_SYMBOL*>( item ), aSchematic, sheets, aForClipboard, aSelectionPath );
            break;

        case SCH_BITMAP_T:
            saveBitmap( static_cast<SCH_BITMAP&>( *item ) );
            break;

        case SCH_SHEET_T:
            saveSheet( static_cast<SCH_SHEET*>( item ), sheets );
            break;

        case SCH_JUNCTION_T:
            saveJunction( static_cast<SCH_JUNCTION*>( item ) );
            break;

        case SCH_NO_CONNECT_T:
            saveNoConnect( static_cast<SCH_NO_CONNECT*>( item ) );
            break;

        case SCH_BUS_WIRE_ENTRY_T:
        case SCH_BUS_BUS_ENTRY_T:
            saveBusEntry( static_cast<SCH_BUS_ENTRY_BASE*>( item ) );
            break;

        case SCH_LINE_T:
            saveLine( static_cast<SCH_LINE*>( item ) );
            break;

        case SCH_SHAPE_T:
            saveShape( static_cast<SCH_SHAPE*>( item ) );
            break;

        case SCH_RULE_AREA_T:
            saveRuleArea( static_cast<SCH_RULE_AREA*>( item ) );
            break;

        case SCH_TEXT_T:
        case SCH_LABEL_T:
        case SCH_GLOBAL_LABEL_T:
        case SCH_HIER_LABEL_T:
        case SCH_DIRECTIVE_LABEL_T:
            saveText( static_cast<SCH_TEXT*>( item ) );
            break;

        case SCH_TEXTBOX_T:
            saveTextBox( static_cast<SCH_TEXTBOX*>( item ) );
            break;

        case SCH_TABLECELL_T:
        {
            SCH_TABLE* table = static_cast<SCH_TABLE*>( item->GetParent() );

            if( promotedTables.count( table ) )
                break;

            table->SetFlags( SKIP_STRUCT );
            saveTable( table );
            table->ClearFlags( SKIP_STRUCT );
            promotedTables.insert( table );
            break;
        }

        case SCH_TABLE_T:
            item->ClearFlags( SKIP_STRUCT );
            saveTable( static_cast<SCH_TABLE*>( item ) );
            break;

        default:
            Q_ASSERT_X( false, "Format", "Unexpected schematic object type in SCH_IO_KICAD_SEXPR::Format()" );
        }
    }
}


void SCH_IO_KICAD_SEXPR::saveSymbol( SCH_SYMBOL* aSymbol, const SCHEMATIC& aSchematic,
                                     const SCH_SHEET_LIST& aSheetList, bool aForClipboard,
                                     const SCH_SHEET_PATH* aRelativePath )
{
    Q_ASSERT_X( aSymbol != nullptr && m_out != nullptr, "saveSymbol", "Invalid parameters" );

    std::string     libName;

    QString symbol_name = QString::fromStdString( aSymbol->GetLibId().Format() );

    if( symbol_name.size() )
    {
        libName = toUTFTildaText( symbol_name.toStdString() );
    }
    else
    {
        libName = "_NONAME_";
    }

    EDA_ANGLE angle;
    int       orientation = aSymbol->GetOrientation() & ~( SYM_MIRROR_X | SYM_MIRROR_Y );

    if( orientation == SYM_ORIENT_90 )
        angle = ANGLE_90;
    else if( orientation == SYM_ORIENT_180 )
        angle = ANGLE_180;
    else if( orientation == SYM_ORIENT_270 )
        angle = ANGLE_270;
    else
        angle = ANGLE_0;

    m_out->Print( "(symbol" );

    if( !aSymbol->UseLibIdLookup() )
    {
        m_out->Print( "(lib_name %s)",
                      m_out->Quotew( aSymbol->GetSchSymbolLibraryName() ).c_str() );
    }

    m_out->Print( "(lib_id %s) (at %s %s %s)",
                  m_out->Quotew( QString::fromStdString( aSymbol->GetLibId().Format() ) ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSymbol->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSymbol->GetPosition().y ).c_str(),
                  EDA_UNIT_UTILS::FormatAngle( angle ).c_str() );

    bool mirrorX = aSymbol->GetOrientation() & SYM_MIRROR_X;
    bool mirrorY = aSymbol->GetOrientation() & SYM_MIRROR_Y;

    if( mirrorX || mirrorY )
    {
        m_out->Print( "(mirror %s %s)",
                      mirrorX ? "x" : "",
                      mirrorY ? "y" : "" );
    }

    // The symbol unit is always set to the ordianal instance regardless of the current sheet
    // instance to prevent file churn.
    SCH_SYMBOL_INSTANCE ordinalInstance;

    ordinalInstance.m_Reference = aSymbol->GetPrefix();

    const SCH_SCREEN* parentScreen = static_cast<const SCH_SCREEN*>( aSymbol->GetParent() );

    Q_ASSERT( parentScreen );

    if( parentScreen && m_schematic )
    {
        std::optional<SCH_SHEET_PATH> ordinalPath =
                m_schematic->Hierarchy().GetOrdinalPath( parentScreen );

        // Design blocks are saved from a temporary sheet & screen which will not be found in
        // the schematic, and will therefore have no ordinal path.
        // Q_ASSERT( ordinalPath );

        if( ordinalPath )
            aSymbol->GetInstance( ordinalInstance, ordinalPath->Path() );
        else if( aSymbol->GetInstances().size() )
            ordinalInstance = aSymbol->GetInstances()[0];
    }

    int unit = ordinalInstance.m_Unit;

    if( aForClipboard && aRelativePath )
    {
        SCH_SYMBOL_INSTANCE unitInstance;

        if( aSymbol->GetInstance( unitInstance, aRelativePath->Path() ) )
            unit = unitInstance.m_Unit;
    }

    m_out->Print( "(unit %d)", unit );

    if( aSymbol->GetBodyStyle() == BODY_STYLE::DEMORGAN )
        m_out->Print( "(convert %d)", aSymbol->GetBodyStyle() );

    KICAD_FORMAT::FormatBool( m_out, "exclude_from_sim", aSymbol->GetExcludedFromSim() );
    KICAD_FORMAT::FormatBool( m_out, "in_bom", !aSymbol->GetExcludedFromBOM() );
    KICAD_FORMAT::FormatBool( m_out, "on_board", !aSymbol->GetExcludedFromBoard() );
    KICAD_FORMAT::FormatBool( m_out, "dnp", aSymbol->GetDNP() );

    AUTOPLACE_ALGO fieldsAutoplaced = aSymbol->GetFieldsAutoplaced();

    if( fieldsAutoplaced == AUTOPLACE_AUTO || fieldsAutoplaced == AUTOPLACE_MANUAL )
        KICAD_FORMAT::FormatBool( m_out, "fields_autoplaced", true );

    KICAD_FORMAT::FormatUuid( m_out, aSymbol->m_Uuid );

    m_nextFreeFieldId = MANDATORY_FIELD_COUNT;

    for( SCH_FIELD& field : aSymbol->GetFields() )
    {
        int id = field.GetId();
        QString value = field.GetText();

        if( !aForClipboard && aSymbol->GetInstances().size() )
        {
            // The instance fields are always set to the default instance regardless of the
            // sheet instance to prevent file churn.
            if( id == REFERENCE_FIELD )
            {
                field.SetText( ordinalInstance.m_Reference );
            }
            else if( id == VALUE_FIELD )
            {
                field.SetText( aSymbol->GetField( VALUE_FIELD )->GetText() );
            }
            else if( id == FOOTPRINT_FIELD )
            {
                field.SetText( aSymbol->GetField( FOOTPRINT_FIELD )->GetText() );
            }
        }
        else if( aForClipboard && aSymbol->GetInstances().size() && aRelativePath
               && ( id == REFERENCE_FIELD ) )
        {
            SCH_SYMBOL_INSTANCE instance;

            if( aSymbol->GetInstance( instance, aRelativePath->Path() ) )
                field.SetText( instance.m_Reference );
        }

        try
        {
            saveField( &field );
        }
        catch( ... )
        {
            // Restore the changed field text on write error.
            if( id == REFERENCE_FIELD || id == VALUE_FIELD || id == FOOTPRINT_FIELD )
                field.SetText( value );

            throw;
        }

        if( id == REFERENCE_FIELD || id == VALUE_FIELD || id == FOOTPRINT_FIELD )
            field.SetText( value );
    }

    for( const std::unique_ptr<SCH_PIN>& pin : aSymbol->GetRawPins() )
    {
        // There was a bug introduced somewhere in the original alternated pin code that would
        // set the alternate pin to the default pin name which caused a number of library symbol
        // comparison issues.  Clearing the alternate pin resolves this issue.
        if( pin->GetAlt().IsEmpty() || ( pin->GetAlt() == pin->GetBaseName() ) )
        {
            m_out->Print( "(pin %s", m_out->Quotew( pin->GetNumber() ).c_str() );
            KICAD_FORMAT::FormatUuid( m_out, pin->m_Uuid );
            m_out->Print( ")" );
        }
        else
        {
            m_out->Print( "(pin %s", m_out->Quotew( pin->GetNumber() ).c_str() );
            KICAD_FORMAT::FormatUuid( m_out, pin->m_Uuid );
            m_out->Print( "(alternate %s))", m_out->Quotew( pin->GetAlt() ).c_str() );
        }
    }

    if( !aSymbol->GetInstances().empty() )
    {
        std::map<KIID, std::vector<SCH_SYMBOL_INSTANCE>> projectInstances;

        m_out->Print( "(instances" );

        QString projectName;
        KIID     rootSheetUuid = aSchematic.Root().m_Uuid;

        for( const SCH_SYMBOL_INSTANCE& inst : aSymbol->GetInstances() )
        {
            // Zero length KIID_PATH objects are not valid and will cause a crash below.
            if( !inst.m_Path.size() ) continue;

            // If the instance data is part of this design but no longer has an associated sheet
            // path, don't save it.  This prevents large amounts of orphaned instance data for the
            // current project from accumulating in the schematic files.
            bool isOrphaned = ( inst.m_Path[0] == rootSheetUuid )
                              && !aSheetList.GetSheetPathByKIIDPath( inst.m_Path );

            // Keep all instance data when copying to the clipboard.  They may be needed on paste.
            if( !aForClipboard && isOrphaned )
                continue;

            auto it = projectInstances.find( inst.m_Path[0] );

            if( it == projectInstances.end() )
                projectInstances[ inst.m_Path[0] ] = { inst };
            else
                it->second.emplace_back( inst );
        }

        for( auto& [uuid, instances] : projectInstances )
        {
            Q_ASSERT( instances.size() > 0 );

            // Sort project instances by KIID_PATH.
            std::sort( instances.begin(), instances.end(),
                       []( SCH_SYMBOL_INSTANCE& aLhs, SCH_SYMBOL_INSTANCE& aRhs )
                       {
                           return aLhs.m_Path < aRhs.m_Path;
                       } );

            projectName = instances[0].m_ProjectName;

            m_out->Print( "(project %s", m_out->Quotew( projectName ).c_str() );

            for( const SCH_SYMBOL_INSTANCE& instance : instances )
            {
                QString path;
                KIID_PATH tmp = instance.m_Path;

                if( aForClipboard && aRelativePath )
                    tmp.MakeRelativeTo( aRelativePath->Path() );

                path = QString::fromStdString( tmp.AsString() );

                m_out->Print( "(path %s (reference %s) (unit %d))",
                              m_out->Quotew( path ).c_str(),
                              m_out->Quotew( instance.m_Reference ).c_str(),
                              instance.m_Unit );
            }

            m_out->Print( ")" );  // Closes `project`.
        }

        m_out->Print( ")" );  // Closes `instances`.
    }

    m_out->Print( ")" );      // Closes `symbol`.
}


void SCH_IO_KICAD_SEXPR::saveField( SCH_FIELD* aField )
{
    Q_ASSERT_X( aField != nullptr && m_out != nullptr, "saveField", "Invalid parameters" );

    QString fieldName = aField->GetCanonicalName();
    // For some reason (bug in legacy parser?) the field ID for non-mandatory fields is -1 so
    // check for this in order to correctly use the field name.

    if( aField->GetId() == -1 /* undefined ID */ )
    {
        // Replace the default name built by GetCanonicalName() by
        // the field name if exists
        if( !aField->GetName().isEmpty() )
            fieldName = aField->GetName();

        aField->SetId( m_nextFreeFieldId );
        m_nextFreeFieldId += 1;
    }
    else if( aField->GetId() >= m_nextFreeFieldId )
    {
        m_nextFreeFieldId = aField->GetId() + 1;
    }

    m_out->Print( "(property %s %s %s (at %s %s %s)",
                  aField->IsPrivate() ? "private" : "",
                  m_out->Quotew( fieldName ).c_str(),
                  m_out->Quotew( aField->GetText() ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aField->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aField->GetPosition().y ).c_str(),
                  EDA_UNIT_UTILS::FormatAngle( aField->GetTextAngle() ).c_str() );

    if( aField->IsNameShown() )
        KICAD_FORMAT::FormatBool( m_out, "show_name", true );

    if( !aField->CanAutoplace() )
        KICAD_FORMAT::FormatBool( m_out, "do_not_autoplace", true );

    if( !aField->IsDefaultFormatting()
            || ( aField->GetTextHeight() != schIUScale.MilsToIU( DEFAULT_SIZE_TEXT ) ) )
    {
        aField->Format( m_out, 0 );
    }

    m_out->Print( ")" );            // Closes `property` token
}


void SCH_IO_KICAD_SEXPR::saveBitmap( const SCH_BITMAP& aBitmap )
{
    Q_ASSERT_X( m_out != nullptr, "saveBitmap", "Invalid output formatter" );

    const REFERENCE_IMAGE& refImage = aBitmap.GetReferenceImage();
    const BITMAP_BASE&     bitmapBase = refImage.GetImage();

    const QImage* image = bitmapBase.GetImageData();

    Q_ASSERT_X( image != nullptr, "saveBitmap", "QImage* is NULL" );

    m_out->Print( "(image (at %s %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       refImage.GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       refImage.GetPosition().y ).c_str() );

    double scale = refImage.GetImageScale();

    // 20230121 or older file format versions assumed 300 image PPI at load/save.
    // Let's keep compatibility by changing image scale.
    if( SEXPR_SCHEMATIC_FILE_VERSION <= 20230121 )
        scale = scale * 300.0 / bitmapBase.GetPPI();

    if( scale != 1.0 )
        m_out->Print( "(scale %g)", scale );

    KICAD_FORMAT::FormatUuid( m_out, aBitmap.m_Uuid );

    QBuffer buffer;
    buffer.open( QIODevice::WriteOnly );
    bitmapBase.SaveImageData( buffer );

    KICAD_FORMAT::FormatStreamData( *m_out, *buffer.buffer().data() );

    m_out->Print( ")" );        // Closes image token.
}


void SCH_IO_KICAD_SEXPR::saveSheet( SCH_SHEET* aSheet, const SCH_SHEET_LIST& aSheetList )
{
    Q_ASSERT_X( aSheet != nullptr && m_out != nullptr, "saveSheet", "Invalid parameters" );

    m_out->Print( "(sheet (at %s %s) (size %s %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSheet->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSheet->GetPosition().y ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSheet->GetSize().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aSheet->GetSize().y ).c_str() );

    KICAD_FORMAT::FormatBool( m_out, "exclude_from_sim", aSheet->GetExcludedFromSim() );
    KICAD_FORMAT::FormatBool( m_out, "in_bom", !aSheet->GetExcludedFromBOM() );
    KICAD_FORMAT::FormatBool( m_out, "on_board", !aSheet->GetExcludedFromBoard() );
    KICAD_FORMAT::FormatBool( m_out, "dnp", aSheet->GetDNP() );

    AUTOPLACE_ALGO fieldsAutoplaced = aSheet->GetFieldsAutoplaced();

    if( fieldsAutoplaced == AUTOPLACE_AUTO || fieldsAutoplaced == AUTOPLACE_MANUAL )
        KICAD_FORMAT::FormatBool( m_out, "fields_autoplaced", true );

    STROKE_PARAMS stroke( aSheet->GetBorderWidth(), LINE_STYLE::SOLID, aSheet->GetBorderColor() );

    stroke.SetWidth( aSheet->GetBorderWidth() );
    stroke.Format( m_out, schIUScale );

    m_out->Print( "(fill (color %d %d %d %0.4f))",
                  KiROUND( aSheet->GetBackgroundColor().r * 255.0 ),
                  KiROUND( aSheet->GetBackgroundColor().g * 255.0 ),
                  KiROUND( aSheet->GetBackgroundColor().b * 255.0 ),
                  aSheet->GetBackgroundColor().a );

    KICAD_FORMAT::FormatUuid( m_out, aSheet->m_Uuid );

    m_nextFreeFieldId = SHEET_MANDATORY_FIELD_COUNT;

    for( SCH_FIELD& field : aSheet->GetFields() )
        saveField( &field );

    for( const SCH_SHEET_PIN* pin : aSheet->GetPins() )
    {
        m_out->Print( "(pin %s %s (at %s %s %s)",
                      EscapedUTF8( pin->GetText() ).c_str(),
                      getSheetPinShapeToken( pin->GetShape() ),
                      EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                           pin->GetPosition().x ).c_str(),
                      EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                           pin->GetPosition().y ).c_str(),
                      EDA_UNIT_UTILS::FormatAngle( getSheetPinAngle( pin->GetSide() ) ).c_str() );

        KICAD_FORMAT::FormatUuid( m_out, pin->m_Uuid );

        pin->Format( m_out, 0 );

        m_out->Print( ")" );  // Closes pin token.
    }

    // Save all sheet instances here except the root sheet instance.
    std::vector< SCH_SHEET_INSTANCE > sheetInstances = aSheet->GetInstances();

    auto it = sheetInstances.begin();

    while( it != sheetInstances.end() )
    {
        if( it->m_Path.size() == 0 )
            it = sheetInstances.erase( it );
        else
            it++;
    }

    if( !sheetInstances.empty() )
    {
        m_out->Print( "(instances" );

        KIID lastProjectUuid;
        KIID rootSheetUuid = m_schematic->Root().m_Uuid;
        bool inProjectClause = false;

        for( size_t i = 0; i < sheetInstances.size(); i++ )
        {
            // If the instance data is part of this design but no longer has an associated sheet
            // path, don't save it.  This prevents large amounts of orphaned instance data for the
            // current project from accumulating in the schematic files.
            //
            // Keep all instance data when copying to the clipboard.  It may be needed on paste.
            if( ( sheetInstances[i].m_Path[0] == rootSheetUuid )
                    && !aSheetList.GetSheetPathByKIIDPath( sheetInstances[i].m_Path, false ) )
            {
                if( inProjectClause && ( ( i + 1 == sheetInstances.size() )
                        || lastProjectUuid != sheetInstances[i+1].m_Path[0] ) )
                {
                    m_out->Print( ")" );  // Closes `project` token.
                    inProjectClause = false;
                }

                continue;
            }

            if( lastProjectUuid != sheetInstances[i].m_Path[0] )
            {
                QString projectName;

                if( sheetInstances[i].m_Path[0] == rootSheetUuid )
                    projectName = m_schematic->Prj().GetProjectName();
                else
                    projectName = sheetInstances[i].m_ProjectName;

                lastProjectUuid = sheetInstances[i].m_Path[0];
                m_out->Print( "(project %s", m_out->Quotew( projectName ).c_str() );
                inProjectClause = true;
            }

            QString path = QString::fromStdString( sheetInstances[i].m_Path.AsString() );

            m_out->Print( "(path %s (page %s))",
                          m_out->Quotew( path ).c_str(),
                          m_out->Quotew( sheetInstances[i].m_PageNumber ).c_str() );

            if( inProjectClause && ( ( i + 1 == sheetInstances.size() )
                    || lastProjectUuid != sheetInstances[i+1].m_Path[0] ) )
            {
                m_out->Print( ")" );  // Closes `project` token.
                inProjectClause = false;
            }
        }

        m_out->Print( ")" );        // Closes `instances` token.
    }

    m_out->Print( ")" );          // Closes sheet token.
}


void SCH_IO_KICAD_SEXPR::saveJunction( SCH_JUNCTION* aJunction )
{
    Q_ASSERT_X( aJunction != nullptr && m_out != nullptr, "saveJunction", "Invalid parameters" );

    m_out->Print( "(junction (at %s %s) (diameter %s) (color %d %d %d %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aJunction->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aJunction->GetPosition().y ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aJunction->GetDiameter() ).c_str(),
                  KiROUND( aJunction->GetColor().r * 255.0 ),
                  KiROUND( aJunction->GetColor().g * 255.0 ),
                  KiROUND( aJunction->GetColor().b * 255.0 ),
                  FormatDouble2Str( aJunction->GetColor().a ).c_str() );

    KICAD_FORMAT::FormatUuid( m_out, aJunction->m_Uuid );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveNoConnect( SCH_NO_CONNECT* aNoConnect )
{
    Q_ASSERT_X( aNoConnect != nullptr && m_out != nullptr, "saveNoConnect", "Invalid parameters" );

    m_out->Print( "(no_connect (at %s %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aNoConnect->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aNoConnect->GetPosition().y ).c_str() );

    KICAD_FORMAT::FormatUuid( m_out, aNoConnect->m_Uuid );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveBusEntry( SCH_BUS_ENTRY_BASE* aBusEntry )
{
    Q_ASSERT_X( aBusEntry != nullptr && m_out != nullptr, "saveBusEntry", "Invalid parameters" );

    // Bus to bus entries are converted to bus line segments.
    if( aBusEntry->GetClass() == "SCH_BUS_BUS_ENTRY" )
    {
        SCH_LINE busEntryLine( aBusEntry->GetPosition(), LAYER_BUS );

        busEntryLine.SetEndPoint( aBusEntry->GetEnd() );
        saveLine( &busEntryLine );
        return;
    }

    m_out->Print( "(bus_entry (at %s %s) (size %s %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aBusEntry->GetPosition().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aBusEntry->GetPosition().y ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aBusEntry->GetSize().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aBusEntry->GetSize().y ).c_str() );

    aBusEntry->GetStroke().Format( m_out, schIUScale );
    KICAD_FORMAT::FormatUuid( m_out, aBusEntry->m_Uuid );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveShape( SCH_SHAPE* aShape )
{
    Q_ASSERT_X( aShape != nullptr && m_out != nullptr, "saveShape", "Invalid parameters" );

    switch( aShape->GetShape() )
    {
    case SHAPE_T::ARC:
        formatArc( m_out, aShape, false, aShape->GetStroke(), aShape->GetFillMode(),
                   aShape->GetFillColor(), false, aShape->m_Uuid );
        break;

    case SHAPE_T::CIRCLE:
        formatCircle( m_out, aShape, false, aShape->GetStroke(), aShape->GetFillMode(),
                      aShape->GetFillColor(), false, aShape->m_Uuid );
        break;

    case SHAPE_T::RECTANGLE:
        formatRect( m_out, aShape, false, aShape->GetStroke(), aShape->GetFillMode(),
                    aShape->GetFillColor(), false, aShape->m_Uuid );
        break;

    case SHAPE_T::BEZIER:
        formatBezier( m_out, aShape, false, aShape->GetStroke(), aShape->GetFillMode(),
                      aShape->GetFillColor(), false, aShape->m_Uuid );
        break;

    case SHAPE_T::POLY:
        formatPoly( m_out, aShape, false, aShape->GetStroke(), aShape->GetFillMode(),
                    aShape->GetFillColor(), false, aShape->m_Uuid );
        break;

    default:
        UNIMPLEMENTED_FOR( aShape->SHAPE_T_asString() );
    }
}


void SCH_IO_KICAD_SEXPR::saveRuleArea( SCH_RULE_AREA* aRuleArea )
{
    Q_ASSERT_X( aRuleArea != nullptr && m_out != nullptr, "saveRuleArea", "Invalid parameters" );

    m_out->Print( "(rule_area " );
    saveShape( aRuleArea );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveLine( SCH_LINE* aLine )
{
    Q_ASSERT_X( aLine != nullptr && m_out != nullptr, "saveLine", "Invalid parameters" );

    QString lineType;

    STROKE_PARAMS line_stroke = aLine->GetStroke();

    switch( aLine->GetLayer() )
    {
    case LAYER_BUS:     lineType = "bus";       break;
    case LAYER_WIRE:    lineType = "wire";      break;
    case LAYER_NOTES:   lineType = "polyline";  break;
    default:
        UNIMPLEMENTED_FOR( LayerName( aLine->GetLayer() ) );
    }

    m_out->Print( "(%s (pts (xy %s %s) (xy %s %s))",
                  lineType.toUtf8().constData(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aLine->GetStartPoint().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aLine->GetStartPoint().y ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aLine->GetEndPoint().x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                       aLine->GetEndPoint().y ).c_str() );

    line_stroke.Format( m_out, schIUScale );
    KICAD_FORMAT::FormatUuid( m_out, aLine->m_Uuid );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveText( SCH_TEXT* aText )
{
    Q_ASSERT_X( aText != nullptr && m_out != nullptr, "saveText", "Invalid parameters" );

    // Note: label is nullptr SCH_TEXT, but not for SCH_LABEL_XXX,
    SCH_LABEL_BASE* label = dynamic_cast<SCH_LABEL_BASE*>( aText );

    m_out->Print( "(%s %s",
                  getTextTypeToken( aText->Type() ),
                  m_out->Quotew( aText->GetText() ).c_str() );

    if( aText->Type() == SCH_TEXT_T )
        KICAD_FORMAT::FormatBool( m_out, "exclude_from_sim", aText->GetExcludedFromSim() );

    if( aText->Type() == SCH_DIRECTIVE_LABEL_T )
    {
        SCH_DIRECTIVE_LABEL* flag = static_cast<SCH_DIRECTIVE_LABEL*>( aText );

        m_out->Print( "(length %s)",
                      EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                           flag->GetPinLength() ).c_str() );
    }

    EDA_ANGLE angle = aText->GetTextAngle();

    if( label )
    {
        if( label->Type() == SCH_GLOBAL_LABEL_T
                || label->Type() == SCH_HIER_LABEL_T
                || label->Type() == SCH_DIRECTIVE_LABEL_T )
        {
            m_out->Print( "(shape %s)", getSheetPinShapeToken( label->GetShape() ) );
        }

        // The angle of the text is always 0 or 90 degrees for readibility reasons,
        // but the item itself can have more rotation (-90 and 180 deg)
        switch( label->GetSpinStyle() )
        {
        default:
        case SPIN_STYLE::LEFT:   angle += ANGLE_180; break;
        case SPIN_STYLE::UP:                         break;
        case SPIN_STYLE::RIGHT:                      break;
        case SPIN_STYLE::BOTTOM: angle += ANGLE_180; break;
        }
    }

    m_out->Print( "(at %s %s %s)",
                    EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                        aText->GetPosition().x ).c_str(),
                    EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                        aText->GetPosition().y ).c_str(),
                    EDA_UNIT_UTILS::FormatAngle( angle ).c_str() );

    if( label && !label->GetFields().empty() )
    {
        AUTOPLACE_ALGO fieldsAutoplaced = label->GetFieldsAutoplaced();

        if( fieldsAutoplaced == AUTOPLACE_AUTO || fieldsAutoplaced == AUTOPLACE_MANUAL )
            KICAD_FORMAT::FormatBool( m_out, "fields_autoplaced", true );
    }

    aText->EDA_TEXT::Format( m_out, 0 );
    KICAD_FORMAT::FormatUuid( m_out, aText->m_Uuid );

    if( label && label->Type() == SCH_GLOBAL_LABEL_T )
        m_nextFreeFieldId = GLOBALLABEL_MANDATORY_FIELD_COUNT;
    else
        m_nextFreeFieldId = 0;

    if( label )
    {
        for( SCH_FIELD& field : label->GetFields() )
            saveField( &field );
    }

    m_out->Print( ")" );   // Closes text token.
}


void SCH_IO_KICAD_SEXPR::saveTextBox( SCH_TEXTBOX* aTextBox )
{
    Q_ASSERT_X( aTextBox != nullptr && m_out != nullptr, "saveTextBox", "Invalid parameters" );

    m_out->Print( "(%s %s",
                  aTextBox->Type() == SCH_TABLECELL_T ? "table_cell" : "text_box",
                  m_out->Quotew( aTextBox->GetText() ).c_str() );

    KICAD_FORMAT::FormatBool( m_out, "exclude_from_sim", aTextBox->GetExcludedFromSim() );

    VECTOR2I pos = aTextBox->GetStart();
    VECTOR2I size = aTextBox->GetEnd() - pos;

    m_out->Print( "(at %s %s %s) (size %s %s) (margins %s %s %s %s)",
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, pos.x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, pos.y ).c_str(),
                  EDA_UNIT_UTILS::FormatAngle( aTextBox->GetTextAngle() ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, size.x ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, size.y ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, aTextBox->GetMarginLeft() ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, aTextBox->GetMarginTop() ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, aTextBox->GetMarginRight() ).c_str(),
                  EDA_UNIT_UTILS::FormatInternalUnits( schIUScale, aTextBox->GetMarginBottom() ).c_str() );

    if( SCH_TABLECELL* cell = dynamic_cast<SCH_TABLECELL*>( aTextBox ) )
        m_out->Print( "(span %d %d)", cell->GetColSpan(), cell->GetRowSpan() );

    if( aTextBox->Type() != SCH_TABLECELL_T )
        aTextBox->GetStroke().Format( m_out, schIUScale );

    formatFill( m_out, aTextBox->GetFillMode(), aTextBox->GetFillColor() );
    aTextBox->EDA_TEXT::Format( m_out, 0 );
    KICAD_FORMAT::FormatUuid( m_out, aTextBox->m_Uuid );
    m_out->Print( ")" );
}


void SCH_IO_KICAD_SEXPR::saveTable( SCH_TABLE* aTable )
{
    if( aTable->GetFlags() & SKIP_STRUCT )
    {
        aTable = static_cast<SCH_TABLE*>( aTable->Clone() );

        int minCol = aTable->GetColCount();
        int maxCol = -1;
        int minRow = aTable->GetRowCount();
        int maxRow = -1;

        for( int row = 0; row < aTable->GetRowCount(); ++row )
        {
            for( int col = 0; col < aTable->GetColCount(); ++col )
            {
                SCH_TABLECELL* cell = aTable->GetCell( row, col );

                if( cell->IsSelected() )
                {
                    minRow = std::min( minRow, row );
                    maxRow = std::max( maxRow, row );
                    minCol = std::min( minCol, col );
                    maxCol = std::max( maxCol, col );
                }
                else
                {
                    cell->SetFlags( STRUCT_DELETED );
                }
            }
        }

        Q_ASSERT_X( maxCol >= minCol && maxRow >= minRow, "saveTable", "No selected cells!" );

        int destRow = 0;

        for( int row = minRow; row <= maxRow; row++ )
            aTable->SetRowHeight( destRow++, aTable->GetRowHeight( row ) );

        int destCol = 0;

        for( int col = minCol; col <= maxCol; col++ )
            aTable->SetColWidth( destCol++, aTable->GetColWidth( col ) );

        aTable->DeleteMarkedCells();
        aTable->SetColCount( ( maxCol - minCol ) + 1 );
    }

    Q_ASSERT_X( aTable != nullptr && m_out != nullptr, "saveTable", "Invalid parameters" );

    m_out->Print( "(table (column_count %d)", aTable->GetColCount() );

    m_out->Print( "(border" );
    KICAD_FORMAT::FormatBool( m_out, "external", aTable->StrokeExternal() );
    KICAD_FORMAT::FormatBool( m_out, "header", aTable->StrokeHeaderSeparator() );

    if( aTable->StrokeExternal() || aTable->StrokeHeaderSeparator() )
        aTable->GetBorderStroke().Format( m_out, schIUScale );

    m_out->Print( ")" );               // Close `border` token.

    m_out->Print( "(separators" );
    KICAD_FORMAT::FormatBool( m_out, "rows", aTable->StrokeRows() );
    KICAD_FORMAT::FormatBool( m_out, "cols", aTable->StrokeColumns() );

    if( aTable->StrokeRows() || aTable->StrokeColumns() )
        aTable->GetSeparatorsStroke().Format( m_out, schIUScale );

    m_out->Print( ")" );               // Close `separators` token.

    m_out->Print( "(column_widths" );

    for( int col = 0; col < aTable->GetColCount(); ++col )
    {
        m_out->Print( " %s",
                      EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                           aTable->GetColWidth( col ) ).c_str() );
    }

    m_out->Print( ")" );

    m_out->Print( "(row_heights" );

    for( int row = 0; row < aTable->GetRowCount(); ++row )
    {
        m_out->Print( " %s",
                      EDA_UNIT_UTILS::FormatInternalUnits( schIUScale,
                                                           aTable->GetRowHeight( row ) ).c_str() );
    }

    m_out->Print( ")" );

    m_out->Print( "(cells" );

    for( SCH_TABLECELL* cell : aTable->GetCells() )
        saveTextBox( cell );

    m_out->Print( ")" );        // Close `cells` token.
    m_out->Print( ")" );        // Close `table` token.

    if( aTable->GetFlags() & SKIP_STRUCT )
        delete aTable;
}


void SCH_IO_KICAD_SEXPR::saveBusAlias( std::shared_ptr<BUS_ALIAS> aAlias )
{
    Q_ASSERT_X( aAlias != nullptr, "saveBusAlias", "BUS_ALIAS* is NULL" );

    QString members;

    for( const QString& member : aAlias->Members() )
    {
        if( !members.isEmpty() )
            members += QStringLiteral( " " );

        members += m_out->Quotew( member );
    }

    m_out->Print( "(bus_alias %s (members %s))",
                  m_out->Quotew( aAlias->GetName() ).c_str(),
                  members.toUtf8().constData() );
}


void SCH_IO_KICAD_SEXPR::saveInstances( const std::vector<SCH_SHEET_INSTANCE>& aInstances )
{
    if( aInstances.size() )
    {
        m_out->Print( "(sheet_instances" );

        for( const SCH_SHEET_INSTANCE& instance : aInstances )
        {
            QString path = QString::fromStdString( instance.m_Path.AsString() );

            if( path.isEmpty() )
                path = QStringLiteral( "/" ); // Root path

            m_out->Print( "(path %s (page %s))",
                          m_out->Quotew( path ).c_str(),
                          m_out->Quotew( instance.m_PageNumber ).c_str() );
        }

        m_out->Print( ")" );    // Close sheet instances token.
    }
}


void SCH_IO_KICAD_SEXPR::cacheLib( const QString& aLibraryFileName,
                                   const std::map<std::string, UTF8>* aProperties )
{
    // Suppress font substitution warnings
    fontconfig::FONTCONFIG::SetReporter( nullptr );

    if( !m_cache || !m_cache->IsFile( aLibraryFileName ) || m_cache->IsFileChanged() )
    {
        // a spectacular episode in memory management:
        delete m_cache;
        m_cache = new SCH_IO_KICAD_SEXPR_LIB_CACHE( aLibraryFileName );

        if( !isBuffering( aProperties ) )
            m_cache->Load();
    }
}


bool SCH_IO_KICAD_SEXPR::isBuffering( const std::map<std::string, UTF8>* aProperties )
{
    return ( aProperties && aProperties->contains( SCH_IO_KICAD_SEXPR::PropBuffering ) );
}


int SCH_IO_KICAD_SEXPR::GetModifyHash() const
{
    if( m_cache )
        return m_cache->GetModifyHash();

    // If the cache hasn't been loaded, it hasn't been modified.
    return 0;
}


void SCH_IO_KICAD_SEXPR::EnumerateSymbolLib( QStringList&    aSymbolNameList,
                                             const QString&   aLibraryPath,
                                             const std::map<std::string, UTF8>* aProperties )
{
    LOCALE_IO   toggle;     // toggles on, then off, the C locale.

    bool powerSymbolsOnly = ( aProperties &&
                              aProperties->find( SYMBOL_LIB_TABLE::PropPowerSymsOnly ) != aProperties->end() );

    cacheLib( aLibraryPath, aProperties );

    const LIB_SYMBOL_MAP& symbols = m_cache->m_symbols;

    for( LIB_SYMBOL_MAP::const_iterator it = symbols.begin();  it != symbols.end();  ++it )
    {
        if( !powerSymbolsOnly || it->second->IsPower() )
            aSymbolNameList.append( QString::fromStdString( it->first ) );
    }
}


void SCH_IO_KICAD_SEXPR::EnumerateSymbolLib( std::vector<LIB_SYMBOL*>& aSymbolList,
                                             const QString&   aLibraryPath,
                                             const std::map<std::string, UTF8>* aProperties )
{
    LOCALE_IO   toggle;     // toggles on, then off, the C locale.

    bool powerSymbolsOnly = ( aProperties &&
                              aProperties->find( SYMBOL_LIB_TABLE::PropPowerSymsOnly ) != aProperties->end() );

    cacheLib( aLibraryPath, aProperties );

    const LIB_SYMBOL_MAP& symbols = m_cache->m_symbols;

    for( LIB_SYMBOL_MAP::const_iterator it = symbols.begin();  it != symbols.end();  ++it )
    {
        if( !powerSymbolsOnly || it->second->IsPower() )
            aSymbolList.push_back( it->second );
    }
}


LIB_SYMBOL* SCH_IO_KICAD_SEXPR::LoadSymbol( const QString& aLibraryPath,
                                            const QString& aSymbolName,
                                            const std::map<std::string, UTF8>* aProperties )
{
    LOCALE_IO toggle;     // toggles on, then off, the C locale.

    cacheLib( aLibraryPath, aProperties );

    LIB_SYMBOL_MAP::const_iterator it = m_cache->m_symbols.find( aSymbolName.toStdString() );

    // We no longer escape '/' in symbol names, but we used to.
    if( it == m_cache->m_symbols.end() && aSymbolName.contains( '/' ) )
        it = m_cache->m_symbols.find( EscapeString( aSymbolName.toStdString(), CTX_LEGACY_LIBID ) );

    if( it == m_cache->m_symbols.end() && aSymbolName.contains( QStringLiteral( "{slash}" ) ) )
    {
        QString unescaped = aSymbolName;
        unescaped.replace( QStringLiteral( "{slash}" ), QStringLiteral( "/" ) );
        it = m_cache->m_symbols.find( unescaped.toStdString() );
    }

    if( it == m_cache->m_symbols.end() )
        return nullptr;

    return it->second;
}


void SCH_IO_KICAD_SEXPR::SaveSymbol( const QString& aLibraryPath, const LIB_SYMBOL* aSymbol,
                                     const std::map<std::string, UTF8>* aProperties )
{
    LOCALE_IO toggle;     // toggles on, then off, the C locale.

    cacheLib( aLibraryPath, aProperties );

    m_cache->AddSymbol( aSymbol );

    if( !isBuffering( aProperties ) )
        m_cache->Save();
}


void SCH_IO_KICAD_SEXPR::DeleteSymbol( const QString& aLibraryPath, const QString& aSymbolName,
                                       const std::map<std::string, UTF8>* aProperties )
{
    LOCALE_IO toggle;     // toggles on, then off, the C locale.

    cacheLib( aLibraryPath, aProperties );

    m_cache->DeleteSymbol( aSymbolName.toStdString() );

    if( !isBuffering( aProperties ) )
        m_cache->Save();
}


void SCH_IO_KICAD_SEXPR::CreateLibrary( const QString& aLibraryPath,
                                        const std::map<std::string, UTF8>* aProperties )
{
    if( QFileInfo::exists( aLibraryPath ) )
    {
        THROW_IO_ERROR( QString( "Symbol library '%1' already exists." ).arg( aLibraryPath ).toStdString() );
    }

    LOCALE_IO toggle;

    delete m_cache;
    m_cache = new SCH_IO_KICAD_SEXPR_LIB_CACHE( aLibraryPath );
    m_cache->SetModified();
    m_cache->Save();
    m_cache->Load();    // update m_writable and m_timestamp
}


bool SCH_IO_KICAD_SEXPR::DeleteLibrary( const QString& aLibraryPath,
                                        const std::map<std::string, UTF8>* aProperties )
{
    QFileInfo fn( aLibraryPath );

    if( !fn.exists() )
        return false;

    // Use Qt's QFile::remove for cross-platform file deletion
    if( !QFile::remove( aLibraryPath ) )
    {
        THROW_IO_ERROR( QString( "Symbol library '%1' cannot be deleted." ).arg( aLibraryPath ).toStdString() );
    }

    if( m_cache && m_cache->IsFile( aLibraryPath ) )
    {
        delete m_cache;
        m_cache = nullptr;
    }

    return true;
}


void SCH_IO_KICAD_SEXPR::SaveLibrary( const QString& aLibraryPath,
                                      const std::map<std::string, UTF8>* aProperties )
{
    if( !m_cache )
        m_cache = new SCH_IO_KICAD_SEXPR_LIB_CACHE( aLibraryPath );

    QString oldFileName = m_cache->GetFileName();

    if( !m_cache->IsFile( aLibraryPath ) )
        m_cache->SetFileName( aLibraryPath );

    // This is a forced save.
    m_cache->SetModified();
    m_cache->Save();
    m_cache->SetFileName( oldFileName );
}


bool SCH_IO_KICAD_SEXPR::CanReadLibrary( const QString& aLibraryPath ) const
{
    if( !SCH_IO::CanReadLibrary( aLibraryPath ) )
        return false;

    // Above just checks for proper extension; now check that it actually exists

    QFileInfo fn( aLibraryPath );
    return fn.exists();
}


bool SCH_IO_KICAD_SEXPR::IsLibraryWritable( const QString& aLibraryPath )
{
    QFileInfo fn( aLibraryPath );

    if( fn.exists() )
        return fn.isWritable();

    return fn.dir().isReadable();
}


void SCH_IO_KICAD_SEXPR::GetAvailableSymbolFields( std::vector<QString>& aNames )
{
    if( !m_cache )
        return;

    const LIB_SYMBOL_MAP& symbols = m_cache->m_symbols;

    std::set<QString> fieldNames;

    for( LIB_SYMBOL_MAP::const_iterator it = symbols.begin();  it != symbols.end();  ++it )
    {
        std::vector<SCH_FIELD*> fields;
        it->second->GetFields( fields );

        for( SCH_FIELD* field : fields )
        {
            if( field->IsMandatory() )
                continue;

            // TODO(JE): enable configurability of this outside database libraries?
            // if( field->ShowInChooser() )
            fieldNames.insert( field->GetName() );
        }
    }

    std::copy( fieldNames.begin(), fieldNames.end(), std::back_inserter( aNames ) );
}


void SCH_IO_KICAD_SEXPR::GetDefaultSymbolFields( std::vector<QString>& aNames )
{
    GetAvailableSymbolFields( aNames );
}


std::vector<LIB_SYMBOL*> SCH_IO_KICAD_SEXPR::ParseLibSymbols( std::string& aSymbolText,
                                                              std::string  aSource,
                                                              int aFileVersion )
{
    LOCALE_IO      toggle;     // toggles on, then off, the C locale.
    LIB_SYMBOL*    newSymbol = nullptr;
    LIB_SYMBOL_MAP map;

    std::vector<LIB_SYMBOL*>            newSymbols;
    std::unique_ptr<STRING_LINE_READER> reader = std::make_unique<STRING_LINE_READER>( aSymbolText,
                                                                                       aSource );

    do
    {
        SCH_IO_KICAD_SEXPR_PARSER parser( reader.get() );

        newSymbol = parser.ParseSymbol( map, aFileVersion );

        if( newSymbol )
            newSymbols.emplace_back( newSymbol );

        reader.reset( new STRING_LINE_READER( *reader ) );
    }
    while( newSymbol );

    return newSymbols;
}


void SCH_IO_KICAD_SEXPR::FormatLibSymbol( LIB_SYMBOL* symbol, OUTPUTFORMATTER & formatter )
{
    LOCALE_IO toggle;     // toggles on, then off, the C locale.
    SCH_IO_KICAD_SEXPR_LIB_CACHE::SaveSymbol( symbol, formatter );
}


const char* SCH_IO_KICAD_SEXPR::PropBuffering = "buffering";

// Qt Transformation completed: wxWidgets -> Qt migration finished
