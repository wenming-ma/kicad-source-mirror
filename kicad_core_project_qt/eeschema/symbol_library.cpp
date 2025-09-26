
#include <algorithm>
#include <kiface_base.h>
#include <eda_base_frame.h>
#include <string_utils.h>
#include <macros.h>
#include <richio.h>
#include <wildcards_and_files_ext.h>
#include <project/project_file.h>
#include <project_rescue.h>
#include <project_sch.h>
#include <widgets/app_progress_dialog.h>

#include <symbol_library.h>
#include <sch_io/kicad_legacy/sch_io_kicad_legacy.h>

#include <QtCore/QDateTime>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QDebug>
#include <QtCore/QtGlobal>
// UNUSED_SYMBOL: MigrateSimModel<LIB_SYMBOL> - Template specialization not available in minimal project
// #include "sim/sim_model.h"

SYMBOL_LIB::SYMBOL_LIB( SCH_LIB_TYPE aType, const QString& aFileName,
                        SCH_IO_MGR::SCH_FILE_T aPluginType ) :
    m_pluginType( aPluginType )
{
    type = aType;
    isModified = false;
    // Fix: timeStamp is QDateTime, not int - set to current time directly
    timeStamp = QDateTime::currentDateTime();
    versionMajor = 0;       // Will be updated after reading the lib file
    versionMinor = 0;       // Will be updated after reading the lib file

    // Fix: fileName is QFileInfo, need to construct from QString
    fileName = QFileInfo(aFileName);

    // Fix: Check if QFileInfo path is empty, then create new QFileInfo
    if( fileName.filePath().isEmpty() )
        fileName = QFileInfo("unnamed.lib");

    // UNUSED_SYMBOL: ?FindPlugin@SCH_IO_MGR@@SAPEAVSCH_IO@@W4SCH_FILE_T - Implementation not available in minimal project
    // m_plugin.reset( SCH_IO_MGR::FindPlugin( m_pluginType ) );
    m_properties = std::make_unique<std::map<std::string, UTF8>>();
    m_mod_hash = 0;
}


SYMBOL_LIB::~SYMBOL_LIB()
{
}


void SYMBOL_LIB::Save( bool aSaveDocFile )
{
    Q_ASSERT_X( m_plugin != nullptr, "SYMBOL_LIB::Save",
                QString( "no plugin defined for library `%1`." )
                .arg( fileName.absoluteFilePath() ).toLocal8Bit().constData() );
    if( m_plugin == nullptr ) return;

    std::map<std::string, UTF8> props;

    // UNUSED_SYMBOL: PropNoDocFile - Static property not available in minimal compilation set
    // if( !aSaveDocFile )
    //     props[ SCH_IO_KICAD_LEGACY::PropNoDocFile ] = "";

    // Fix: SaveLibrary expects QString, not QFileInfo
    m_plugin->SaveLibrary( fileName.absoluteFilePath(), &props );
    isModified = false;
}


void SYMBOL_LIB::Create( const QString& aFileName )
{
    // Fix: Convert QFileInfo to QString
    QString tmpFileName = fileName.absoluteFilePath();

    if( !aFileName.isEmpty() )
        tmpFileName = aFileName;

    m_plugin->CreateLibrary( tmpFileName, m_properties.get() );
}


void SYMBOL_LIB::SetPluginType( SCH_IO_MGR::SCH_FILE_T aPluginType )
{
    if( m_pluginType != aPluginType )
    {
        m_pluginType = aPluginType;
        // UNUSED_SYMBOL: ?FindPlugin@SCH_IO_MGR@@SAPEAVSCH_IO@@W4SCH_FILE_T - Implementation not available in minimal project
        // m_plugin.reset( SCH_IO_MGR::FindPlugin( m_pluginType ) );
    }
}


bool SYMBOL_LIB::IsCache() const
{
    // UNUSED_SYMBOL: PropNoDocFile - Static property not available in minimal compilation set
    // return m_properties->contains( SCH_IO_KICAD_LEGACY::PropNoDocFile );
    return false; // Default to non-cache behavior
}


void SYMBOL_LIB::SetCache()
{
    // UNUSED_SYMBOL: PropNoDocFile - Static property not available in minimal compilation set
    // (*m_properties)[ SCH_IO_KICAD_LEGACY::PropNoDocFile ] = "";
}


// UNUSED_SYMBOL: PropBuffering - Method depends on unused PropBuffering property
/*
bool SYMBOL_LIB::IsBuffering() const
{
    return m_properties->contains( SCH_IO_KICAD_LEGACY::PropBuffering );
}
*/


// UNUSED_SYMBOL: PropBuffering - Method depends on unused PropBuffering property
/*
void SYMBOL_LIB::EnableBuffering( bool aEnable )
{
    if( aEnable )
        (*m_properties)[ SCH_IO_KICAD_LEGACY::PropBuffering ] = "";
    else
        m_properties->erase( SCH_IO_KICAD_LEGACY::PropBuffering );
}
*/


void SYMBOL_LIB::GetSymbolNames( QStringList& aNames ) const
{
    // Fix: EnumerateSymbolLib expects QString, not QFileInfo
    m_plugin->EnumerateSymbolLib( aNames, fileName.absoluteFilePath(), m_properties.get() );

    aNames.sort();
}


void SYMBOL_LIB::GetSymbols( std::vector<LIB_SYMBOL*>& aSymbols ) const
{
    // Fix: EnumerateSymbolLib expects QString, and use absoluteFilePath() instead of GetFullPath()
    m_plugin->EnumerateSymbolLib( aSymbols, fileName.absoluteFilePath(), m_properties.get() );

    std::sort( aSymbols.begin(), aSymbols.end(),
            [](LIB_SYMBOL *lhs, LIB_SYMBOL *rhs) -> bool
            {
                return lhs->GetName() < rhs->GetName();
            } );
}


LIB_SYMBOL* SYMBOL_LIB::FindSymbol( const QString& aName ) const
{
    // Fix: LoadSymbol expects QString, not QFileInfo
    LIB_SYMBOL* symbol = m_plugin->LoadSymbol( fileName.absoluteFilePath(), aName, m_properties.get() );

    if( symbol )
    {
        // Set the library to this even though technically the legacy cache plugin owns the
        // symbols.  This allows the symbol library table conversion tool to determine the
        // correct library where the symbol was found.
        if( !symbol->GetLib() )
            symbol->SetLib( const_cast<SYMBOL_LIB*>( this ) );

        // UNUSED_SYMBOL: MigrateSimModel<LIB_SYMBOL> - Template specialization not available in minimal project
        // SIM_MODEL::MigrateSimModel<LIB_SYMBOL>( *symbol, nullptr );
    }

    return symbol;
}


LIB_SYMBOL* SYMBOL_LIB::FindSymbol( const LIB_ID& aLibId ) const
{
    return FindSymbol( QString::fromStdString( aLibId.Format() ) );
}


void SYMBOL_LIB::AddSymbol( LIB_SYMBOL* aSymbol )
{
    // add a clone, not the caller's copy, the plugin take ownership of the new symbol.
    // Fix: SaveSymbol expects QString, not QFileInfo
    m_plugin->SaveSymbol( fileName.absoluteFilePath(),
                          new LIB_SYMBOL( *aSymbol->SharedPtr().get(), this ),
                          m_properties.get() );

    // If we are not buffering, the library file is updated immediately when the plugin
    // SaveSymbol() function is called.
    // UNUSED_SYMBOL: IsBuffering - Method not available, always assume buffering
    // if( IsBuffering() )
        isModified = true;

    ++m_mod_hash;
}


LIB_SYMBOL* SYMBOL_LIB::RemoveSymbol( LIB_SYMBOL* aEntry )
{
    Q_ASSERT_X( aEntry != nullptr, "SYMBOL_LIB::RemoveSymbol", "NULL pointer cannot be removed from library." );
    if( aEntry == nullptr ) return nullptr;

    // Fix: DeleteSymbol expects QString, not QFileInfo
    m_plugin->DeleteSymbol( fileName.absoluteFilePath(), aEntry->GetName(), m_properties.get() );

    // If we are not buffering, the library file is updated immediately when the plugin
    // SaveSymbol() function is called.
    // UNUSED_SYMBOL: IsBuffering - Method not available, always assume buffering
    // if( IsBuffering() )
        isModified = true;

    ++m_mod_hash;
    return nullptr;
}


LIB_SYMBOL* SYMBOL_LIB::ReplaceSymbol( LIB_SYMBOL* aOldSymbol, LIB_SYMBOL* aNewSymbol )
{
    Q_ASSERT( aOldSymbol != nullptr );
    Q_ASSERT( aNewSymbol != nullptr );

    // Fix: DeleteSymbol expects QString, not QFileInfo
    m_plugin->DeleteSymbol( fileName.absoluteFilePath(), aOldSymbol->GetName(), m_properties.get() );

    LIB_SYMBOL* my_part = new LIB_SYMBOL( *aNewSymbol, this );

    // Fix: SaveSymbol expects QString, not QFileInfo
    m_plugin->SaveSymbol( fileName.absoluteFilePath(), my_part, m_properties.get() );

    // If we are not buffering, the library file is updated immediately when the plugin
    // SaveSymbol() function is called.
    // UNUSED_SYMBOL: IsBuffering - Method not available, always assume buffering
    // if( IsBuffering() )
        isModified = true;

    ++m_mod_hash;
    return my_part;
}


SYMBOL_LIB* SYMBOL_LIB::LoadSymbolLibrary( const QString& aFileName )
{
    std::unique_ptr<SYMBOL_LIB> lib = std::make_unique<SYMBOL_LIB>( SCH_LIB_TYPE::LT_EESCHEMA,
                                                                    aFileName );

    std::vector<LIB_SYMBOL*> parts;
    // This loads the library.
    lib->GetSymbols( parts );

    // Now, set the LIB_SYMBOL m_library member but it will only be used
    // when loading legacy libraries in the future. Once the symbols in the
    // schematic have a full #LIB_ID, this will not get called.
    for( size_t ii = 0; ii < parts.size(); ii++ )
    {
        LIB_SYMBOL* part = parts[ii];

        part->SetLib( lib.get() );
    }

    SYMBOL_LIB* ret = lib.release();
    return ret;
}


SYMBOL_LIB* SYMBOL_LIBS::AddLibrary( const QString& aFileName )
{
    SYMBOL_LIB* lib;

    QFileInfo fn( aFileName );
    // Don't reload the library if it is already loaded.
    lib = FindLibrary( fn.baseName() );

    if( lib )
        return lib;

    try
    {
        lib = SYMBOL_LIB::LoadSymbolLibrary( aFileName );
        push_back( lib );

        return lib;
    }
    catch( ... )
    {
        return nullptr;
    }
}


SYMBOL_LIB* SYMBOL_LIBS::AddLibrary( const QString& aFileName, SYMBOL_LIBS::iterator& aIterator )
{
    // Don't reload the library if it is already loaded.
    QFileInfo fn( aFileName );
    SYMBOL_LIB* lib = FindLibrary( fn.baseName() );

    if( lib )
        return lib;

    try
    {
        lib = SYMBOL_LIB::LoadSymbolLibrary( aFileName );

        if( aIterator >= begin() && aIterator < end() )
            insert( aIterator, lib );
        else
            push_back( lib );

        return lib;
    }
    catch( ... )
    {
        return nullptr;
    }
}


bool SYMBOL_LIBS::ReloadLibrary( const QString &aFileName )
{
    QFileInfo  fn( aFileName );
    SYMBOL_LIB* lib = FindLibrary( fn.baseName() );

    // Check if the library already exists.
    if( !lib )
        return false;

    // Create a clone of the library pointer in case we need to re-add it
    SYMBOL_LIB *cloneLib = lib;

    // Try to find the iterator of the library
    for( auto it = begin(); it != end(); ++it )
    {
        if( it->GetName() == fn.baseName() )
        {
            // Remove the old library and keep the pointer
            lib = &*it;
            release( it );
            break;
        }
    }

    // Try to reload the library
    try
    {
        lib = SYMBOL_LIB::LoadSymbolLibrary( aFileName );

        // If the library is successfully reloaded, add it back to the set.
        push_back( lib );
        return true;
    }
    catch( ... )
    {
        // If an exception occurs, ensure that the SYMBOL_LIBS remains unchanged
        // by re-adding the old library back to the set.
        push_back( cloneLib );
        return false;
    }
}


SYMBOL_LIB* SYMBOL_LIBS::FindLibrary( const QString& aName )
{
    for( SYMBOL_LIBS::iterator it = begin();  it!=end();  ++it )
    {
        if( it->GetName() == aName )
            return &*it;
    }

    return nullptr;
}


SYMBOL_LIB* SYMBOL_LIBS::GetCacheLibrary()
{
    for( SYMBOL_LIBS::iterator it = begin();  it!=end();  ++it )
    {
        if( it->IsCache() )
            return &*it;
    }

    return nullptr;
}


SYMBOL_LIB* SYMBOL_LIBS::FindLibraryByFullFileName( const QString& aFullFileName )
{
    for( SYMBOL_LIBS::iterator it = begin();  it!=end();  ++it )
    {
        if( it->GetFullFileName() == aFullFileName )
            return &*it;
    }

    return nullptr;
}


QStringList SYMBOL_LIBS::GetLibraryNames( bool aSorted )
{
    QStringList cacheNames;
    QStringList names;

    for( SYMBOL_LIB& lib : *this )
    {
        if( lib.IsCache() && aSorted )
            cacheNames.append( lib.GetName() );
        else
            names.append( lib.GetName() );
    }

    // Even sorted, the cache library is always at the end of the list.
    if( aSorted )
        names.sort();

    for( int i = 0; i < cacheNames.size(); i++ )
        names.append( cacheNames.at( i ) );

    return names;
}


LIB_SYMBOL* SYMBOL_LIBS::FindLibSymbol( const LIB_ID& aLibId, const QString& aLibraryName )
{
    LIB_SYMBOL* part = nullptr;

    for( SYMBOL_LIB& lib : *this )
    {
        if( !aLibraryName.isEmpty() && lib.GetName() != aLibraryName )
            continue;

        part = lib.FindSymbol( QString::fromStdString( aLibId.GetLibItemName() ) );

        if( part )
            break;
    }

    return part;
}


void SYMBOL_LIBS::FindLibraryNearEntries( std::vector<LIB_SYMBOL*>& aCandidates,
                                          const QString& aEntryName,
                                          const QString& aLibraryName )
{
    for( SYMBOL_LIB& lib : *this )
    {
        if( !aLibraryName.isEmpty() && lib.GetName() != aLibraryName )
            continue;

        QStringList partNames;

        lib.GetSymbolNames( partNames );

        if( partNames.isEmpty() )
            continue;

        for( size_t i = 0;  i < partNames.size();  i++ )
        {
            if( partNames[i].compare( aEntryName, Qt::CaseInsensitive ) == 0 )
                aCandidates.push_back( lib.FindSymbol( partNames[i] ) );
        }
    }
}


void SYMBOL_LIBS::GetLibNamesAndPaths( PROJECT* aProject, QString* aPaths, QStringList* aNames )
{
    Q_ASSERT_X( aProject != nullptr, "SYMBOL_LIBS::GetLibNamesAndPaths", "Null PROJECT in GetLibNamesAndPaths" );
    if( aProject == nullptr ) return;

    PROJECT_FILE& project = aProject->GetProjectFile();

    if( aPaths )
        *aPaths = QString::fromStdString( project.m_LegacyLibDir );

    if( aNames )
    {
        aNames->clear();
        for( const std::string& name : project.m_LegacyLibNames )
            aNames->append( QString::fromStdString( name ) );
    }
}


void SYMBOL_LIBS::SetLibNamesAndPaths( PROJECT* aProject, const QString& aPaths,
                                       const QStringList& aNames )
{
    Q_ASSERT_X( aProject != nullptr, "SYMBOL_LIBS::SetLibNamesAndPaths", "Null PROJECT in SetLibNamesAndPaths" );
    if( aProject == nullptr ) return;

    PROJECT_FILE& project = aProject->GetProjectFile();

    project.m_LegacyLibDir = aPaths.toStdString();
    project.m_LegacyLibNames.clear();
    for( const QString& name : aNames )
        project.m_LegacyLibNames.push_back( name.toStdString() );
}


const QString SYMBOL_LIBS::CacheName( const QString& aFullProjectFilename )
{
    QFileInfo fileinfo( aFullProjectFilename );
    QString   name = fileinfo.baseName();
    QString   dir = fileinfo.absolutePath();

    QFileInfo filename( dir + "/" + name + "-cache." + QString::fromStdString( FILEEXT::LegacySymbolLibFileExtension ) );

    if( filename.exists() )
        return filename.absoluteFilePath();

    // Try the old (2007) cache name
    QFileInfo oldFilename( dir + "/" + name + ".cache." + QString::fromStdString( FILEEXT::LegacySymbolLibFileExtension ) );

    if( oldFilename.exists() )
        return oldFilename.absoluteFilePath();

    return QString();
}


void SYMBOL_LIBS::LoadAllLibraries( PROJECT* aProject, bool aShowProgress )
{
    QString        filename;
    QString        libs_not_found;
    SEARCH_STACK*   lib_search = PROJECT_SCH::SchSearchS( aProject );

#if defined(DEBUG) && 0
    lib_search->Show( __func__ );
#endif

    QStringList   lib_names;

    GetLibNamesAndPaths( aProject, nullptr, &lib_names );

    // Post symbol library table, this should be empty.  Only the cache library should get loaded.
    if( !lib_names.isEmpty() )
    {
        APP_PROGRESS_DIALOG lib_dialog( "Loading Symbol Libraries",
                                        QString(),
                                        lib_names.size(),
                                        nullptr,
                                        false );

        if( aShowProgress )
        {
            lib_dialog.show();
        }

        for( int i = 0; i < lib_names.size();  ++i )
        {
            if( aShowProgress )
            {
                lib_dialog.Update( i, QString( "Loading %1..." ).arg( lib_names[i] ) );
            }

            // lib_names[] does not store the file extension. Set it.
            // Remember lib_names[i] can contain a '.' in name, so using a QFileInfo
            // before adding the extension can create incorrect full filename
            QString fullname = lib_names[i] + "." + QString::fromStdString( FILEEXT::LegacySymbolLibFileExtension );

            // Now the full name is set, we can use a QFileInfo.
            QFileInfo fn( fullname );

            // Skip if the file name is not valid..
            if( fullname.isEmpty() )
                continue;

            if( !fn.exists() )
            {
                filename = QString::fromStdString( lib_search->FindValidPath( fn.absoluteFilePath().toStdString() ) );

                if( filename.isEmpty() )
                {
                    libs_not_found += fn.absoluteFilePath();
                    libs_not_found += '\n';
                    continue;
                }
            }
            else
            {   // ensure the lib filename has a absolute path.
                // If the lib has no absolute path, and is found in the cwd by fn.exists(),
                // make a full absolute path, to avoid issues with load library functions which
                // expects an absolute path.
                if( !fn.isAbsolute() )
                    filename = fn.absoluteFilePath();
                else
                    filename = fn.absoluteFilePath();
            }

            try
            {
                AddLibrary( filename );
            }
            catch( const IO_ERROR& ioe )
            {
                QString msg = QString( "Symbol library '%1' failed to load." ).arg( filename );

                qDebug() << msg << "\n" << ioe.What();
            }
        }
    }

    // add the special cache library.
    QString cache_name = CacheName( aProject->GetProjectFullName() );
    SYMBOL_LIB* cache_lib;

    if( !aProject->IsNullProject() && !cache_name.isEmpty() )
    {
        try
        {
            cache_lib = AddLibrary( cache_name );

            if( cache_lib )
                cache_lib->SetCache();
        }
        catch( const IO_ERROR& ioe )
        {
            QString msg = QString( "Error loading symbol library '%1'.\n%2" )
                                             .arg( cache_name )
                                             .arg( ioe.What() );

            THROW_IO_ERROR( msg );
        }
    }

    // Print the libraries not found
    if( !libs_not_found.isEmpty() )
    {
        // Use a different exception type so catch()er can route to proper use
        // of the HTML_MESSAGE_BOX.
        THROW_PARSE_ERROR( QString(), __func__, TO_UTF8( libs_not_found ), 0, 0 );
    }
}
