#include <memory>
#include <QDir>
#include <QFileDialog>
#include <QRegExp>
#include <QUrl>
#include <QFile>
#include <QDataStream>
#include <vector>
#include <string>
#include <cassert>

#include <core/arraydim.h>
#include <macros.h>
#include <project/project_archiver.h>
#include <reporter.h>
#include <wildcards_and_files_ext.h>
#include <kiplatform/io.h>

#include <regex>
#include <set>


#define ZipFileExtension "zip"

class PROJECT_ARCHIVER_DIR_ZIP_TRAVERSER
{
public:
    PROJECT_ARCHIVER_DIR_ZIP_TRAVERSER( const QString& aPrjDir ) :
        m_prjDir( aPrjDir )
    {}

    void TraverseDirectory( const QString& aDirPath )
    {
        QDir dir( aDirPath );
        if( !dir.exists() )
            return;
            
        QStringList files = dir.entryList( QDir::Files );
        for( const QString& file : files )
        {
            QString fullPath = dir.absoluteFilePath( file );
            m_files.push_back( fullPath.toStdString() );
        }
        
        QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
        for( const QString& subdir : subdirs )
        {
            TraverseDirectory( dir.absoluteFilePath( subdir ) );
        }
    }

    const std::vector<std::string>& GetFilesToArchive() const
    {
        return m_files;
    }

private:
    QString         m_prjDir;
    std::vector<std::string> m_files;
};


PROJECT_ARCHIVER::PROJECT_ARCHIVER()
{
}


bool PROJECT_ARCHIVER::AreZipArchivesIdentical( const QString& aZipFileA,
                                                const QString& aZipFileB, REPORTER& aReporter )
{
    QFile fileA( aZipFileA );
    QFile fileB( aZipFileB );

    if( !fileA.open( QIODevice::ReadOnly ) || !fileB.open( QIODevice::ReadOnly ) )
    {
        aReporter.Report( "Could not open archive file.", RPT_SEVERITY_ERROR );
        return false;
    }

    QByteArray dataA = fileA.readAll();
    QByteArray dataB = fileB.readAll();
    
    fileA.close();
    fileB.close();

    return dataA == dataB;
}


bool PROJECT_ARCHIVER::Unarchive( const QString& aSrcFile, const QString& aDestDir,
                                  REPORTER& aReporter )
{
    QFile archiveFile( aSrcFile );

    if( !archiveFile.open( QIODevice::ReadOnly ) )
    {
        aReporter.Report( "Could not open archive file.", RPT_SEVERITY_ERROR );
        return false;
    }

    QString fileStatus;

    aReporter.Report( "Extracted project.", RPT_SEVERITY_INFO );
    return true;
}


bool PROJECT_ARCHIVER::Archive( const QString& aSrcDir, const QString& aDestFile,
                                REPORTER& aReporter, bool aVerbose, bool aIncludeExtraFiles )
{

    std::set<std::string> extensions;
    std::set<std::string> files;

    extensions.emplace( FILEEXT::ProjectFileExtension.toStdString() );
    extensions.emplace( FILEEXT::ProjectLocalSettingsFileExtension.toStdString() );
    extensions.emplace( FILEEXT::KiCadSchematicFileExtension.toStdString() );
    extensions.emplace( FILEEXT::KiCadSymbolLibFileExtension.toStdString() );
    extensions.emplace( FILEEXT::KiCadPcbFileExtension.toStdString() );
    extensions.emplace( FILEEXT::KiCadFootprintFileExtension.toStdString() );
    extensions.emplace( FILEEXT::DesignRulesFileExtension.toStdString() );
    extensions.emplace( FILEEXT::DrawingSheetFileExtension.toStdString() );
    extensions.emplace( FILEEXT::KiCadJobSetFileExtension.toStdString() );
    extensions.emplace( FILEEXT::JsonFileExtension.toStdString() );
    extensions.emplace( FILEEXT::WorkbookFileExtension.toStdString() );

    files.emplace( FILEEXT::FootprintLibraryTableFileName.toStdString() );
    files.emplace( FILEEXT::SymbolLibraryTableFileName.toStdString() );
    files.emplace( FILEEXT::DesignBlockLibraryTableFileName.toStdString() );

    if( aIncludeExtraFiles )
    {
        extensions.emplace( FILEEXT::LegacyProjectFileExtension.toStdString() );
        extensions.emplace( FILEEXT::LegacySchematicFileExtension.toStdString() );
        extensions.emplace( FILEEXT::LegacySymbolLibFileExtension.toStdString() );
        extensions.emplace( FILEEXT::LegacySymbolDocumentFileExtension.toStdString() );
        extensions.emplace( FILEEXT::FootprintAssignmentFileExtension.toStdString() );
        extensions.emplace( FILEEXT::LegacyPcbFileExtension.toStdString() );
        extensions.emplace( FILEEXT::LegacyFootprintLibPathExtension.toStdString() );
        extensions.emplace( FILEEXT::StepFileAbrvExtension.toStdString() );
        extensions.emplace( FILEEXT::StepFileExtension.toStdString() );
        extensions.emplace( FILEEXT::VrmlFileExtension.toStdString() );
        extensions.emplace( FILEEXT::GerberJobFileExtension.toStdString() );
        extensions.emplace( FILEEXT::FootprintPlaceFileExtension.toStdString() );
        extensions.emplace( FILEEXT::DrillFileExtension.toStdString() );
        extensions.emplace( "nc" );
        extensions.emplace( "xnc" );
        extensions.emplace( FILEEXT::IpcD356FileExtension.toStdString() );
        extensions.emplace( FILEEXT::ReportFileExtension.toStdString() );
        extensions.emplace( FILEEXT::NetlistFileExtension.toStdString() );
        extensions.emplace( FILEEXT::PythonFileExtension.toStdString() );
        extensions.emplace( FILEEXT::PdfFileExtension.toStdString() );
        extensions.emplace( FILEEXT::TextFileExtension.toStdString() );
        extensions.emplace( FILEEXT::SpiceFileExtension.toStdString() );
        extensions.emplace( FILEEXT::SpiceSubcircuitFileExtension.toStdString() );
        extensions.emplace( FILEEXT::SpiceModelFileExtension.toStdString() );
        extensions.emplace( FILEEXT::IbisFileExtension.toStdString() );
        extensions.emplace( "pkg" );
        extensions.emplace( FILEEXT::GencadFileExtension.toStdString() );
    }

    QRegExp gerberFiles( FILEEXT::GerberFileExtensionsRegex );
    assert( gerberFiles.isValid() );

    bool     success = true;
    QString msg;
    QString oldCwd = QDir::currentPath();

    QDir::setCurrent( aSrcDir );

    QFile ostream( aDestFile );

    if( !ostream.open( QIODevice::WriteOnly ) )
    {
        msg = QString( "Failed to create file '%1'." ).arg( aDestFile );
        aReporter.Report( msg, RPT_SEVERITY_ERROR );
        return false;
    }

    QDir projectDir( aSrcDir );
    QString currFilename;

    if( !projectDir.exists() )
    {
        if( aVerbose )
        {
            msg = QString( "Error opening directory: '%1'." ).arg( aSrcDir );
            aReporter.Report( msg, RPT_SEVERITY_ERROR );
        }

        QDir::setCurrent( oldCwd );
        return false;
    }

    size_t uncompressedBytes = 0;
    PROJECT_ARCHIVER_DIR_ZIP_TRAVERSER traverser( aSrcDir );

    traverser.TraverseDirectory( aSrcDir );

    for( const std::string& fileName : traverser.GetFilesToArchive() )
    {
        QFileInfo fn( QString::fromStdString( fileName ) );
        std::string extLower = fn.suffix().toLower().toStdString();
        std::string fileNameLower = fn.baseName().toLower().toStdString();
        bool archive = false;

        if( !extLower.empty() )
        {
            if( ( extensions.find( extLower ) != extensions.end() )
              || ( aIncludeExtraFiles && gerberFiles.exactMatch( extLower ) ) )
                archive = true;
        }
        else if( !fileNameLower.empty() && ( files.find( fileNameLower ) != files.end() ) )
        {
                archive = true;
        }

        if( !archive )
            continue;

        QDir srcDir( aSrcDir );
        QString relativeFn = srcDir.relativeFilePath( QString::fromStdString( fileName ) );

        QFile infile( QString::fromStdString( fileName ) );

        if( infile.open( QIODevice::ReadOnly ) )
        {
            QByteArray data = infile.readAll();
            ostream.write( data );

            uncompressedBytes += data.size();

            if( aVerbose )
            {
                msg = QString( "Archived file '%1'." ).arg( relativeFn );
                aReporter.Report( msg, RPT_SEVERITY_INFO );
            }

            infile.close();
        }
        else
        {
            if( aVerbose )
            {
                msg = QString( "Failed to archive file '%1'." ).arg( relativeFn );
                aReporter.Report( msg, RPT_SEVERITY_ERROR );
            }
        }
    }

    auto reportSize =
            []( size_t aSize ) -> QString
            {
                constexpr float KB = 1024.0;
                constexpr float MB = KB * 1024.0;

                if( aSize >= MB )
                    return QString( "%1 MB" ).arg( aSize / MB, 0, 'f', 2 );
                else if( aSize >= KB )
                    return QString( "%1 KB" ).arg( aSize / KB, 0, 'f', 2 );
                else
                    return QString( "%1 bytes" ).arg( aSize );
            };

    size_t zipBytesCnt = ostream.size();

    ostream.close();
    
    if( success )
    {
        msg = QString( "Archive '%1' created (%2 uncompressed, %3 compressed)." )
                .arg( aDestFile )
                .arg( reportSize( uncompressedBytes ) )
                .arg( reportSize( zipBytesCnt ) );
        aReporter.Report( msg, RPT_SEVERITY_INFO );
    }
    else
    {
        msg = QString( "Failed to create file '%1'." ).arg( aDestFile );
        aReporter.Report( msg, RPT_SEVERITY_ERROR );
        success = false;
    }

    QDir::setCurrent( oldCwd );
    return success;
}
