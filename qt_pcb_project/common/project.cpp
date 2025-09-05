
#include <map>
#include <vector>
#include <QtCore/QStandardPaths>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <kiplatform/environment.h>

#include <pgm_base.h>
#include <confirm.h>
#include <core/kicad_algo.h>
#include <design_block_lib_table.h>
#include <fp_lib_table.h>
#include <string_utils.h>
#include <kiface_ids.h>
#include <kiway.h>
#include <macros.h>
#include <project.h>
#include <project/project_file.h>
#include <trace_helpers.h>
#include <wildcards_and_files_ext.h>
#include <settings/common_settings.h>
#include <settings/settings_manager.h>
#include <title_block.h>

PROJECT::PROJECT() :
        m_readOnly( false ),
        m_textVarsTicker( 0 ),
        m_netclassesTicker( 0 ),
        m_projectFile( nullptr ),
        m_localSettings( nullptr )
{
    m_elems.fill( nullptr );
}


void PROJECT::ElemsClear()
{
    // careful here, this should work, but the virtual destructor may not
    // be in the same link image as PROJECT.
    for( unsigned i = 0;  i < m_elems.size();  ++i )
    {
        SetElem( static_cast<PROJECT::ELEM>( i ), nullptr );
    }
}


PROJECT::~PROJECT()
{
    ElemsClear();
}


bool PROJECT::TextVarResolver( QString* aToken ) const
{
    if( *aToken == "PROJECTNAME" )
    {
        *aToken = GetProjectName();
        return true;
    }
    else if( *aToken == "CURRENT_DATE" )
    {
        *aToken = TITLE_BLOCK::GetCurrentDate();
        return true;
    }
    else if( GetTextVars().count( *aToken ) > 0 )
    {
        *aToken = GetTextVars().at( *aToken );
        return true;
    }

    return false;
}


std::map<QString, QString>& PROJECT::GetTextVars() const
{
    return GetProjectFile().m_TextVars;
}


void PROJECT::ApplyTextVars( const std::map<QString, QString>& aVarsMap )
{
    if( aVarsMap.size() == 0 )
        return;

    std::map<QString, QString>& existingVarsMap = GetTextVars();

    for( auto it = aVarsMap.begin(); it != aVarsMap.end(); ++it )
    {
        existingVarsMap[it->first] = it->second;
    }
}


void PROJECT::setProjectFullName( const QString& aFullPathAndName )
{
    QFileInfo candidate_path( aFullPathAndName );

    if( m_project_name.absoluteFilePath() != candidate_path.absoluteFilePath() )
    {
        Clear();

        qDebug() << __func__ << ": old:" << GetProjectFullName() << " new:" << aFullPathAndName;

        m_project_name = QFileInfo( aFullPathAndName );

        Q_ASSERT( m_project_name.isAbsolute() );

        Q_ASSERT( m_project_name.suffix() == FILEEXT::ProjectFileExtension );
    }
}


const QString PROJECT::GetProjectFullName() const
{
    return m_project_name.absoluteFilePath();
}


const QString PROJECT::GetProjectPath() const
{
    return m_project_name.absolutePath() + QDir::separator();
}


const QString PROJECT::GetProjectDirectory() const
{
    return m_project_name.absolutePath();
}


const QString PROJECT::GetProjectName() const
{
    return m_project_name.baseName();
}


bool PROJECT::IsNullProject() const
{
    return m_project_name.baseName().isEmpty();
}


const QString PROJECT::SymbolLibTableName() const
{
    return libTableName( FILEEXT::SymbolLibraryTableFileName );
}


const QString PROJECT::FootprintLibTblName() const
{
    return libTableName( FILEEXT::FootprintLibraryTableFileName );
}


const QString PROJECT::DesignBlockLibTblName() const
{
    return libTableName( FILEEXT::DesignBlockLibraryTableFileName );
}


void PROJECT::PinLibrary( const QString& aLibrary, enum LIB_TYPE_T aLibType )
{
    COMMON_SETTINGS*     cfg = Pgm().GetCommonSettings();
    std::vector<QString>* pinnedLibsCfg = nullptr;
    std::vector<QString>* pinnedLibsFile = nullptr;

    switch( aLibType )
    {
    case LIB_TYPE_T::SYMBOL_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedSymbolLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_symbol_libs;
        break;
    case LIB_TYPE_T::FOOTPRINT_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedFootprintLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_fp_libs;
        break;
    case LIB_TYPE_T::DESIGN_BLOCK_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedDesignBlockLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_design_block_libs;
        break;
    default:
        Q_ASSERT_X( false, "PinLibrary", "Cannot pin library: invalid library type" );
        return;
    }

    if( !alg::contains( *pinnedLibsFile, aLibrary ) )
        pinnedLibsFile->push_back( aLibrary );

    Pgm().GetSettingsManager().SaveProject();

    if( !alg::contains( *pinnedLibsCfg, aLibrary ) )
        pinnedLibsCfg->push_back( aLibrary );

    cfg->SaveToFile( Pgm().GetSettingsManager().GetPathForSettingsFile( cfg ) );
}


void PROJECT::UnpinLibrary( const QString& aLibrary, enum LIB_TYPE_T aLibType )
{
    COMMON_SETTINGS*     cfg = Pgm().GetCommonSettings();
    std::vector<QString>* pinnedLibsCfg = nullptr;
    std::vector<QString>* pinnedLibsFile = nullptr;

    switch( aLibType )
    {
    case LIB_TYPE_T::SYMBOL_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedSymbolLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_symbol_libs;
        break;
    case LIB_TYPE_T::FOOTPRINT_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedFootprintLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_fp_libs;
        break;
    case LIB_TYPE_T::DESIGN_BLOCK_LIB:
        pinnedLibsFile = &m_projectFile->m_PinnedDesignBlockLibs;
        pinnedLibsCfg = &cfg->m_Session.pinned_design_block_libs;
        break;
    default:
        Q_ASSERT_X( false, "UnpinLibrary", "Cannot unpin library: invalid library type" );
        return;
    }

    alg::delete_matching( *pinnedLibsFile, aLibrary );
    Pgm().GetSettingsManager().SaveProject();

    alg::delete_matching( *pinnedLibsCfg, aLibrary );
    cfg->SaveToFile( Pgm().GetSettingsManager().GetPathForSettingsFile( cfg ) );
}


const QString PROJECT::libTableName( const QString& aLibTableName ) const
{
    QFileInfo fn( GetProjectFullName() );
    QString path = fn.absolutePath();

    QDir dir( path );
    if( path.isEmpty() || !fn.exists() || !QFileInfo( path ).isWritable() )
    {
#ifdef __WXMAC__
        path = KIPLATFORM::ENV::GetUserConfigPath();
#else
        path = QStandardPaths::writableLocation( QStandardPaths::TempLocation );
#endif

#if defined( __WINDOWS__ )
        path += QDir::separator() + "kicad";
#endif

        QDir dir( path );
        fn.setFile( dir, "prj-" + aLibTableName );
    }
    else
    {
        fn.setFile( dir, aLibTableName );
    }

    return fn.absoluteFilePath();
}


const QString PROJECT::GetSheetName( const KIID& aSheetID )
{
    if( m_sheetNames.empty() )
    {
        for( const auto& pair : GetProjectFile().GetSheets() )
            m_sheetNames[pair.first] = pair.second;
    }

    if( m_sheetNames.contains( aSheetID ) )
        return m_sheetNames.value( aSheetID );
    else
        return aSheetID.AsString();
}


void PROJECT::SetRString( RSTRING_T aIndex, const QString& aString )
{
    unsigned ndx = unsigned( aIndex );

    if( ndx < m_rstrings.size() )
        m_rstrings[ndx] = aString;
    else
        Q_ASSERT( false );
}


const QString& PROJECT::GetRString( RSTRING_T aIndex )
{
    unsigned ndx = unsigned( aIndex );

    if( ndx < m_rstrings.size() )
    {
        return m_rstrings[ndx];
    }
    else
    {
        static QString no_cookie_for_you;

        Q_ASSERT( false );

        return no_cookie_for_you;
    }
}


PROJECT::_ELEM* PROJECT::GetElem( PROJECT::ELEM aIndex )
{
    // This is virtual, so implement it out of line

    if( static_cast<unsigned>( aIndex ) < m_elems.size() )
        return m_elems[static_cast<unsigned>( aIndex )];

    return nullptr;
}


void PROJECT::SetElem( PROJECT::ELEM aIndex, _ELEM* aElem )
{
    // This is virtual, so implement it out of line
    if( static_cast<unsigned>( aIndex ) < m_elems.size() )
    {
        delete m_elems[static_cast<unsigned>(aIndex)];
        m_elems[static_cast<unsigned>( aIndex )] = aElem;
    }
}


const QString PROJECT::AbsolutePath( const QString& aFileName ) const
{
    QFileInfo fn( aFileName );

    if( aFileName.startsWith( "${" ) )
        return aFileName;

    if( !fn.isAbsolute() )
    {
        QString pro_dir = QFileInfo( GetProjectFullName() ).absolutePath();
        fn.setFile( pro_dir, aFileName );
    }

    return fn.absoluteFilePath();
}


FP_LIB_TABLE* PROJECT::PcbFootprintLibs( KIWAY& aKiway )
{
    // This is a lazy loading function, it loads the project specific table when
    // that table is asked for, not before.

    FP_LIB_TABLE* tbl = (FP_LIB_TABLE*) GetElem( PROJECT::ELEM::FPTBL );

    if( tbl )
    {
        Q_ASSERT( tbl->ProjectElementType() == PROJECT::ELEM::FPTBL );
    }
    else
    {
        try
        {
            // Build a new project specific FP_LIB_TABLE with the global table as a fallback.
            // ~FP_LIB_TABLE() will not touch the fallback table, so multiple projects may
            // stack this way, all using the same global fallback table.
            KIFACE* kiface = aKiway.KiFACE( KIWAY::FACE_PCB );

            tbl = (FP_LIB_TABLE*) kiface->IfaceOrAddress( KIFACE_NEW_FOOTPRINT_TABLE );
            tbl->Load( FootprintLibTblName() );

            SetElem( PROJECT::ELEM::FPTBL, tbl );
        }
        catch( const IO_ERROR& ioe )
        {
            DisplayErrorMessage( nullptr, _( "Error loading project footprint library table." ),
                                 ioe.What() );
        }
        catch( ... )
        {
            DisplayErrorMessage( nullptr, _( "Error loading project footprint library table." ) );
        }
    }

    return tbl;
}


DESIGN_BLOCK_LIB_TABLE* PROJECT::DesignBlockLibs()
{
    // This is a lazy loading function, it loads the project specific table when
    // that table is asked for, not before.

    DESIGN_BLOCK_LIB_TABLE* tbl = (DESIGN_BLOCK_LIB_TABLE*) GetElem( ELEM::DESIGN_BLOCK_LIB_TABLE );

    if( tbl )
    {
        Q_ASSERT( tbl->ProjectElementType() == PROJECT::ELEM::DESIGN_BLOCK_LIB_TABLE );
    }
    else
    {
        try
        {
            tbl = new DESIGN_BLOCK_LIB_TABLE( &DESIGN_BLOCK_LIB_TABLE::GetGlobalLibTable() );
            tbl->Load( DesignBlockLibTblName() );

            SetElem( ELEM::DESIGN_BLOCK_LIB_TABLE, tbl );
        }
        catch( const IO_ERROR& ioe )
        {
            DisplayErrorMessage( nullptr, _( "Error loading project design block library table." ),
                                 ioe.What() );
        }
        catch( ... )
        {
            DisplayErrorMessage( nullptr,
                                 _( "Error loading project design block library table." ) );
        }
    }

    return tbl;
}
