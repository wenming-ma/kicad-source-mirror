#include <QStandardPaths>
#include <QFileInfo>
#include <QDir>
#include <QProcessEnvironment>

#include <systemdirsappend.h>
#include <common.h>
#include <kiplatform/environment.h>
#include <search_stack.h>
#include <pgm_base.h>
#include <config.h>     // to define DEFAULT_INSTALL_PATH
#include <paths.h>

// Put your best guesses in here, send the computer on a wild goose chase, its
// got nothing else to do.

void SystemDirsAppend( SEARCH_STACK* aSearchStack )
{
    // No clearing is done here, the most general approach is NOT to assume that
    // our appends will be the only thing in the stack.  This function has no
    // knowledge of caller's intentions.

    // QProcessEnvironment is used for cross-platform environment access.
    // SEARCH_STACK::AddPaths() will verify readability and existence of
    // each directory before adding.
    SEARCH_STACK maybe;

    // User environment variable path is the first search path.  Chances are
    // if the user is savvy enough to set an environment variable they know
    // what they are doing.  It should take precedence over anything else.
    // Otherwise don't set it.
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    maybe.AddPaths( env.value( "KICAD" ).toStdString() );

#ifdef __APPLE__
    // Add the directory for the user-dependent, program specific data files.
    maybe.AddPaths( PATHS::GetOSXKicadUserDataDir().toStdString() );

    // Global machine specific application data
    maybe.AddPaths( PATHS::GetOSXKicadMachineDataDir().toStdString() );

    // Global application specific data files inside bundle
    maybe.AddPaths( PATHS::GetOSXKicadDataDir().toStdString() );
#else
    // This is from CMAKE_INSTALL_PREFIX.
    // Useful when KiCad is installed by `make install`.
    // Use as second ranked place.
    maybe.AddPaths( std::string( DEFAULT_INSTALL_PATH ) );

#ifdef __linux__
    // On Linux, the stock EDA library data install path can be redefined via
    // KICAD_LIBRARY_DATA, otherwise KICAD_DATA will be used.
    // Useful when multiple versions of KiCad are installed in parallel.
    maybe.AddPaths( PATHS::GetStockEDALibraryPath().toStdString() );
#endif

    // Add the directory for the user-dependent, program specific data files.
    // According to Qt documentation:
    // Unix: ~/.appname
    // Windows: C:\Documents and Settings\username\Application Data\appname
    maybe.AddPaths( KIPLATFORM::ENV::GetDocumentsPath().toStdString() );

    {
        // Should be full path to this program executable.
        QString   bin_dir = Pgm().GetExecutablePath();

#if defined(_WIN32)
        // bin_dir uses unix path separator.  So to parse with QFileInfo
        // use windows separator, especially important for server inclusion:
        // like: \\myserver\local_path .
        bin_dir.replace( '/', QDir::separator() );
#endif

        QFileInfo bin_fn( bin_dir );

        // Dir of the global (not user-specific), application specific, data files.
        // From Qt docs:
        // Unix: prefix/share/appname
        // Windows: the directory where the executable file is located
        // Mac: appname.app/Contents/SharedSupport bundle subdirectory
        QString data_dir = QStandardPaths::writableLocation( QStandardPaths::AppDataLocation );

        if( bin_fn.absolutePath() != data_dir )
        {
            // add data_dir if it is different from the bin_dir
            maybe.AddPaths( data_dir.toStdString() );
        }

        // Up one level relative to binary path with "share" appended below.
        QDir parent_dir( bin_fn.absolutePath() );
        parent_dir.cdUp();
        maybe.AddPaths( parent_dir.absolutePath().toStdString() );
    }

    /* The normal OS program file install paths allow for a binary to be
     * installed in a different path from the library files.  This is
     * useful for development purposes so the library and documentation
     * files do not need to be installed separately.  If someone can
     * figure out a way to implement this without #ifdef, please do.
     */
#if defined(_WIN32)
    maybe.AddPaths( env.value( "PROGRAMFILES" ).toStdString() );
#else
    maybe.AddPaths( env.value( "PATH" ).toStdString() );
#endif
#endif

#if defined(DEBUG) && 0
    maybe.Show( "maybe wish list" );
#endif

    // Append 1) kicad, 2) kicad/share, 3) share, and 4) share/kicad to each
    // possible base path in 'maybe'. Since SEARCH_STACK::AddPaths() will verify
    // readability and existence of each directory, not all of these will be
    // actually appended.
    for( unsigned i = 0; i < maybe.GetCount();  ++i )
    {
        QDir fn( QString::fromStdString( maybe[i] ) );

#ifndef __APPLE__
        if( fn.dirName() == QString( "bin" ) )
        {
            fn.cdUp();

            if( fn.isRoot() )
                continue;               // at least on linux
        }
#endif

        aSearchStack->AddPaths( fn.absolutePath().toStdString() );

#ifndef __APPLE__
        fn.cd( QString( "kicad" ) );
        aSearchStack->AddPaths( fn.absolutePath().toStdString() );     // add maybe[i]/kicad

        fn.cd( QString( "share" ) );
        aSearchStack->AddPaths( fn.absolutePath().toStdString() );     // add maybe[i]/kicad/share

        fn.cdUp();                                       // ../  clear share
        fn.cdUp();                                       // ../  clear kicad

        fn.cd( QString( "share" ) );
        aSearchStack->AddPaths( fn.absolutePath().toStdString() );     // add maybe[i]/share

        fn.cd( QString( "kicad" ) );
        aSearchStack->AddPaths( fn.absolutePath().toStdString() );     // add maybe[i]/share/kicad
#endif
    }

#if defined(DEBUG) && 0
    // final results:
    aSearchStack->Show( __func__ );
#endif
}


void GlobalPathsAppend( SEARCH_STACK* aDst, KIWAY::FACE_T aId )
{
    SEARCH_STACK bases;

    SystemDirsAppend( &bases );
    aDst->Clear();

    for( unsigned i = 0; i < bases.GetCount(); ++i )
    {
        QDir fn( QString::fromStdString( bases[i] ) );

        // Add schematic library file path to search path list.
        // we must add <kicad path>/library and <kicad path>/library/doc
        if( aId == KIWAY::FACE_SCH )
        {
            // Add schematic doc file path (library/doc) to search path list.

            fn.cd( QString( "library" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );

            fn.cd( QString( "doc" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );

            fn.cdUp();
            fn.cdUp(); // "../../"  up twice, removing library/doc/

            fn.cd( QString( "symbols" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );

            fn.cd( QString( "doc" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );

            fn.cdUp();
            fn.cdUp(); // "../../"  up twice, removing symbols/doc/
        }

        // Add PCB library file path to search path list.
        if( aId == KIWAY::FACE_PCB || aId == KIWAY::FACE_CVPCB )
        {
            fn.cd( QString( "modules" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );
            fn.cdUp();

            fn.cd( QString( "footprints" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );
            fn.cdUp();

            // Add 3D module library file path to search path list.
            fn.cd( QString( "3dmodels" ) );
            aDst->AddPaths( fn.absolutePath().toStdString() );
            fn.cdUp();
        }

        // Add KiCad template file path to search path list.
        fn.cd( QString( "template" ) );
        aDst->AddPaths( fn.absolutePath().toStdString() );
    }

#ifndef __APPLE__
    aDst->AddPaths( std::string( "/usr/local/share" ) );
#endif
}