
// QT_TRANSFORMATION_COMPLETED - Verified on 2025-09-05

#include <unordered_set>
#include <pcb_io/pcb_io.h>
#include <pcb_io/pcb_io_mgr.h>
#include <ki_exception.h>
#include <QFileInfo>
#include <QDebug>
#include <QDir>
#include <QString>
#include <QStringList>


#define FMT_UNIMPLEMENTED "Plugin \"%1\" does not implement the \"%2\" function."
#define NOT_IMPLEMENTED( aCaller )                                          \
    THROW_IO_ERROR( QString( FMT_UNIMPLEMENTED ).arg( GetName() ).arg( QString::fromUtf8( aCaller ) ) );


bool PCB_IO::CanReadBoard( const QString& aFileName ) const
{
    const std::vector<std::string>& exts = GetBoardFileDesc().m_FileExtensions;

    QString fileExt = QFileInfo( aFileName ).suffix().toLower();

    for( const std::string& ext : exts )
    {
        if( fileExt == QString::fromStdString( ext ).toLower() )
            return true;
    }

    return false;
}


bool PCB_IO::CanReadFootprint( const QString& aFileName ) const
{
    const std::vector<std::string>& exts = GetLibraryFileDesc().m_FileExtensions;

    QString fileExt = QFileInfo( aFileName ).suffix().toLower();

    for( const std::string& ext : exts )
    {
        if( fileExt == QString::fromStdString( ext ).toLower() )
            return true;
    }

    return false;
}


BOARD* PCB_IO::LoadBoard( const QString& aFileName, BOARD* aAppendToMe,
                          const std::map<std::string, UTF8>* aProperties, PROJECT* aProject )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


std::vector<FOOTPRINT*> PCB_IO::GetImportedCachedLibraryFootprints()
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void PCB_IO::SaveBoard( const QString& aFileName, BOARD* aBoard,
                        const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the PLUGIN interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void PCB_IO::FootprintEnumerate( QStringList& aFootprintNames, const QString& aLibraryPath,
                                 bool aBestEfforts, const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the PLUGIN interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


FOOTPRINT* PCB_IO::ImportFootprint( const QString& aFootprintPath, QString& aFootprintNameOut,
                                    const std::map<std::string, UTF8>* aProperties )
{
    QStringList footprintNames;

    FootprintEnumerate( footprintNames, aFootprintPath, true, aProperties );

    if( footprintNames.isEmpty() )
        return nullptr;

    if( footprintNames.size() > 1 )
    {
        qDebug() << "Selected file contains multiple footprints. Only the first one will be "
                    "imported.\nTo load all footprints, add it as a library using Preferences "
                    "-> Manage Footprint Libraries...";
    }

    aFootprintNameOut = footprintNames.first();

    return FootprintLoad( aFootprintPath, aFootprintNameOut, false, aProperties );
}


const FOOTPRINT* PCB_IO::GetEnumeratedFootprint( const QString& aLibraryPath,
                                                 const QString& aFootprintName,
                                                 const std::map<std::string, UTF8>* aProperties )
{
    // default implementation
    return FootprintLoad( aLibraryPath, aFootprintName, false, aProperties );
}


bool PCB_IO::FootprintExists( const QString& aLibraryPath, const QString& aFootprintName,
                              const std::map<std::string, UTF8>* aProperties )
{
    // default implementation
    return FootprintLoad( aLibraryPath, aFootprintName, true, aProperties ) != nullptr;
}


FOOTPRINT* PCB_IO::FootprintLoad( const QString& aLibraryPath, const QString& aFootprintName,
                                  bool  aKeepUUID, const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the PLUGIN interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void PCB_IO::FootprintSave( const QString& aLibraryPath, const FOOTPRINT* aFootprint,
                            const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the PLUGIN interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void PCB_IO::FootprintDelete( const QString& aLibraryPath, const QString& aFootprintName,
                              const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the PLUGIN interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void PCB_IO::GetLibraryOptions( std::map<std::string, UTF8>* aListToAppendTo ) const
{
    // Get base options first
    IO_BASE::GetLibraryOptions( aListToAppendTo );

    // disable all these in another couple of months, after everyone has seen them:
#if 1
    (*aListToAppendTo)["debug_level"] = UTF8( "Enable <b>debug</b> logging for Footprint*() "
                                                 "functions in this PCB_IO." );

    (*aListToAppendTo)["read_filter_regex"] = UTF8( "Regular expression <b>footprint name</b> "
                                                       "filter." );

    (*aListToAppendTo)["enable_transaction_logging"] = UTF8( "Enable transaction logging. The "
                                                                "mere presence of this option "
                                                                "turns on the logging, no need to "
                                                                "set a Value." );

    (*aListToAppendTo)["username"] = UTF8( "User name for <b>login</b> to some special library "
                                              "server." );

    (*aListToAppendTo)["password"] = UTF8( "Password for <b>login</b> to some special library "
                                              "server." );
#endif

#if 1
    // Suitable for a C++ to python PCB_IO::Footprint*() adapter, move it to the adapter
    // if and when implemented.
    (*aListToAppendTo)["python_footprint_plugin"] = UTF8( "Enter the python module which "
                                                             "implements the PCB_IO::Footprint*() "
                                                             "functions." );
#endif
}
