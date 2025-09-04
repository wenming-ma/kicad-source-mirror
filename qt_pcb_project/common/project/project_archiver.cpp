#include <memory>
#include <QDir>
#include <QFileDialog>
#include <QRegExp>
#include <QUrl>
#include <QFile>
#include <QDataStream>

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
            m_files.emplace_back( fullPath );
        }
        
        QStringList subdirs = dir.entryList( QDir::Dirs | QDir::NoDotAndDotDot );
        for( const QString& subdir : subdirs )
        {
            TraverseDirectory( dir.absoluteFilePath( subdir ) );
        }
    }

    const QVector<QString>& GetFilesToArchive() const
    {
        return m_files;
    }

private:
    QString         m_prjDir;
    QVector<QString> m_files;
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

    std::set<QString> extensions;
    std::set<QString> files;

    extensions.emplace( FILEEXT::ProjectFileExtension );
    extensions.emplace( FILEEXT::ProjectLocalSettingsFileExtension );
    extensions.emplace( FILEEXT::KiCadSchematicFileExtension );
    extensions.emplace( FILEEXT::KiCadSymbolLibFileExtension );
    extensions.emplace( FILEEXT::KiCadPcbFileExtension );
    extensions.emplace( FILEEXT::KiCadFootprintFileExtension );
    extensions.emplace( FILEEXT::DesignRulesFileExtension );
    extensions.emplace( FILEEXT::DrawingSheetFileExtension );
    extensions.emplace( FILEEXT::KiCadJobSetFileExtension );
    extensions.emplace( FILEEXT::JsonFileExtension );
    extensions.emplace( FILEEXT::WorkbookFileExtension );

    files.emplace( FILEEXT::FootprintLibraryTableFileName );
    files.emplace( FILEEXT::SymbolLibraryTableFileName );
    files.emplace( FILEEXT::DesignBlockLibraryTableFileName );

    if( aIncludeExtraFiles )
    {
        extensions.emplace( FILEEXT::LegacyProjectFileExtension );
        extensions.emplace( FILEEXT::LegacySchematicFileExtension );
        extensions.emplace( FILEEXT::LegacySymbolLibFileExtension );
        extensions.emplace( FILEEXT::LegacySymbolDocumentFileExtension );
        extensions.emplace( FILEEXT::FootprintAssignmentFileExtension );
        extensions.emplace( FILEEXT::LegacyPcbFileExtension );
        extensions.emplace( FILEEXT::LegacyFootprintLibPathExtension );
        extensions.emplace( FILEEXT::StepFileAbrvExtension );
        extensions.emplace( FILEEXT::StepFileExtension );
        extensions.emplace( FILEEXT::VrmlFileExtension );
        extensions.emplace( FILEEXT::GerberJobFileExtension );
        extensions.emplace( FILEEXT::FootprintPlaceFileExtension );
        extensions.emplace( FILEEXT::DrillFileExtension );
        extensions.emplace( "nc" );
        extensions.emplace( "xnc" );
        extensions.emplace( FILEEXT::IpcD356FileExtension );
        extensions.emplace( FILEEXT::ReportFileExtension );
        extensions.emplace( FILEEXT::NetlistFileExtension );
        extensions.emplace( FILEEXT::PythonFileExtension );
        extensions.emplace( FILEEXT::PdfFileExtension );
        extensions.emplace( FILEEXT::TextFileExtension );
        extensions.emplace( FILEEXT::SpiceFileExtension );
        extensions.emplace( FILEEXT::SpiceSubcircuitFileExtension );
        extensions.emplace( FILEEXT::SpiceModelFileExtension );
        extensions.emplace( FILEEXT::IbisFileExtension );
        extensions.emplace( "pkg" );
        extensions.emplace( FILEEXT::GencadFileExtension );
    }

    QRegExp gerberFiles( FILEEXT::GerberFileExtensionsRegex );
    Q_ASSERT( gerberFiles.isValid() );

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

    for( const QString& fileName : traverser.GetFilesToArchive() )
    {
        QFileInfo fn( fileName );
        QString extLower = fn.suffix().toLower();
        QString fileNameLower = fn.baseName().toLower();
        bool archive = false;

        if( !extLower.isEmpty() )
        {
            if( ( extensions.find( extLower ) != extensions.end() )
              || ( aIncludeExtraFiles && gerberFiles.exactMatch( extLower ) ) )
                archive = true;
        }
        else if( !fileNameLower.isEmpty() && ( files.find( fileNameLower ) != files.end() ) )
        {
                archive = true;
        }

        if( !archive )
            continue;

        QDir srcDir( aSrcDir );
        QString relativeFn = srcDir.relativeFilePath( fileName );

        QFile infile( fileName );

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
