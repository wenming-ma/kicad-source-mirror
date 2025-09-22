
#include <symbol_library.h>
#include <confirm.h>
// UNUSED_SYMBOL: HTML_MESSAGE_BOX - Header not needed since class is unused
// #include <dialogs/html_message_box.h>
#include <kiface_base.h>
#include <pgm_base.h>
#include <QApplication>
#include <QWidget>
#include <QDir>
#include <QStringList>
#include <QDebug>
#include <core/utf8.h>
#include <symbol_lib_table.h>
#include <project_sch.h>

static std::mutex s_symbolTableMutex;

// non-member so it can be moved easily, and kept REALLY private.
// Do NOT Clear() in here.
static void add_search_paths( SEARCH_STACK* aDst, const SEARCH_STACK& aSrc, int aIndex )
{
    for( unsigned i=0; i<aSrc.GetCount();  ++i )
        aDst->AddPaths( aSrc[i], aIndex );
}


SEARCH_STACK* PROJECT_SCH::SchSearchS( PROJECT* aProject )
{
    SEARCH_STACK* ss = (SEARCH_STACK*) aProject->GetElem( PROJECT::ELEM::SCH_SEARCH_STACK );

    Q_ASSERT( !ss || dynamic_cast<SEARCH_STACK*>( ss ) );

    if( !ss )
    {
        ss = new SEARCH_STACK();

        // Make PROJECT the new SEARCH_STACK owner.
        aProject->SetElem( PROJECT::ELEM::SCH_SEARCH_STACK, ss );

        // to the empty SEARCH_STACK for SchSearchS(), add project dir as first
        ss->AddPaths( aProject->GetProjectDirectory() );

        // next add the paths found in *.pro, variable "LibDir"
        QString        libDir;

        try
        {
            SYMBOL_LIBS::GetLibNamesAndPaths( aProject, &libDir );
        }
        catch( const IO_ERROR& )
        {
        }

        if( !libDir.isEmpty() )
        {
            QStringList   paths;

            SEARCH_STACK::Split( &paths, libDir );

            for( unsigned i =0; i<paths.size();  ++i )
            {
                QString path = aProject->AbsolutePath( paths[i] );

                ss->AddPaths( path );     // at the end
            }
        }

        // append all paths from aSList
        // add_search_paths( ss, Kiface().KifaceSearch(), -1 ); // UNUSED_SYMBOL: Kiface function not available
    }

    return ss;
}


SYMBOL_LIBS* PROJECT_SCH::SchLibs( PROJECT* aProject )
{
    SYMBOL_LIBS* libs = (SYMBOL_LIBS*) aProject->GetElem( PROJECT::ELEM::SCH_SYMBOL_LIBS );

    Q_ASSERT( !libs || libs->ProjectElementType() == PROJECT::ELEM::SCH_SYMBOL_LIBS );

    if( !libs )
    {
        libs = new SYMBOL_LIBS();

        // Make PROJECT the new SYMBOL_LIBS owner.
        aProject->SetElem( PROJECT::ELEM::SCH_SYMBOL_LIBS, libs );

        try
        {
            libs->LoadAllLibraries( aProject );
        }
        catch( const PARSE_ERROR& pe )
        {
            // UNUSED_SYMBOL: HTML_MESSAGE_BOX constructor - Constructor and related methods not compiled in minimal set
            /*
            QString    lib_list = UTF8( pe.inputLine );
            QWidget*   parent = Pgm().App().GetTopWindow();

            // parent of this dialog cannot be NULL since that breaks the Kiway() chain.
            HTML_MESSAGE_BOX dlg( parent, _( "Not Found" ) );

            dlg.MessageSet( _( "The following libraries were not found:" ) );
            dlg.ListSet( lib_list );
            dlg.Layout();

            dlg.ShowModal();
            */
        }
        catch( const IO_ERROR& ioe )
        {
            QWidget* parent = Pgm().App().GetTopWindow();

            DisplayError( parent, ioe.What() );
        }
    }

    return libs;
}


SYMBOL_LIB_TABLE* PROJECT_SCH::SchSymbolLibTable( PROJECT* aProject )
{
    std::lock_guard<std::mutex> lock( s_symbolTableMutex );

    // This is a lazy loading function, it loads the project specific table when
    // that table is asked for, not before.
    SYMBOL_LIB_TABLE* tbl =
            (SYMBOL_LIB_TABLE*) aProject->GetElem( PROJECT::ELEM::SYMBOL_LIB_TABLE );

    // its gotta be NULL or a SYMBOL_LIB_TABLE, or a bug.
    Q_ASSERT( !tbl || tbl->ProjectElementType() == PROJECT::ELEM::SYMBOL_LIB_TABLE );

    if( !tbl )
    {
        // Stack the project specific SYMBOL_LIB_TABLE overlay on top of the global table.
        // ~SYMBOL_LIB_TABLE() will not touch the fallback table, so multiple projects may
        // stack this way, all using the same global fallback table.
        tbl = new SYMBOL_LIB_TABLE( &SYMBOL_LIB_TABLE::GetGlobalLibTable() );

        aProject->SetElem( PROJECT::ELEM::SYMBOL_LIB_TABLE, tbl );

        QString prjPath;

        prjPath = qgetenv( PROJECT_VAR_NAME );

        if( !prjPath.isEmpty() )
        {
            QDir dir( prjPath );
            QString fn = dir.filePath( SYMBOL_LIB_TABLE::GetSymbolLibTableFileName() );

            try
            {
                tbl->Load( fn );
            }
            catch( const IO_ERROR& ioe )
            {
                QString msg;
                msg = QString::asprintf( "Error loading the symbol library table '%s'.", fn.toStdString().c_str() );
                DisplayErrorMessage( nullptr, msg, ioe.What() );
            }
        }
    }

    return tbl;
}
