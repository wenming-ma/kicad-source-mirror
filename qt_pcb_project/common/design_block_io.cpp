
#include <common.h>
#include <i18n_utility.h>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QString>
#include <QStringList>
#include <QDateTime>
#include <QIODevice>
#include <QThread>
#include <QCoreApplication>
#include <wildcards_and_files_ext.h>
#include <kiway_player.h>
#include <design_block_io.h>
#include <design_block.h>
#include <ki_exception.h>
#include <trace_helpers.h>
#include <fstream>

const QString DESIGN_BLOCK_IO_MGR::ShowType( DESIGN_BLOCK_FILE_T aFileType )
{
    switch( aFileType )
    {
    case KICAD_SEXP: return _("KiCad");
    default: return QString(_("UNKNOWN (%1)")).arg(aFileType);
    }
}


DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T
DESIGN_BLOCK_IO_MGR::EnumFromStr( const QString& aFileType )
{
    if( aFileType == _("KiCad") )
        return DESIGN_BLOCK_FILE_T( KICAD_SEXP );

    return DESIGN_BLOCK_FILE_T( DESIGN_BLOCK_FILE_UNKNOWN );
}


DESIGN_BLOCK_IO* DESIGN_BLOCK_IO_MGR::FindPlugin( DESIGN_BLOCK_FILE_T aFileType )
{
    switch( aFileType )
    {
    case KICAD_SEXP:          return new DESIGN_BLOCK_IO();
    default:                  return nullptr;
    }
}


DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T
DESIGN_BLOCK_IO_MGR::GuessPluginTypeFromLibPath( const QString& aLibPath, int aCtl )
{
    if( IO_RELEASER<DESIGN_BLOCK_IO>( FindPlugin( KICAD_SEXP ) )->CanReadLibrary( aLibPath )
            && aCtl != KICTL_NONKICAD_ONLY )
    {
        return KICAD_SEXP;
    }

    return DESIGN_BLOCK_IO_MGR::FILE_TYPE_NONE;
}


bool DESIGN_BLOCK_IO_MGR::ConvertLibrary( std::map<std::string, UTF8>* aOldFileProps,
                                          const QString&              aOldFilePath,
                                          const QString&              aNewFilePath )
{
    DESIGN_BLOCK_IO_MGR::DESIGN_BLOCK_FILE_T oldFileType =
            DESIGN_BLOCK_IO_MGR::GuessPluginTypeFromLibPath( aOldFilePath );

    if( oldFileType == DESIGN_BLOCK_IO_MGR::FILE_TYPE_NONE )
        return false;


    IO_RELEASER<DESIGN_BLOCK_IO> oldFilePI( DESIGN_BLOCK_IO_MGR::FindPlugin( oldFileType ) );
    IO_RELEASER<DESIGN_BLOCK_IO> kicadPI(
            DESIGN_BLOCK_IO_MGR::FindPlugin( DESIGN_BLOCK_IO_MGR::KICAD_SEXP ) );
    QStringList dbNames;
    QFileInfo    newFileName( aNewFilePath );

    if( !newFileName.suffix().isEmpty() )
    {
        QString extraDir = newFileName.fileName();
        newFileName = QFileInfo( newFileName.absolutePath() );
        newFileName = QFileInfo( newFileName.absoluteFilePath() + "/" + extraDir );
    }

    if( !QDir( newFileName.absolutePath() ).exists() && !QDir().mkpath( aNewFilePath ) )
        return false;

    try
    {
        bool bestEfforts = false; // throw on first error
        oldFilePI->DesignBlockEnumerate( dbNames, aOldFilePath, bestEfforts, aOldFileProps );

        for( const QString& dbName : dbNames )
        {
            std::unique_ptr<const DESIGN_BLOCK> db(
                    oldFilePI->GetEnumeratedDesignBlock( aOldFilePath, dbName, aOldFileProps ) );
            kicadPI->DesignBlockSave( aNewFilePath, db.get() );
        }
    }
    catch( ... )
    {
        return false;
    }

    return true;
}


const DESIGN_BLOCK_IO::IO_FILE_DESC DESIGN_BLOCK_IO::GetLibraryDesc() const
{
    return IO_BASE::IO_FILE_DESC( _HKI( "KiCad Design Block folders" ), {},
                                  { FILEEXT::KiCadDesignBlockLibPathExtension }, false );
}


long long DESIGN_BLOCK_IO::GetLibraryTimestamp( const QString& aLibraryPath ) const
{
    QDir libDir( aLibraryPath );

    if( !libDir.exists() )
        return 0;

    long long ts = 0;

    QStringList entries = libDir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    
    for( const QString& filename : entries )

    {
        QFileInfo blockDir( aLibraryPath + "/" + filename );

        // Check if the directory ends with ".kicad_block", if so hash all the files in it.
        if( blockDir.fileName().endsWith( QString::fromUtf8(FILEEXT::KiCadDesignBlockPathExtension) ) )
            ts += TimestampDir( blockDir.absoluteFilePath(), "*" );
    }

    return ts;
}


void DESIGN_BLOCK_IO::CreateLibrary( const QString&                    aLibraryPath,
                                     const std::map<std::string, UTF8>* aProperties )
{
    if( QDir( aLibraryPath ).exists() )
    {
        THROW_IO_ERROR( QString(_("Cannot overwrite library path '%1'.")).arg( aLibraryPath ) );
    }

    QDir dir;
    
    if( !dir.mkpath( aLibraryPath ) )
    {
        THROW_IO_ERROR(
                QString(_("Library path '%1' could not be created.\n\n"
                                     "Make sure you have write permissions and try again.")).arg( aLibraryPath ) );
    }
}


bool DESIGN_BLOCK_IO::DeleteLibrary( const QString&                    aLibraryPath,
                                     const std::map<std::string, UTF8>* aProperties )
{
    QFileInfo fn( aLibraryPath );

    // Return if there is no library path to delete.
    if( !fn.exists() || !fn.isDir() )
        return false;

    if( !fn.isWritable() )
    {
        THROW_IO_ERROR( QString(_("Insufficient permissions to delete folder '%1'.")).arg( aLibraryPath ) );
    }

    QDir dir( aLibraryPath );

    // Design block folders should only contain sub-folders for each design block
    if( !dir.entryList( QDir::Files ).isEmpty() )
    {
        THROW_IO_ERROR( QString(_("Library folder '%1' has unexpected files.")).arg( aLibraryPath ) );
    }

    // Must delete all sub-directories before deleting the library directory
    QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
    if( !subdirs.isEmpty() )
    {
        QStringList dirs;

        // Get all sub-directories in the library path
        for( const QString& subdir : subdirs )
            dirs.append( dir.absoluteFilePath( subdir ) );

        for( int i = 0; i < dirs.size(); i++ )
        {
            QFileInfo tmp( dirs[i] );

            if( tmp.suffix() != QString::fromUtf8(FILEEXT::KiCadDesignBlockLibPathExtension) )
            {
                THROW_IO_ERROR( QString(_("Unexpected folder '%1' found in library "
                                                     "path '%2'.")).arg( dirs[i] ).arg( aLibraryPath ) );
            }
        }

        for( int i = 0; i < dirs.size(); i++ )
            QDir().rmpath( dirs[i] );
    }

    qDebug() << "Removing design block library '" << aLibraryPath << "'.";

    // Remove directory recursively
    if( !QDir( aLibraryPath ).removeRecursively() )
    {
        THROW_IO_ERROR( QString(_("Design block library '%1' cannot be deleted.")).arg( aLibraryPath ) );
    }

    // For some reason removing a directory in Windows is not immediately updated.  This delay
    // prevents an error when attempting to immediately recreate the same directory when over
    // writing an existing library.
#ifdef _WIN32
    QThread::msleep( 250 );
#endif

    return true;
}


void DESIGN_BLOCK_IO::DesignBlockEnumerate( QStringList&  aDesignBlockNames,
                                            const QString& aLibraryPath, bool aBestEfforts,
                                            const std::map<std::string, UTF8>* aProperties )
{
    // From the starting directory, look for all directories ending in the .kicad_block extension
    QDir dir( aLibraryPath );

    if( !dir.exists() )
        return;

    QString fileSpec = "*." + QString::fromStdString( FILEEXT::KiCadDesignBlockPathExtension );
    QStringList entries = dir.entryList( QStringList() << fileSpec, QDir::Dirs );

    for( const QString& dirname : entries )
    {
        aDesignBlockNames.append( dirname.left( dirname.lastIndexOf( '.' ) ) );
    }
}


DESIGN_BLOCK* DESIGN_BLOCK_IO::DesignBlockLoad( const QString& aLibraryPath,
                                                const QString& aDesignBlockName, bool aKeepUUID,
                                                const std::map<std::string, UTF8>* aProperties )
{
    QString dbPath = aLibraryPath + "/" + aDesignBlockName + "."
                      + QString::fromStdString(FILEEXT::KiCadDesignBlockPathExtension) + "/";
    QString dbSchPath = dbPath + aDesignBlockName + "."
                         + QString::fromStdString(FILEEXT::KiCadSchematicFileExtension);
    QString dbMetadataPath = dbPath + aDesignBlockName + "." + QString::fromStdString(FILEEXT::JsonFileExtension);

    if( !QFile::exists( dbSchPath ) )
        return nullptr;

    DESIGN_BLOCK* newDB = new DESIGN_BLOCK();

    // Library name needs to be empty for when we fill it in with the correct library nickname
    // one layer above
    newDB->SetLibId( LIB_ID( QString(), aDesignBlockName ) );
    newDB->SetSchematicFile( dbSchPath );

    // Parse the JSON file if it exists
    if( QFile::exists( dbMetadataPath ) )
    {
        try
        {
            nlohmann::ordered_json dbMetadata;
            std::ifstream          dbMetadataFile( dbMetadataPath.toStdString() );

            dbMetadataFile >> dbMetadata;

            if( dbMetadata.contains( "description" ) )
                newDB->SetLibDescription( QString::fromStdString(dbMetadata["description"]) );

            if( dbMetadata.contains( "keywords" ) )
                newDB->SetKeywords( QString::fromStdString(dbMetadata["keywords"]) );

            nlohmann::ordered_map<QString, QString> fields;

            // Read the "fields" object from the JSON
            if( dbMetadata.contains( "fields" ) )
            {
                for( auto& item : dbMetadata["fields"].items() )
                {
                    QString name = QString::fromUtf8( item.key().c_str() );
                    QString value = QString::fromUtf8( item.value().get<std::string>().c_str() );

                    fields[name] = value;
                }

                newDB->SetFields( fields );
            }
        }
        catch( ... )
        {
            THROW_IO_ERROR( QString(
                    _("Design block metadata file '%1' could not be read.")).arg( dbMetadataPath ) );
        }
    }


    return newDB;
}


void DESIGN_BLOCK_IO::DesignBlockSave( const QString&                    aLibraryPath,
                                       const DESIGN_BLOCK*                aDesignBlock,
                                       const std::map<std::string, UTF8>* aProperties )
{
    // Make sure we have a valid LIB_ID or we can't save the design block
    if( !aDesignBlock->GetLibId().IsValid() )
    {
        THROW_IO_ERROR( _("Design block does not have a valid library ID.") );
    }

    QFileInfo schematicFile( aDesignBlock->GetSchematicFile() );

    if( !schematicFile.exists() )
    {
        THROW_IO_ERROR( QString(_("Schematic source file '%1' does not exist.")).arg(
                                          schematicFile.absoluteFilePath() ) );
    }

    // Create the design block folder
    QFileInfo dbFolder( aLibraryPath + "/"
                         + aDesignBlock->GetLibId().GetLibItemName() + "."
                         + QString::fromStdString(FILEEXT::KiCadDesignBlockPathExtension)
                         + "/" );

    if( !QDir( dbFolder.absoluteFilePath() ).exists() )
    {
        if( !QDir().mkpath( dbFolder.absoluteFilePath() ) )
        {
            THROW_IO_ERROR( QString(_("Design block folder '%1' could not be created.")).arg(
                                              dbFolder.absoluteFilePath() ) );
        }
    }

    // The new schematic file name is based on the design block name, not the source sheet name
    QString dbSchematicFile = dbFolder.absoluteFilePath() + "/" + aDesignBlock->GetLibId().GetLibItemName()
                               + "." + QString::fromStdString(FILEEXT::KiCadSchematicFileExtension);

    // If the source and destination files are the same, then we don't need to copy the file
    // as we are just updating the metadata
    if( schematicFile.absoluteFilePath() != dbSchematicFile )
    {
        // Copy the source sheet file to the design block folder, under the design block name
        if( !QFile::copy( schematicFile.absoluteFilePath(), dbSchematicFile ) )
        {
            THROW_IO_ERROR( QString(
                    _("Schematic file '%1' could not be saved as design block at '%2'.")).arg(
                    schematicFile.absoluteFilePath() ).arg( dbSchematicFile ) );
        }
    }


    QString dbMetadataFile = dbFolder.absoluteFilePath() + "/" + aDesignBlock->GetLibId().GetLibItemName()
                              + "." + QString::fromStdString(FILEEXT::JsonFileExtension);

    // Write the metadata file
    nlohmann::ordered_json dbMetadata;
    dbMetadata["description"] = aDesignBlock->GetLibDescription().toStdString();
    dbMetadata["keywords"] = aDesignBlock->GetKeywords().toStdString();
    // Convert QString keys and values to std::string for JSON compatibility
    nlohmann::ordered_json fieldsJson;
    const auto& fields = aDesignBlock->GetFields();
    for( auto it = fields.begin(); it != fields.end(); ++it )
    {
        fieldsJson[it->first.toStdString()] = it->second.toStdString();
    }
    dbMetadata["fields"] = fieldsJson;

    bool success = false;

    try
    {
        QFile mdFile( dbMetadataFile );

        if( mdFile.open( QIODevice::WriteOnly ) )
            success = ( mdFile.write( dbMetadata.dump( 0 ).c_str() ) != -1 );

        // QFile dtor will close the file
    }
    catch( ... )
    {
        success = false;
    }

    if( !success )
    {
        THROW_IO_ERROR( QString(
                _("Design block metadata file '%1' could not be saved.")).arg( dbMetadataFile ) );
    }
}


void DESIGN_BLOCK_IO::DesignBlockDelete( const QString& aLibPath, const QString& aDesignBlockName,
                                         const std::map<std::string, UTF8>* aProperties )
{
    QFileInfo dbDir( aLibPath + "/" + aDesignBlockName
                                   + "." + QString::fromStdString(FILEEXT::KiCadDesignBlockPathExtension) );

    if( !dbDir.exists() || !dbDir.isDir() )
    {
        THROW_IO_ERROR(
                QString(_("Design block '%1' does not exist.")).arg( dbDir.fileName() ) );
    }

    // Delete the whole design block folder
    if( !QDir( dbDir.absoluteFilePath() ).removeRecursively() )
    {
        THROW_IO_ERROR( QString(_("Design block folder '%1' could not be deleted.")).arg(
                                          dbDir.absoluteFilePath() ) );
    }
}


bool DESIGN_BLOCK_IO::IsLibraryWritable( const QString& aLibraryPath )
{
    QFileInfo path( aLibraryPath );
    return path.exists() && path.isWritable();
}
