#include <unordered_set>

#include <io/io_base.h>
#include <progress_reporter.h>
#include <ki_exception.h>
#include <reporter.h>
#include <wildcards_and_files_ext.h>

#include <QString>
#include <QFileInfo>
#include <QDir>
#include <QCoreApplication>

#define FMT_UNIMPLEMENTED "IO interface \"%1\" does not implement the \"%2\" function."
#define NOT_IMPLEMENTED( aCaller )                                       \
    THROW_IO_ERROR( QString( FMT_UNIMPLEMENTED ).arg( GetName() ).arg( QString::fromUtf8( aCaller ) ) );


QString IO_BASE::IO_FILE_DESC::FileFilter() const
{
    return QCoreApplication::translate( "IO_BASE", m_Description.toUtf8().data() ) + AddFileExtListToFilter( m_FileExtensions );
}


void IO_BASE::CreateLibrary( const QString& aLibraryPath,
                             const std::map<std::string, UTF8>* aProperties )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


bool IO_BASE::DeleteLibrary( const QString& aLibraryPath,
                             const std::map<std::string, UTF8>* aProperties )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}


bool IO_BASE::IsLibraryWritable( const QString& aLibraryPath )
{
    NOT_IMPLEMENTED( __FUNCTION__ );
}

void IO_BASE::GetLibraryOptions( std::map<std::string, UTF8>* aListToAppendTo ) const
{
    // No global options to append
}


bool IO_BASE::CanReadLibrary( const QString& aFileName ) const
{
    const IO_BASE::IO_FILE_DESC& desc = GetLibraryDesc();

    if( desc.m_IsFile )
    {
        const std::vector<std::string>& exts = desc.m_FileExtensions;

        QString fileExt = QFileInfo( aFileName ).suffix().toLower();

        for( const std::string& ext : exts )
        {
            if( fileExt == QString::fromStdString( ext ).toLower() )
                return true;
        }
    }
    else
    {
        QDir dir( aFileName );

        if( !dir.exists() )
            return false;

        std::vector<std::string>     exts = desc.m_ExtensionsInDir;
        std::unordered_set<QString> lowerExts;

        for( const std::string& ext : exts )
            lowerExts.emplace( QString::fromStdString( ext ).toLower() );

        QStringList files = dir.entryList( QDir::Files | QDir::Hidden );

        for( const QString& filename : files )
        {
            QString ext = "";

            int idx = filename.lastIndexOf( '.' );

            if( idx != -1 )
                ext = filename.mid( idx + 1 ).toLower();

            if( lowerExts.count( ext ) )
                return true;
        }
    }

    return false;
}


void IO_BASE::Report( const QString& aText, SEVERITY aSeverity )
{
    if( !m_reporter )
        return;

    m_reporter->Report( aText, aSeverity );
}


void IO_BASE::AdvanceProgressPhase()
{
    if( !m_progressReporter )
        return;

    if( !m_progressReporter->KeepRefreshing() )
        THROW_IO_ERROR( QCoreApplication::translate( "IO_BASE", "Loading file canceled by user." ) );

    m_progressReporter->AdvancePhase();
}
