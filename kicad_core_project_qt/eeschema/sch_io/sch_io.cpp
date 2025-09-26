
#include <unordered_set>

#include <ki_exception.h>
#include <sch_io/sch_io.h>
#include <sch_io/sch_io_mgr.h>
#include <QFileInfo>
#include <QDir>
#include <QString>

#define FMT_UNIMPLEMENTED "Plugin \"%s\" does not implement the \"%s\" function."
#define NOT_IMPLEMENTED( aCaller )                                                   \
    THROW_IO_ERROR( QString::asprintf( FMT_UNIMPLEMENTED,                           \
                                      GetName().toStdString().c_str(),                           \
                                      QString::fromUtf8( aCaller ).toStdString().c_str() ) );


const IO_BASE::IO_FILE_DESC SCH_IO::GetSchematicFileDesc() const
{
    return IO_BASE::IO_FILE_DESC( QString(), {} );
}


bool SCH_IO::CanReadSchematicFile( const QString& aFileName ) const
{
    const std::vector<std::string>& exts = GetSchematicFileDesc().m_FileExtensions;

    QString fileExt = QFileInfo( aFileName ).suffix().toLower();

    for( const std::string& ext : exts )
    {
        if( fileExt == QString::fromStdString( ext ).toLower() )
            return true;
    }

    return false;
}


void SCH_IO::SaveLibrary( const QString& aFileName, const std::map<std::string, UTF8>* aProperties )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


SCH_SHEET* SCH_IO::LoadSchematicFile( const QString& aFileName, SCHEMATIC* aSchematic,
                                      SCH_SHEET* aAppendToMe, const std::map<std::string, UTF8>* aProperties )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::SaveSchematicFile( const QString& aFileName, SCH_SHEET* aSheet, SCHEMATIC* aSchematic,
                                const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::EnumerateSymbolLib( QStringList&    aAliasNameList,
                                 const QString&   aLibraryPath,
                                 const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::EnumerateSymbolLib( std::vector<LIB_SYMBOL*>& aSymbolList,
                                 const QString&   aLibraryPath,
                                 const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


LIB_SYMBOL* SCH_IO::LoadSymbol( const QString& aLibraryPath, const QString& aSymbolName,
                                const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::SaveSymbol( const QString& aLibraryPath, const LIB_SYMBOL* aSymbol,
                         const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::DeleteSymbol( const QString& aLibraryPath, const QString& aSymbolName,
                           const std::map<std::string, UTF8>* aProperties )
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}


void SCH_IO::GetLibraryOptions( std::map<std::string, UTF8>* aListToAppendTo ) const
{
    // Get base options first
    IO_BASE::GetLibraryOptions( aListToAppendTo );

    // Empty for most plugins
    //
    // To add a new option override and use example code below:
    //
    //(*aListToAppendTo)["new_option_name"] = UTF8( _(
    //    "A nice descrtiption with possibility for <b>bold</b> and other formatting."
    //    ) );
}


const QString& SCH_IO::GetError() const
{
    // not pure virtual so that plugins only have to implement subset of the SCH_IO interface.
    NOT_IMPLEMENTED( __FUNCTION__ );
}
