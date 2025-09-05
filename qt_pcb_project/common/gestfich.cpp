#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QStandardPaths>
#include <QFileSystemModel>
#include <QRegularExpression>

#include <pgm_base.h>
#include <confirm.h>
#include <core/arraydim.h>
#include <gestfich.h>
#include <string_utils.h>
#include <launch_ext.h>

#include <filesystem>
#include <vector>
#include <i18n_utility.h>
void QuoteString( QString& string )
{
    if( !string.startsWith( "\"" ) )
    {
        string.prepend( "\"" );
        string.append( "\"" );
    }
}


QString FindKicadFile( const QString& shortname )
{
    // Test the presence of the file in the directory shortname of
    // the KiCad binary path.
#ifndef __APPLE__
    QString fullFileName = Pgm().GetExecutablePath() + shortname;
#else
    QString fullFileName = Pgm().GetExecutablePath() + "Contents/MacOS/" + shortname;
#endif
    if( QFileInfo::exists( fullFileName ) )
        return fullFileName;

    if( qEnvironmentVariableIsSet( "KICAD_RUN_FROM_BUILD_DIR" ) )
    {
        QFileInfo buildDir( Pgm().GetExecutablePath() + "/" + shortname );
        QString buildPath = buildDir.absolutePath();
        buildPath = QFileInfo( buildPath ).absolutePath();
#ifndef _WIN32
        buildPath += "/" + shortname;
#else
        int lastDot = shortname.lastIndexOf( '.' );
        if( lastDot > 0 )
            buildPath += "/" + shortname.left( lastDot );
        else
            buildPath += "/" + shortname;
#endif

        if( buildPath.endsWith( "pl_editor" ) )
        {
            buildPath = QFileInfo( buildPath ).absolutePath();
            buildPath += "/pagelayout_editor";
        }

        if( QFileInfo::exists( buildPath ) )
            return buildPath;
    }

    // Test the presence of the file in the directory shortname
    // defined by the environment variable KiCad.
    if( Pgm().IsKicadEnvVariableDefined() )
    {
        fullFileName = Pgm().GetKicadEnvVariable() + shortname;

        if( QFileInfo::exists( fullFileName ) )
            return fullFileName;
    }

#if defined( __WINDOWS__ )
    // KiCad can be installed highly portably on Windows, anywhere and concurrently
    // either the "kicad file" is immediately adjacent to the exe or it's not a valid install
    return shortname;
#else

    // Path list for KiCad binary files
    const static QString possibilities[] = {
#if defined( __APPLE__ )
        "Contents/Applications/pcbnew.app/Contents/MacOS/",
        "Contents/Applications/eeschema.app/Contents/MacOS/",
        "Contents/Applications/gerbview.app/Contents/MacOS/",
        "Contents/Applications/bitmap2component.app/Contents/MacOS/",
        "Contents/Applications/pcb_calculator.app/Contents/MacOS/",
        "Contents/Applications/pl_editor.app/Contents/MacOS/",
#else
        "/usr/bin/",
        "/usr/local/bin/",
        "/usr/local/kicad/bin/",
#endif
    };

    // find binary file from possibilities list:
    for( unsigned i=0;  i<arrayDim(possibilities);  ++i )
    {
#ifndef __APPLE__
        fullFileName = possibilities[i] + shortname;
#else
        fullFileName = Pgm().GetExecutablePath() + possibilities[i] + shortname;
#endif

        if( QFileInfo::exists( fullFileName ) )
            return fullFileName;
    }

    return shortname;

#endif
}


int ExecuteFile( const QString& aEditorName, const QString& aFileName, QProcess** aCallback,
                 bool aFileForKicad )
{
    QString        fullEditorName;
    std::vector<std::string> params;

#ifdef __UNIX__
    QString param;
    bool     inSingleQuotes = false;
    bool     inDoubleQuotes = false;

    auto pushParam =
            [&]()
            {
                if( !param.isEmpty() )
                {
                    params.push_back( param.toStdString() );
                    param.clear();
                }
            };

    for( QChar ch : aEditorName )
    {
        if( inSingleQuotes )
        {
            if( ch == '\'' )
            {
                pushParam();
                inSingleQuotes = false;
                continue;
            }
            else
            {
                param += ch;
            }
        }
        else if( inDoubleQuotes )
        {
            if( ch == '"' )
            {
                pushParam();
                inDoubleQuotes = false;
            }
            else
            {
                param += ch;
            }
        }
        else if( ch == '\'' )
        {
            pushParam();
            inSingleQuotes = true;
        }
        else if( ch == '"' )
        {
            pushParam();
            inDoubleQuotes = true;
        }
        else if( ch == ' ' )
        {
            pushParam();
        }
        else
        {
            param += ch;
        }
    }

    pushParam();

    if( aFileForKicad )
        fullEditorName = FindKicadFile( QString::fromStdString( params[0] ) );
    else
        fullEditorName = QString::fromStdString( params[0] );

    params.erase( params.begin() );
#else

    if( aFileForKicad )
        fullEditorName = FindKicadFile( aEditorName );
    else
        fullEditorName = aEditorName;
#endif

    if( QFileInfo::exists( fullEditorName ) )
    {
        QStringList arguments;

        if( !params.empty() )
        {
            for( const std::string& p : params )
                arguments << QString::fromStdString( p );
        }

        if( !aFileName.isEmpty() )
            arguments << aFileName;

        QProcess* process = new QProcess();
        if( aCallback )
            *aCallback = process;
        
        process->startDetached( fullEditorName, arguments );
        return 0;
    }

    QString msg;
    msg = QString( _( "Command '%1' could not be found." ) ).arg( fullEditorName );
    DisplayErrorMessage( nullptr, msg );
    return -1;
}


bool OpenPDF( const QString& file )
{
    QString msg;
    QString filename = file;

    Pgm().ReadPdfBrowserInfos();

    if( Pgm().UseSystemPdfBrowser() )
    {
        if( !LaunchExternal( filename ) )
        {
            msg = QString( _( "Unable to find a PDF viewer for '%1'." ) ).arg( filename );
            DisplayErrorMessage( nullptr, msg );
            return false;
        }
    }
    else
    {
        QStringList args;
        args << filename;
        
        QProcess process;
        if( process.startDetached( Pgm().GetPdfBrowserName(), args ) == false )
        {
            msg = QString( _( "Problem while running the PDF viewer '%1'." ) ).arg( Pgm().GetPdfBrowserName() );
            DisplayErrorMessage( nullptr, msg );
            return false;
        }
    }

    return true;
}


void KiCopyFile( const QString& aSrcPath, const QString& aDestPath, QString& aErrors )
{
    if( !QFile::copy( aSrcPath, aDestPath ) )
    {
        QString msg;

        if( !aErrors.isEmpty() )
            aErrors += "\n";

        msg = QString( _( "Cannot copy file '%1'." ) ).arg( aDestPath );
        aErrors += msg;
    }
}


QString QuoteFullPath( QFileInfo& fn )
{
    return "\"" + fn.absoluteFilePath() + "\"";
}


bool RmDirRecursive( const QString& aFileName, QString* aErrors )
{
    namespace fs = std::filesystem;

    std::string rmDir = aFileName.toStdString();

    if( rmDir.length() < 3 )
    {
        if( aErrors )
            *aErrors = _( "Invalid directory name, cannot remove root" );

        return false;
    }

    if( !fs::exists( rmDir ) )
    {
        if( aErrors )
            *aErrors = QString( _( "Directory '%1' does not exist" ) ).arg( aFileName );

        return false;
    }

    fs::path path( rmDir );

    if( !fs::is_directory( path ) )
    {
        if( aErrors )
            *aErrors = QString( _( "'%1' is not a directory" ) ).arg( aFileName );

        return false;
    }

    try
    {
        fs::remove_all( path );
    }
    catch( const fs::filesystem_error& e )
    {
        if( aErrors )
            *aErrors = QString( _( "Error removing directory '%1': %2" ) )
                                         .arg( aFileName ).arg( e.what() );

        return false;
    }

    return true;
}


bool CopyDirectory( const QString& aSourceDir, const QString& aDestDir, QString& aErrors )
{
    QDir dir( aSourceDir );

    if( !dir.exists() )
    {
        aErrors += QString( _( "Could not open source directory: %1" ) ).arg( aSourceDir );
        aErrors += "\n";
        return false;
    }

    if( !QDir().mkpath( aDestDir ) )
    {
        aErrors += QString( _( "Could not create destination directory: %1" ) ).arg( aDestDir );
        aErrors += "\n";
        return false;
    }

    QFileInfoList entries = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot );
    
    for( const QFileInfo& entry : entries )
    {
        QString sourcePath = entry.absoluteFilePath();
        QString destPath = aDestDir + QDir::separator() + entry.fileName();

        if( entry.isDir() )
        {
            // Recursively copy subdirectories
            if( !CopyDirectory( sourcePath, destPath, aErrors ) )
                return false;
        }
        else
        {
            // Copy files
            if( !QFile::copy( sourcePath, destPath ) )
            {
                aErrors += QString( _( "Could not copy file: %1 to %2" ) )
                                             .arg( sourcePath )
                                             .arg( destPath );
                return false;
            }
        }
    }

    return true;
}


bool CopyFilesOrDirectory( const QString& aSourcePath, const QString& aDestDir, QString& aErrors,
                           int& aFileCopiedCount, const std::vector<std::string>& aExclusions )
{
    // Parse source path and determine if it's a directory
    QFileInfo sourceFn( aSourcePath );
    QString   sourcePath = sourceFn.absoluteFilePath();
    bool       isSourceDirectory = sourceFn.isDir();
    QString   baseDestDir = aDestDir;

    auto performCopy = [&]( const QString& src, const QString& dest ) -> bool
    {
        if( QFile::copy( src, dest ) )
        {
            aFileCopiedCount++;
            return true;
        }

        aErrors += QString( _( "Could not copy file: %1 to %2" ) ).arg( src ).arg( dest );
        aErrors += "\n";
        return false;
    };

    auto processEntries = [&]( const QString& srcDir, const QString& pattern,
                               const QString& destDir ) -> bool
    {
        QDir dir( srcDir );

        if( !dir.exists() )
        {
            aErrors += QString( _( "Could not open source directory: %1" ) ).arg( srcDir );
            aErrors += "\n";
            return false;
        }

        bool success = true;

        QStringList nameFilters;
        if( !pattern.isEmpty() )
            nameFilters << pattern;
        
        QFileInfoList entries = dir.entryInfoList( nameFilters, QDir::AllEntries | QDir::Hidden | QDir::NoDotAndDotDot );

        for( const QFileInfo& entry : entries )
        {
            const QString entrySrc = entry.absoluteFilePath();
            const QString entryDest = destDir + QDir::separator() + entry.fileName();
            const QString filename = entry.fileName();

            // Apply exclusion filters
            bool exclude = false;
            QRegularExpression lockPattern1( QRegularExpression::wildcardToRegularExpression( "~*.lck" ) );
            QRegularExpression lockPattern2( QRegularExpression::wildcardToRegularExpression( "*.lck" ) );
            
            if( lockPattern1.match( filename ).hasMatch() || lockPattern2.match( filename ).hasMatch() )
                exclude = true;

            for( const auto& exclusion : aExclusions )
            {
                QRegularExpression exclusionPattern( QRegularExpression::wildcardToRegularExpression( QString::fromStdString( exclusion ) ) );
                if( exclusionPattern.match( entrySrc ).hasMatch() )
                {
                    exclude = true;
                    break;
                }
            }

            if( !exclude )
            {
                if( entry.isDir() )
                {
                    // Recursively process subdirectories
                    if( !CopyFilesOrDirectory( entrySrc, destDir, aErrors, aFileCopiedCount,
                                               aExclusions ) )
                    {
                        aErrors += QString( _( "Could not copy directory: %1 to %2" ) )
                                                     .arg( entrySrc ).arg( entryDest );
                        aErrors += "\n";

                        success = false;
                    }
                }
                else
                {
                    // Copy individual files
                    if( !performCopy( entrySrc, entryDest ) )
                    {
                        success = false;
                    }
                }
            }
        }

        return success;
    };

    // If copying a directory, append its name to destination path
    if( isSourceDirectory )
    {
        QString sourceDirName = sourceFn.fileName();
        baseDestDir = QDir( aDestDir ).filePath( sourceDirName );
    }

    // Create destination directory hierarchy
    if( !QDir().mkpath( baseDestDir ) )
    {
        aErrors += QString( _( "Could not create destination directory: %1" ) ).arg( baseDestDir );
        aErrors += "\n";

        return false;
    }

    // Execute appropriate copy operation based on source type
    if( !isSourceDirectory )
    {
        const QString fileName = sourceFn.fileName();

        // Handle wildcard patterns in filenames
        if( fileName.contains( '*' ) || fileName.contains( '?' ) )
        {
            const QString dirPath = sourceFn.absolutePath();

            if( !QDir( dirPath ).exists() )
            {
                aErrors += QString( _( "Source directory does not exist: %1" ) ).arg( dirPath );
                aErrors += "\n";

                return false;
            }
            // Process all matching files in source directory
            return processEntries( dirPath, fileName, baseDestDir );
        }

        // Single file copy operation
        return performCopy( sourcePath, QDir( baseDestDir ).filePath( fileName ) );
    }

    // Full directory copy operation
    return processEntries( sourcePath, QString(), baseDestDir );
}


bool AddDirectoryToZip( QIODevice& aZip, const QString& aSourceDir, QString& aErrors,
                        const QString& aParentDir )
{
    QDir dir( aSourceDir );

    if( !dir.exists() )
    {
        aErrors += QString( _( "Could not open source directory: %1" ) ).arg( aSourceDir );
        aErrors += "\n";
        return false;
    }

    QFileInfoList entries = dir.entryInfoList( QDir::AllEntries | QDir::NoDotAndDotDot );
    
    for( const QFileInfo& entry : entries )
    {
        QString sourcePath = entry.absoluteFilePath();
        QString zipPath = aParentDir + entry.fileName();

        if( entry.isDir() )
        {
            // Recursively add subdirectories
            if( !AddDirectoryToZip( aZip, sourcePath, aErrors, zipPath + "/" ) )
                return false;
        }
        else
        {
            // Add file to ZIP - simplified implementation
            QFile file( sourcePath );
            if( !file.open( QIODevice::ReadOnly ) )
            {
                aErrors += QString( _( "Could not read file: %1" ) ).arg( sourcePath );
                return false;
            }
            
            QByteArray data = file.readAll();
            aZip.write( data );
        }
    }

    return true;
}