#include <common.h>
#include <pgm_base.h>
#include <paths.h>
#include <systemdirsappend.h>
#include <trace_helpers.h>

#include <vector>
#include <string>
#include <QLoggingCategory>
#include <QLocale>
#include <QDir>
#include <QFileInfo>


QString SearchHelpFileFullPath( const QString& aBaseName )
{
    SEARCH_STACK basePaths;
    QString      helpFile;

    // help files are most likely located in the documentation install path
    basePaths.Add( PATHS::GetDocumentationPath() );

#ifndef __WIN32__
    // just in case, add all known system directories to the search stack
    SystemDirsAppend( &basePaths );
#endif

#if defined( DEBUG )
    basePaths.Show( QString( __func__ ) + ": basePaths" );
#endif

    // By default, the documentation from kicad-doc is installed to a folder called "help" with
    // subdirectories for all supported languages. Although this can be changed at build-time by
    // overwriting ${KICAD_DOC_PATH}, the best guess KiCad can make is that help files are always
    // located in a folder named "help". If no translation matching the current locale settings is
    // available, the English version will be returned instead.

    QLocale*                currentLocale = Pgm().GetLocale();
    std::vector<std::string> localeNameDirs;

    // canonical form of the current locale (e.g., "fr_FR")
    localeNameDirs.push_back( currentLocale->name().toStdString() );

    // short form of the current locale (e.g., "fr")
    // QLocale::name() returns the full name, extract the language part
    QString fullName = currentLocale->name();
    int underscorePos = fullName.indexOf('_');
    QString shortName = (underscorePos != -1) ? fullName.left(underscorePos) : fullName;
    localeNameDirs.push_back( shortName.toStdString() );

    // plain English (in case a localised version of the help file cannot be found)
    localeNameDirs.push_back( "en" );

    for( const std::string& locale : localeNameDirs )
    {
        SEARCH_STACK docPaths;

        for( QString& base : basePaths )
        {
            QDir path( base );

            // add <base>/help/<locale>/
            QDir helpPath = path;
            helpPath.cd( "help" );
            helpPath.cd( QString::fromStdString( locale ) );
            docPaths.AddPaths( helpPath.absolutePath() );

            // add <base>/doc/help/<locale>/
            QDir docHelpPath = path;
            docHelpPath.cd( "doc" );
            docHelpPath.cd( "help" );
            docHelpPath.cd( QString::fromStdString( locale ) );
            docPaths.AddPaths( docHelpPath.absolutePath() );

            // add <base>/doc/kicad/help/<locale>/
            QDir docKicadPath = path;
            docKicadPath.cd( "doc" );
            docKicadPath.cd( "kicad" );
            docKicadPath.cd( "help" );
            docKicadPath.cd( QString::fromStdString( locale ) );
            docPaths.AddPaths( docKicadPath.absolutePath() );
        }

#if defined( DEBUG )
        docPaths.Show( QString( __func__ ) + ": docPaths (" + QString::fromStdString( locale ) + ")" );
#endif

        // search HTML first, as it is the preferred format for help files
        qCDebug( tracePathsAndFiles ) << "Checking SEARCH_STACK for file" << aBaseName << ".html";
        helpFile = docPaths.FindValidPath( aBaseName + ".html" );

        if( !helpFile.isEmpty() )
        {
            // prepend URI protocol to open the file in a browser
            helpFile = "file://" + helpFile;
            break;
        }

        // search PDF only when no corresponding HTML file was found
        qCDebug( tracePathsAndFiles ) << "Checking SEARCH_STACK for file" << aBaseName << ".pdf";
        helpFile = docPaths.FindValidPath( aBaseName + ".pdf" );

        if( !helpFile.isEmpty() )
            break;
    }

    return helpFile;
}
