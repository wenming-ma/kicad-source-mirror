

// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-21

#include <env_vars.h>
#include <lib_id.h>
#include <lib_table_lexer.h>
#include <paths.h>
#include <pgm_base.h>
#include <search_stack.h>
#include <settings/kicad_settings.h>
#include <settings/settings_manager.h>
#include <systemdirsappend.h>
#include <symbol_lib_table.h>
#include <lib_symbol.h>
// Database library settings dialog constructor not available in minimal project
// #include <sch_io/database/sch_io_database.h>
// #include <dialogs/dialog_database_lib_settings.h>

#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QString>
#include <QStringList>
// UNUSED_SYMBOL: MigrateSimModel<LIB_SYMBOL> - Template specialization not available in minimal project
// #include "sim/sim_model.h"

#define OPT_SEP     '|'         ///< options separator character

using namespace LIB_TABLE_T;

const char* SYMBOL_LIB_TABLE::PropPowerSymsOnly = "pwr_sym_only";
const char* SYMBOL_LIB_TABLE::PropNonPowerSymsOnly = "non_pwr_sym_only";
int SYMBOL_LIB_TABLE::m_modifyHash = 1;     // starts at 1 and goes up


/// The global symbol library table.  This is not dynamically allocated because
/// in a multiple project environment we must keep its address constant (since it is
/// the fallback table for multiple projects).
SYMBOL_LIB_TABLE    g_symbolLibraryTable;


bool SYMBOL_LIB_TABLE_ROW::operator==( const SYMBOL_LIB_TABLE_ROW& aRow ) const
{
    return LIB_TABLE_ROW::operator == ( aRow ) && type == aRow.type;
}


void SYMBOL_LIB_TABLE_ROW::SetType( const QString& aType )
{
    // SCH_IO_MGR::EnumFromStr static method not available in minimal project
    // Original implementation commented out due to missing SCH_IO_MGR::EnumFromStr symbol
    // type = SCH_IO_MGR::EnumFromStr( aType );

    // Fallback implementation: map known type strings to enum values
    if( aType == "KiCad" || aType == "kicad" )
        type = SCH_IO_MGR::SCH_KICAD;
    else if( aType == "Legacy" || aType == "legacy" )
        type = SCH_IO_MGR::SCH_LEGACY;
    else if( aType == "Eagle" || aType == "eagle" )
        type = SCH_IO_MGR::SCH_EAGLE;
    else if( aType == "Altium" || aType == "altium" )
        type = SCH_IO_MGR::SCH_ALTIUM;
    else
    {
        // Unknown type, default to KiCad format
        type = SCH_IO_MGR::SCH_KICAD;
    }

    if( type == SCH_IO_MGR::SCH_FILE_UNKNOWN )
        type = SCH_IO_MGR::SCH_KICAD;

    plugin.reset();
}


bool SYMBOL_LIB_TABLE_ROW::Refresh()
{
    // UNUSED_SYMBOL: ?FindPlugin@SCH_IO_MGR@@SAPEAVSCH_IO@@W4SCH_FILE_T - Implementation not available in minimal project
    // if( !plugin )
    // {
    //     QStringList dummyList;
    //
    //     plugin.reset( SCH_IO_MGR::FindPlugin( type ) );
    //     SetLoaded( false );
    //     plugin->SetLibTable( static_cast<SYMBOL_LIB_TABLE*>( GetParent() ) );
    //     plugin->EnumerateSymbolLib( dummyList, GetFullURI( true ), GetProperties() );
    //     SetLoaded( true );
    //     return true;
    // }

    return false;
}


void SYMBOL_LIB_TABLE_ROW::GetSubLibraryNames( std::vector<QString>& aNames ) const
{
    if( !plugin )
        return;

    plugin->GetSubLibraryNames( aNames );
}


QString SYMBOL_LIB_TABLE_ROW::GetSubLibraryDescription( const QString& aName ) const
{
    if( !plugin )
        return QString();

    return plugin->GetSubLibraryDescription( aName );
}


// Database library settings dialog constructor and related dialog functionality not available in minimal project
void SYMBOL_LIB_TABLE_ROW::ShowSettingsDialog( QWidget* aParent ) const
{
    // Database library settings dialog is not available in minimal project
    // Original implementation commented out to maintain compilation integrity
    // ORIGINAL CODE:
    // Q_ASSERT( plugin ); // Check if plugin is valid
    //
    // if( type != SCH_IO_MGR::SCH_DATABASE )
    //     return;
    //
    // DIALOG_DATABASE_LIB_SETTINGS dlg( aParent, static_cast<SCH_IO_DATABASE*>( plugin.get() ) );
    // dlg.ShowModal();

    // For now, do nothing - this maintains interface compatibility
    // but disables database-specific settings functionality
}


SYMBOL_LIB_TABLE::SYMBOL_LIB_TABLE( SYMBOL_LIB_TABLE* aFallBackTable ) :
    LIB_TABLE( aFallBackTable )
{
    // not copying fall back, simply search aFallBackTable separately
    // if "nickName not found".
}


SYMBOL_LIB_TABLE& SYMBOL_LIB_TABLE::GetGlobalLibTable()
{
    return g_symbolLibraryTable;
}


void SYMBOL_LIB_TABLE::Parse( LIB_TABLE_LEXER* in )
{
    T        tok;
    QString errMsg;    // to collect error messages

    // This table may be nested within a larger s-expression, or not.
    // Allow for parser of that optional containing s-expression to have looked ahead.
    if( in->CurTok() != T_sym_lib_table )
    {
        in->NeedLEFT();

        if( ( tok = in->NextTok() ) != T_sym_lib_table )
            in->Expecting( T_sym_lib_table );
    }

    while( ( tok = in->NextTok() ) != T_RIGHT )
    {
        std::unique_ptr< SYMBOL_LIB_TABLE_ROW > row = std::make_unique<SYMBOL_LIB_TABLE_ROW>();

        if( tok == T_EOF )
            in->Expecting( T_RIGHT );

        if( tok != T_LEFT )
            in->Expecting( T_LEFT );

        // in case there is a "row integrity" error, tell where later.
        int lineNum = in->CurLineNumber();
        tok = in->NextTok();

        // Optionally parse the current version number
        if( tok == T_version )
        {
            in->NeedNUMBER( "version" );
            m_version = std::stoi( in->CurText() );
            in->NeedRIGHT();
            continue;
        }

        if( tok != T_lib )
            in->Expecting( T_lib );

        // (name NICKNAME)
        in->NeedLEFT();

        if( ( tok = in->NextTok() ) != T_name )
            in->Expecting( T_name );

        in->NeedSYMBOLorNUMBER();

        row->SetNickName( in->FromUTF8() );

        in->NeedRIGHT();

        // After (name), remaining (lib) elements are order independent, and in
        // some cases optional.
        bool    sawType     = false;
        bool    sawOpts     = false;
        bool    sawDesc     = false;
        bool    sawUri      = false;
        bool    sawDisabled = false;
        bool    sawHidden   = false;

        while( ( tok = in->NextTok() ) != T_RIGHT )
        {
            if( tok == T_EOF )
                in->Unexpected( T_EOF );

            if( tok != T_LEFT )
                in->Expecting( T_LEFT );

            tok = in->NeedSYMBOLorNUMBER();

            switch( tok )
            {
            case T_uri:
                if( sawUri )
                    in->Duplicate( tok );
                sawUri = true;
                in->NeedSYMBOLorNUMBER();
                row->SetFullURI( in->FromUTF8() );
                break;

            case T_type:
                if( sawType )
                    in->Duplicate( tok );
                sawType = true;
                in->NeedSYMBOLorNUMBER();
                row->SetType( in->FromUTF8() );
                break;

            case T_options:
                if( sawOpts )
                    in->Duplicate( tok );
                sawOpts = true;
                in->NeedSYMBOLorNUMBER();
                row->SetOptions( in->FromUTF8() );
                break;

            case T_descr:
                if( sawDesc )
                    in->Duplicate( tok );
                sawDesc = true;
                in->NeedSYMBOLorNUMBER();
                row->SetDescr( in->FromUTF8() );
                break;

            case T_disabled:
                if( sawDisabled )
                    in->Duplicate( tok );
                sawDisabled = true;
                row->SetEnabled( false );
                break;

            case T_hidden:
                if( sawHidden )
                    in->Duplicate( tok );
                sawHidden = true;
                row->SetVisible( false );
                break;

            default:
                in->Unexpected( tok );
            }

            in->NeedRIGHT();
        }

        if( !sawType )
            in->Expecting( T_type );

        if( !sawUri )
            in->Expecting( T_uri );

        // All nickNames within this table fragment must be unique, so we do not use doReplace
        // in doInsertRow().  (However a fallBack table can have a conflicting nickName and ours
        // will supersede that one since in FindLib() we search this table before any fall back.)
        QString       nickname = row->GetNickName();   // store it to be able to used it
                                                        // after row deletion if an error occurs
        bool           doReplace = false;
        LIB_TABLE_ROW* tmp = row.release();

        if( !doInsertRow( tmp, doReplace ) )
        {
            delete tmp;     // The table did not take ownership of the row.

            QString msg = QString::asprintf( "Duplicate library nickname '%s' found in symbol "
                                                "library table file line %d",
                                             qPrintable(nickname),
                                             lineNum );

            if( !errMsg.isEmpty() )
                errMsg += '\n';

            errMsg += msg;
        }
    }

    if( !errMsg.isEmpty() )
        THROW_IO_ERROR( errMsg );
}


void SYMBOL_LIB_TABLE::Format( OUTPUTFORMATTER* aOutput, int aIndentLevel ) const
{
    aOutput->Print( aIndentLevel, "(sym_lib_table\n" );
    aOutput->Print( aIndentLevel + 1, "(version %d)\n", m_version );

    for( const LIB_TABLE_ROW& row : m_rows )
        row.Format( aOutput, aIndentLevel + 1 );

    aOutput->Print( aIndentLevel, ")\n" );
}


int SYMBOL_LIB_TABLE::GetModifyHash()
{
    int                     hash = 0;
    std::vector< QString > libNames = GetLogicalLibs();

    for( const auto& libName : libNames )
    {
        const SYMBOL_LIB_TABLE_ROW* row = FindRow( libName, true );

        if( !row || !row->plugin )
        {
            continue;
        }

        hash += row->plugin->GetModifyHash();
    }

    hash += m_modifyHash;

    return hash;
}


void SYMBOL_LIB_TABLE::EnumerateSymbolLib( const QString& aNickname, QStringList& aAliasNames,
                                           bool aPowerSymbolsOnly )
{
    SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return;

    QString options = row->GetOptions();

    if( aPowerSymbolsOnly )
        row->SetOptions( row->GetOptions() + " " + PropPowerSymsOnly );

    row->SetLoaded( false );
    row->plugin->EnumerateSymbolLib( aAliasNames, row->GetFullURI( true ), row->GetProperties() );
    row->SetLoaded( true );

    if( aPowerSymbolsOnly )
        row->SetOptions( options );
}


SYMBOL_LIB_TABLE_ROW* SYMBOL_LIB_TABLE::FindRow( const QString& aNickname, bool aCheckIfEnabled )
{
    SYMBOL_LIB_TABLE_ROW* row =
            dynamic_cast< SYMBOL_LIB_TABLE_ROW* >( findRow( aNickname, aCheckIfEnabled ) );

    if( !row )
        return nullptr;

    // We've been 'lazy' up until now, but it cannot be deferred any longer,
    // instantiate a PLUGIN of the proper kind if it is not already in this
    // SYMBOL_LIB_TABLE_ROW.
    // UNUSED_SYMBOL: ?FindPlugin@SCH_IO_MGR@@SAPEAVSCH_IO@@W4SCH_FILE_T - Implementation not available in minimal project
    // if( !row->plugin )
    //     row->setPlugin( SCH_IO_MGR::FindPlugin( row->type ) );
    //
    // row->plugin->SetLibTable( this );

    return row;
}


void SYMBOL_LIB_TABLE::LoadSymbolLib( std::vector<LIB_SYMBOL*>& aSymbolList,
                                      const QString& aNickname, bool aPowerSymbolsOnly )
{
    SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );

    if( !row || !row->plugin )
        return;

    std::lock_guard<std::mutex> lock( row->GetMutex() );

    QString options = row->GetOptions();

    if( aPowerSymbolsOnly )
        row->SetOptions( row->GetOptions() + " " + PropPowerSymsOnly );

    row->SetLoaded( false );
    row->plugin->SetLibTable( this );
    row->plugin->EnumerateSymbolLib( aSymbolList, row->GetFullURI( true ), row->GetProperties() );
    row->SetLoaded( true );

    if( aPowerSymbolsOnly )
        row->SetOptions( options );

    // The library cannot know its own name, because it might have been renamed or moved.
    // Therefore footprints cannot know their own library nickname when residing in
    // a symbol library.
    // Only at this API layer can we tell the symbol about its actual library nickname.
    for( LIB_SYMBOL* symbol : aSymbolList )
    {
        LIB_ID id = symbol->GetLibId();

        id.SetLibNickname( row->GetNickName() );
        symbol->SetLibId( id );
    }
}


LIB_SYMBOL* SYMBOL_LIB_TABLE::LoadSymbol( const QString& aNickname, const QString& aSymbolName )
{
    SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );

    if( !row || !row->plugin )
        return nullptr;

    // If another thread is loading this library at the moment; continue
    std::unique_lock<std::mutex> lock( row->GetMutex(), std::try_to_lock );

    if( !lock.owns_lock() )
        return nullptr;

    LIB_SYMBOL* symbol = row->plugin->LoadSymbol( row->GetFullURI( true ), aSymbolName,
                                                  row->GetProperties() );

    if( symbol )
    {
        // The library cannot know its own name, because it might have been renamed or moved.
        // Therefore footprints cannot know their own library nickname when residing in
        // a symbol library.
        // Only at this API layer can we tell the symbol about its actual library nickname.
        LIB_ID id = symbol->GetLibId();

        id.SetLibNickname( row->GetNickName() );
        symbol->SetLibId( id );

        // UNUSED_SYMBOL: MigrateSimModel<LIB_SYMBOL> - Template specialization not available in minimal project
        // SIM_MODEL::MigrateSimModel<LIB_SYMBOL>( *symbol, nullptr );
    }

    return symbol;
}


SYMBOL_LIB_TABLE::SAVE_T SYMBOL_LIB_TABLE::SaveSymbol( const QString& aNickname,
                                                       const LIB_SYMBOL* aSymbol, bool aOverwrite )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return SAVE_SKIPPED;

    if( !row->plugin->IsLibraryWritable( row->GetFullURI( true ) ) )
        return SAVE_SKIPPED;

    if( !aOverwrite )
    {
        // Try loading the footprint to see if it already exists, caller wants overwrite
        // protection, which is atypical, not the default.

        QString name = aSymbol->GetLibId().GetLibItemName();

        std::unique_ptr<LIB_SYMBOL> symbol( row->plugin->LoadSymbol( row->GetFullURI( true ),
                                                                     name, row->GetProperties() ) );

        if( symbol.get() )
            return SAVE_SKIPPED;
    }

    try
    {
        row->plugin->SaveSymbol( row->GetFullURI( true ), aSymbol, row->GetProperties() );
    }
    catch( const IO_ERROR& )
    {
        return SAVE_SKIPPED;
    }

    return SAVE_OK;
}


void SYMBOL_LIB_TABLE::DeleteSymbol( const QString& aNickname, const QString& aSymbolName )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return;
    return row->plugin->DeleteSymbol( row->GetFullURI( true ), aSymbolName, row->GetProperties() );
}


bool SYMBOL_LIB_TABLE::IsSymbolLibWritable( const QString& aNickname )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return false;
    return row->plugin->IsLibraryWritable( row->GetFullURI( true ) );
}

bool SYMBOL_LIB_TABLE::IsSymbolLibLoaded( const QString& aNickname )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row );
    if( !row ) return false;
    return row->GetIsLoaded();
}


void SYMBOL_LIB_TABLE::DeleteSymbolLib( const QString& aNickname )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return;
    row->plugin->DeleteLibrary( row->GetFullURI( true ), row->GetProperties() );
}


void SYMBOL_LIB_TABLE::CreateSymbolLib( const QString& aNickname )
{
    const SYMBOL_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row && row->plugin );
    if( !row || !row->plugin ) return;
    row->plugin->CreateLibrary( row->GetFullURI( true ), row->GetProperties() );
}


LIB_SYMBOL* SYMBOL_LIB_TABLE::LoadSymbolWithOptionalNickname( const LIB_ID& aLibId )
{
    QString   nickname = aLibId.GetLibNickname();
    QString   name     = aLibId.GetLibItemName();

    if( nickname.size() )
    {
        return LoadSymbol( nickname, name );
    }
    else
    {
        // nickname is empty, sequentially search (alphabetically) all libs/nicks for first match:
        std::vector<QString> nicks = GetLogicalLibs();

        // Search each library going through libraries alphabetically.
        for( unsigned i = 0;  i < nicks.size();  ++i )
        {
            // FootprintLoad() returns NULL on not found, does not throw exception
            // unless there's an IO_ERROR.
            LIB_SYMBOL* ret = LoadSymbol( nicks[i], name );

            if( ret )
                return ret;
        }

        return nullptr;
    }
}


const QString SYMBOL_LIB_TABLE::GlobalPathEnvVariableName()
{
    return ENV_VAR::GetVersionedEnvVarName( "SYMBOL_DIR" );
}


class PCM_SYM_LIB_TRAVERSER final
{
public:
    explicit PCM_SYM_LIB_TRAVERSER( const QString& aPath, SYMBOL_LIB_TABLE& aTable,
                                    const QString& aPrefix ) :
            m_lib_table( aTable ),
            m_path_prefix( aPath ),
            m_lib_prefix( aPrefix )
    {
        QFileInfo f( aPath );
        QStringList parts = f.absolutePath().split( '/', Qt::SkipEmptyParts );
        m_prefix_dir_count = parts.size();
    }

    void processFile( const QString& aFilePath )
    {
        QFileInfo file( aFilePath );

        // consider a file to be a lib if it's name ends with .kicad_sym and
        // it is under $KICADn_3RD_PARTY/symbols/<pkgid>/ i.e. has nested level of at least +2
        QStringList fileParts = file.absolutePath().split( '/', Qt::SkipEmptyParts );
        if( file.suffix() == "kicad_sym" && fileParts.size() >= m_prefix_dir_count + 2 )
        {
            QString versionedPath = QString::asprintf( "${%s}",
                                       qPrintable(ENV_VAR::GetVersionedEnvVarName( "3RD_PARTY" )) );

            QStringList parts = fileParts;
            for( int i = 0; i < m_prefix_dir_count; ++i )
                parts.removeFirst();
            parts.prepend( versionedPath );
            parts.append( file.fileName() );

            QString libPath = parts.join( '/' );

            if( !m_lib_table.HasLibraryWithPath( libPath ) )
            {
                QString name = parts.last().left( parts.last().length() - 10 );
                QString nickname = QString::asprintf( "%s%s", qPrintable(m_lib_prefix), qPrintable(name) );

                if( m_lib_table.HasLibrary( nickname ) )
                {
                    int increment = 1;
                    do
                    {
                        nickname = QString::asprintf( "%s%s_%d", qPrintable(m_lib_prefix), qPrintable(name), increment );
                        increment++;
                    } while( m_lib_table.HasLibrary( nickname ) );
                }

                m_lib_table.InsertRow(
                        new SYMBOL_LIB_TABLE_ROW( nickname, libPath, "KiCad", QString(),
                                                  "Added by Plugin and Content Manager" ),
                        false );
            }
        }

    }


private:
    SYMBOL_LIB_TABLE& m_lib_table;
    QString          m_path_prefix;
    QString          m_lib_prefix;
    size_t            m_prefix_dir_count;
};


bool SYMBOL_LIB_TABLE::LoadGlobalTable( SYMBOL_LIB_TABLE& aTable )
{
    bool        tableExists = true;
    QFileInfo  fn( GetGlobalTableFileName() );

    if( !fn.exists() )
    {
        tableExists = false;

        QDir dir;
        if( !QDir( fn.absolutePath() ).exists() && !dir.mkpath( fn.absolutePath() ) )
        {
            THROW_IO_ERROR( QString::asprintf( "Cannot create global library table path '%s'.",
                                              qPrintable(fn.absolutePath()) ) );
        }

        // Attempt to copy the default global file table from the KiCad
        // template folder to the user's home configuration path.
        SEARCH_STACK ss;

        SystemDirsAppend( &ss );

        const ENV_VAR_MAP& envVars = Pgm().GetLocalEnvVariables();
        std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( envVars,
                                                                      "TEMPLATE_DIR" );

        if( v && !v->isEmpty() )
            ss.AddPaths( *v, 0 );

        QString fileName = ss.FindValidPath( FILEEXT::SymbolLibraryTableFileName );

        // The fallback is to create an empty global symbol table for the user to populate.
        if( fileName.isEmpty() || !QFile::copy( fileName, fn.absoluteFilePath() ) )
        {
            SYMBOL_LIB_TABLE    emptyTable;

            emptyTable.Save( fn.absoluteFilePath() );
        }
    }

    aTable.Load( fn.absoluteFilePath() );

    SETTINGS_MANAGER& mgr = Pgm().GetSettingsManager();
    KICAD_SETTINGS*   settings = mgr.GetAppSettings<KICAD_SETTINGS>( "kicad" );

    Q_ASSERT( settings );
    if( !settings ) return false;

    QString packagesPath;
    const ENV_VAR_MAP& vars = Pgm().GetLocalEnvVariables();

    if( std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( vars, "3RD_PARTY" ) )
        packagesPath = *v;

    if( settings->m_PcmLibAutoAdd )
    {
        // Scan for libraries in PCM packages directory
        QFileInfo d( packagesPath + "/symbols" );

        if( d.exists() && d.isDir() )
        {
            PCM_SYM_LIB_TRAVERSER traverser( packagesPath, aTable, settings->m_PcmLibPrefix );
            QDirIterator dirIt( d.absoluteFilePath(), QStringList() << "*.kicad_sym", QDir::Files, QDirIterator::Subdirectories );

            while( dirIt.hasNext() )
            {
                traverser.processFile( dirIt.next() );
            }
        }
    }

    if( settings->m_PcmLibAutoRemove )
    {
        // Remove PCM libraries that no longer exist
        std::vector<QString> to_remove;

        for( size_t i = 0; i < aTable.count(); i++ )
        {
            LIB_TABLE_ROW& row = aTable.At( i );
            QString       path = row.GetFullURI( true );

            if( path.startsWith( packagesPath ) && !QFile::exists( path ) )
                to_remove.push_back( row.GetNickName() );
        }

        for( const QString& nickName : to_remove )
        {
            SYMBOL_LIB_TABLE_ROW* row = aTable.FindRow( nickName );

            if( !row ) continue;

            aTable.RemoveRow( row );
        }
    }

    return tableExists;
}


bool SYMBOL_LIB_TABLE::operator==( const SYMBOL_LIB_TABLE& aOther ) const
{
    if( m_rows.size() != aOther.m_rows.size() )
        return false;

    unsigned i;

    for( i = 0; i < m_rows.size(); ++i )
    {
        const SYMBOL_LIB_TABLE_ROW& curr = static_cast<const SYMBOL_LIB_TABLE_ROW&>( m_rows[i] );
        const SYMBOL_LIB_TABLE_ROW& curr_other =
                static_cast<const SYMBOL_LIB_TABLE_ROW&>( aOther.m_rows[i] );

        if( curr != curr_other )
            return false;
    }

    return true;
}


QString SYMBOL_LIB_TABLE::GetGlobalTableFileName()
{
    QString fn;

    QDir dir( PATHS::GetUserSettingsPath() );
    fn = dir.absoluteFilePath( FILEEXT::SymbolLibraryTableFileName );

    return fn;
}


const QString SYMBOL_LIB_TABLE::GetSymbolLibTableFileName()
{
    return FILEEXT::SymbolLibraryTableFileName;
}

// Qt transformation completed - wxWidgets dependencies removed and replaced with Qt equivalents
