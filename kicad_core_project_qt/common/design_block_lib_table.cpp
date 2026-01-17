#include <kiface_base.h>
#include <env_vars.h>
#include <lib_id.h>
#include <lib_table_lexer.h>
#include <paths.h>
#include <pgm_base.h>
#include <search_stack.h>
#include <settings/kicad_settings.h>
#include <settings/settings_manager.h>
#include <systemdirsappend.h>
#include <wildcards_and_files_ext.h>
#include <design_block_info.h>
#include <design_block_lib_table.h>
#include <design_block.h>

#include <QDir>
#include <QDirIterator>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QStringList>
#include <QCoreApplication>
#include <vector>
#include <locale_io.h>
#include <i18n_utility.h>

#define OPT_SEP '|'

DESIGN_BLOCK_LIB_TABLE GDesignBlockTable;

DESIGN_BLOCK_LIST_IMPL GDesignBlockList;


using namespace LIB_TABLE_T;


bool DESIGN_BLOCK_LIB_TABLE_ROW::operator==( const DESIGN_BLOCK_LIB_TABLE_ROW& aRow ) const
{
    return LIB_TABLE_ROW::operator==( aRow ) && type == aRow.type;
}


void DESIGN_BLOCK_LIB_TABLE_ROW::SetType( const QString& aType )
{
    type = DESIGN_BLOCK_IO_MGR::EnumFromStr( aType );

    if( DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T( -1 ) == type )
        type = DESIGN_BLOCK_IO_MGR::KICAD_SEXP;

    plugin.reset();
}


DESIGN_BLOCK_LIB_TABLE::DESIGN_BLOCK_LIB_TABLE( DESIGN_BLOCK_LIB_TABLE* aFallBackTable ) :
        LIB_TABLE( aFallBackTable )
{
    // not copying fall back, simply search aFallBackTable separately
    // if "nickName not found".
}


void DESIGN_BLOCK_LIB_TABLE::Parse( LIB_TABLE_LEXER* in )
{
    T        tok;
    QString errMsg; // to collect error messages

    // This table may be nested within a larger s-expression, or not.
    // Allow for parser of that optional containing s-epression to have looked ahead.
    if( in->CurTok() != T_design_block_lib_table )
    {
        in->NeedLEFT();

        if( ( tok = in->NextTok() ) != T_design_block_lib_table )
            in->Expecting( T_design_block_lib_table );
    }

    while( ( tok = in->NextTok() ) != T_RIGHT )
    {
        std::unique_ptr<DESIGN_BLOCK_LIB_TABLE_ROW> row =
                std::make_unique<DESIGN_BLOCK_LIB_TABLE_ROW>();

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
        bool sawType = false;
        bool sawOpts = false;
        bool sawDesc = false;
        bool sawUri = false;
        bool sawDisabled = false;

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
                // Hiding design block libraries is not yet supported.  Unclear what path can
                // set this attribute, but clear it on load.
                row->SetVisible();
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
        // will supercede that one since in FindLib() we search this table before any fall back.)
        QString nickname = row->GetNickName(); // store it to be able to used it
                                                // after row deletion if an error occurs
        bool           doReplace = false;
        LIB_TABLE_ROW* tmp = row.release();

        if( !doInsertRow( tmp, doReplace ) )
        {
            delete tmp; // The table did not take ownership of the row.

            QString msg = QString( _( "Duplicate library nickname '%1' found in "
                                                "design block library table file line %2." ) )
                                             .arg( nickname ).arg( lineNum );

            if( !errMsg.isEmpty() )
                errMsg += '\n';

            errMsg += msg;
        }
    }

    if( !errMsg.isEmpty() )
        THROW_IO_ERROR( errMsg );
}


bool DESIGN_BLOCK_LIB_TABLE::operator==( const DESIGN_BLOCK_LIB_TABLE& aDesignBlockTable ) const
{
    if( m_rows.size() == aDesignBlockTable.m_rows.size() )
    {
        for( unsigned i = 0; i < m_rows.size(); ++i )
        {
            if( (DESIGN_BLOCK_LIB_TABLE_ROW&) m_rows[i]
                != (DESIGN_BLOCK_LIB_TABLE_ROW&) aDesignBlockTable.m_rows[i] )
            {
                return false;
            }
        }

        return true;
    }

    return false;
}


void DESIGN_BLOCK_LIB_TABLE::Format( OUTPUTFORMATTER* aOutput, int aIndentLevel ) const
{
    aOutput->Print( aIndentLevel, "(design_block_lib_table\n" );
    aOutput->Print( aIndentLevel + 1, "(version %d)\n", m_version );

    for( const LIB_TABLE_ROW& row : m_rows)
        row.Format( aOutput, aIndentLevel + 1 );

    aOutput->Print( aIndentLevel, ")\n" );
}


long long DESIGN_BLOCK_LIB_TABLE::GenerateTimestamp( const QString* aNickname )
{
    long long hash = 0;

    if( aNickname )
    {
        const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( *aNickname, true );

        Q_ASSERT( row && row->plugin );

        return row->plugin->GetLibraryTimestamp( row->GetFullURI( true ) )
               + qHash( *aNickname );
    }

    for( const QString& nickname : GetLogicalLibs() )
    {
        const DESIGN_BLOCK_LIB_TABLE_ROW* row = nullptr;

        try
        {
            row = FindRow( nickname, true );
        }
        catch( ... )
        {
            // Do nothing if not found: just skip.
        }

        if( !row || !row->plugin ) continue;

        hash += row->plugin->GetLibraryTimestamp( row->GetFullURI( true ) )
                + qHash( nickname );
    }

    return hash;
}


void DESIGN_BLOCK_LIB_TABLE::DesignBlockEnumerate( QStringList&  aDesignBlockNames, const QString& aNickname,
                                                   bool aBestEfforts, const LOCALE_IO* aLocale )
{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );

    if( !aLocale )
    {
        LOCALE_IO toggle_locale;

        row->plugin->DesignBlockEnumerate( aDesignBlockNames, row->GetFullURI( true ), aBestEfforts,
                                           row->GetProperties() );
    }
    else
    {
        row->plugin->DesignBlockEnumerate( aDesignBlockNames, row->GetFullURI( true ), aBestEfforts,
                                           row->GetProperties() );
    }
}


const DESIGN_BLOCK_LIB_TABLE_ROW* DESIGN_BLOCK_LIB_TABLE::FindRow( const QString& aNickname,
                                                                   bool            aCheckIfEnabled )
{
    DESIGN_BLOCK_LIB_TABLE_ROW* row =
            static_cast<DESIGN_BLOCK_LIB_TABLE_ROW*>( findRow( aNickname, aCheckIfEnabled ) );

    if( !row )
    {
        THROW_IO_ERROR( QString( _( "design-block-lib-table files contain no library "
                                             "named '%1'." ) ).arg( aNickname ) );
    }

    if( !row->plugin )
        row->setPlugin( DESIGN_BLOCK_IO_MGR::FindPlugin( row->type ) );

    return row;
}


static void setLibNickname( DESIGN_BLOCK* aModule, const QString& aNickname,
                            const QString& aDesignBlockName )
{
    // The library cannot know its own name, because it might have been renamed or moved.
    // Therefore design blocks cannot know their own library nickname when residing in
    // a design block library.
    // Only at this API layer can we tell the design block about its actual library nickname.
    if( aModule )
    {
        // remove "const"-ness, I really do want to set nickname without
        // having to copy the LIB_ID and its two strings, twice each.
        LIB_ID& dbid = (LIB_ID&) aModule->GetLibId();

        // Catch any misbehaving plugin, which should be setting internal design block name
        // properly:
        Q_ASSERT( aDesignBlockName == dbid.GetLibItemName() );

        // and clearing nickname
        Q_ASSERT( !dbid.GetLibNickname().size() );

        dbid.SetLibNickname( aNickname );
    }
}


const DESIGN_BLOCK*
DESIGN_BLOCK_LIB_TABLE::GetEnumeratedDesignBlock( const QString& aNickname,
                                                  const QString& aDesignBlockName,
                                                  const LOCALE_IO* aLocale )
{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );

    if( !aLocale )
    {
        LOCALE_IO toggle_locale;

        return row->plugin->GetEnumeratedDesignBlock( row->GetFullURI( true ), aDesignBlockName,
                                                      row->GetProperties() );
    }
    else
    {
        return row->plugin->GetEnumeratedDesignBlock( row->GetFullURI( true ), aDesignBlockName,
                                                      row->GetProperties() );
    }
}


bool DESIGN_BLOCK_LIB_TABLE::DesignBlockExists( const QString& aNickname,
                                                const QString& aDesignBlockName )
{
    // NOT THREAD-SAFE!  LOCALE_IO is global!

    LOCALE_IO toggle_locale;

    try
    {
        const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
        Q_ASSERT( row->plugin );

        return row->plugin->DesignBlockExists( row->GetFullURI( true ), aDesignBlockName,
                                               row->GetProperties() );
    }
    catch( ... )
    {
        return false;
    }
}


DESIGN_BLOCK* DESIGN_BLOCK_LIB_TABLE::DesignBlockLoad( const QString& aNickname,
                                                       const QString& aDesignBlockName,
                                                       bool            aKeepUUID )
{
    // NOT THREAD-SAFE!  LOCALE_IO is global!

    LOCALE_IO toggle_locale;

    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );

    DESIGN_BLOCK* ret = row->plugin->DesignBlockLoad( row->GetFullURI( true ), aDesignBlockName,
                                                      aKeepUUID, row->GetProperties() );

    setLibNickname( ret, row->GetNickName(), aDesignBlockName );

    return ret;
}


DESIGN_BLOCK_LIB_TABLE::SAVE_T
DESIGN_BLOCK_LIB_TABLE::DesignBlockSave( const QString&     aNickname,
                                         const DESIGN_BLOCK* aDesignBlock, bool aOverwrite )
{
    // NOT THREAD-SAFE!  LOCALE_IO is global!

    LOCALE_IO toggle_locale;

    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );

    if( !aOverwrite )
    {
        // Try loading the design block to see if it already exists, caller wants overwrite
        // protection, which is atypical, not the default.

        QString DesignBlockname = aDesignBlock->GetLibId().GetLibItemName();

        std::unique_ptr<DESIGN_BLOCK> design_block( row->plugin->DesignBlockLoad(
                row->GetFullURI( true ), DesignBlockname, row->GetProperties() ) );

        if( design_block )
            return SAVE_SKIPPED;
    }

    row->plugin->DesignBlockSave( row->GetFullURI( true ), aDesignBlock, row->GetProperties() );

    return SAVE_OK;
}


void DESIGN_BLOCK_LIB_TABLE::DesignBlockDelete( const QString& aNickname,
                                                const QString& aDesignBlockName )

{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );
    return row->plugin->DesignBlockDelete( row->GetFullURI( true ), aDesignBlockName,
                                           row->GetProperties() );
}


bool DESIGN_BLOCK_LIB_TABLE::IsDesignBlockLibWritable( const QString& aNickname )
{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );
    return row->plugin->IsLibraryWritable( row->GetFullURI( true ) );
}


void DESIGN_BLOCK_LIB_TABLE::DesignBlockLibDelete( const QString& aNickname )
{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );
    row->plugin->DeleteLibrary( row->GetFullURI( true ), row->GetProperties() );
}


void DESIGN_BLOCK_LIB_TABLE::DesignBlockLibCreate( const QString& aNickname )
{
    const DESIGN_BLOCK_LIB_TABLE_ROW* row = FindRow( aNickname, true );
    Q_ASSERT( row->plugin );
    row->plugin->CreateLibrary( row->GetFullURI( true ), row->GetProperties() );
}


DESIGN_BLOCK*
DESIGN_BLOCK_LIB_TABLE::DesignBlockLoadWithOptionalNickname( const LIB_ID& aDesignBlockId,
                                                             bool          aKeepUUID )
{
    QString nickname = aDesignBlockId.GetLibNickname();
    QString DesignBlockname = aDesignBlockId.GetLibItemName();

    if( nickname.size() )
    {
        return DesignBlockLoad( nickname, DesignBlockname, aKeepUUID );
    }

    // nickname is empty, sequentially search (alphabetically) all libs/nicks for first match:
    else
    {
        // Search each library going through libraries alphabetically.
        for( const QString& library : GetLogicalLibs() )
        {
            // DesignBlockLoad() returns NULL on not found, does not throw exception
            // unless there's an IO_ERROR.
            DESIGN_BLOCK* ret = DesignBlockLoad( library, DesignBlockname, aKeepUUID );

            if( ret )
                return ret;
        }

        return nullptr;
    }
}


const QString DESIGN_BLOCK_LIB_TABLE::GlobalPathEnvVariableName()
{
    return ENV_VAR::GetVersionedEnvVarName( "DESIGN_BLOCK_DIR" );
}


class PCM_DESIGN_BLOCK_LIB_TRAVERSER final
{
public:
    explicit PCM_DESIGN_BLOCK_LIB_TRAVERSER( const QString& aPath, DESIGN_BLOCK_LIB_TABLE& aTable,
                                             const QString& aPrefix ) :
            m_lib_table( aTable ),
            m_path_prefix( aPath ),
            m_lib_prefix( aPrefix )
    {
        QFileInfo f( aPath );
        m_prefix_dir_count = f.absolutePath().split( '/' ).size();
    }

    void traverseDirectory( const QString& dirPath )
    {
        QFileInfo dir( dirPath );

        // consider a directory to be a lib if it's name ends with the design block lib dir
        // extension it is under $KICADn_3RD_PARTY/design_blocks/<pkgid>/ i.e. has nested
        // level of at least +3.
        if( dirPath.endsWith( QString( ".%1" ).arg( FILEEXT::KiCadDesignBlockLibPathExtension ) )
            && dir.absolutePath().split( '/' ).size() >= m_prefix_dir_count + 3 )
        {
            QString versionedPath = QString( "${%1}" ).arg( ENV_VAR::GetVersionedEnvVarName( "3RD_PARTY" ) );

            QStringList parts = dir.absolutePath().split( '/' );
            for( int i = 0; i < m_prefix_dir_count; ++i )
                parts.removeFirst();
            parts.prepend( versionedPath );

            QString libPath = parts.join( '/' );

            if( !m_lib_table.HasLibraryWithPath( libPath ) )
            {
                QString name = parts.last().left( parts.last().length() - 7 );
                QString nickname = QString( "%1%2" ).arg( m_lib_prefix ).arg( name );

                if( m_lib_table.HasLibrary( nickname ) )
                {
                    int increment = 1;

                    do
                    {
                        nickname = QString( "%1%2_%3" ).arg( m_lib_prefix ).arg( name ).arg( increment++ );
                    } while( m_lib_table.HasLibrary( nickname ) );
                }

                m_lib_table.InsertRow( new DESIGN_BLOCK_LIB_TABLE_ROW( nickname, libPath, "KiCad",
                                                                       QString(),
                                                                       _( "Added by Plugin and Content Manager" ) ) );
            }
        }
    }

private:
    DESIGN_BLOCK_LIB_TABLE& m_lib_table;
    QString                 m_path_prefix;
    QString                 m_lib_prefix;
    size_t                  m_prefix_dir_count;
};


bool DESIGN_BLOCK_LIB_TABLE::LoadGlobalTable( DESIGN_BLOCK_LIB_TABLE& aTable )
{
    bool       tableExists = true;
    QFileInfo fn( GetGlobalTableFileName() );

    if( !fn.exists() )
    {
        tableExists = false;

        QDir dir;
        if( !dir.exists( fn.path() ) && !dir.mkpath( fn.path() ) )
        {
            THROW_IO_ERROR( QString( _( "Cannot create global library table path '%1'." ) )
                                              .arg( fn.path() ) );
        }

        // Attempt to copy the default global file table from the KiCad
        // template folder to the user's home configuration path.
        SEARCH_STACK ss;

        SystemDirsAppend( &ss );

        const ENV_VAR_MAP&      envVars = Pgm().GetLocalEnvVariables();
        std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( envVars, "TEMPLATE_DIR" );

        if( v && !v->isEmpty() )
            ss.AddPaths( v->toStdString(), 0 );

        std::string fileName = ss.FindValidPath( FILEEXT::DesignBlockLibraryTableFileName );

        // The fallback is to create an empty global design block table for the user to populate.
        if( fileName.empty() || !QFile::copy( QString::fromStdString( fileName ), fn.absoluteFilePath() ) )
        {
            DESIGN_BLOCK_LIB_TABLE emptyTable;

            emptyTable.Save( fn.absoluteFilePath() );
        }
    }

    aTable.clear();
    aTable.Load( fn.absoluteFilePath() );

    SETTINGS_MANAGER& mgr = Pgm().GetSettingsManager();
    KICAD_SETTINGS*   settings = mgr.GetAppSettings<KICAD_SETTINGS>( "kicad" );

    const ENV_VAR_MAP& env = Pgm().GetLocalEnvVariables();
    QString           packagesPath;

    if( std::optional<QString> v = ENV_VAR::GetVersionedEnvVarValue( env, "3RD_PARTY" ) )
        packagesPath = *v;

    if( settings->m_PcmLibAutoAdd )
    {
        // Scan for libraries in PCM packages directory

        QDir d( packagesPath );
        d.cd( "design_blocks" );

        if( d.exists() )
        {
            PCM_DESIGN_BLOCK_LIB_TRAVERSER traverser( packagesPath, aTable, settings->m_PcmLibPrefix );
            
            QDirIterator it( d.absolutePath(), QDir::Dirs | QDir::NoDotAndDotDot, QDirIterator::Subdirectories );
            while( it.hasNext() )
            {
                traverser.traverseDirectory( it.next() );
            }
        }
    }

    if( settings->m_PcmLibAutoRemove )
    {
        // Remove PCM libraries that no longer exist
        std::vector<std::string> to_remove;

        for( size_t i = 0; i < aTable.GetCount(); i++ )
        {
            LIB_TABLE_ROW& row = aTable.At( i );
            QString       path = row.GetFullURI( true );

            if( path.startsWith( packagesPath ) && !QDir( path ).exists() )
                to_remove.push_back( row.GetNickName().toStdString() );
        }

        for( const std::string& nickName : to_remove )
            aTable.RemoveRow( aTable.FindRow( QString::fromStdString( nickName ) ) );
    }

    return tableExists;
}


DESIGN_BLOCK_LIB_TABLE& DESIGN_BLOCK_LIB_TABLE::GetGlobalLibTable()
{
    return GDesignBlockTable;
}


DESIGN_BLOCK_LIST_IMPL& DESIGN_BLOCK_LIB_TABLE::GetGlobalList()
{
    return GDesignBlockList;
}


QString DESIGN_BLOCK_LIB_TABLE::GetGlobalTableFileName()
{
    QDir dir( PATHS::GetUserSettingsPath() );
    return dir.filePath( QString::fromStdString( FILEEXT::DesignBlockLibraryTableFileName ) );
}
