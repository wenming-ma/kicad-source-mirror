#include <kicad_curl/kicad_curl_easy.h>

#include <bitmaps.h>
#include <build_version.h>
#include <common.h>
#include <pgm_base.h>
#include <tool/actions.h>
#include <tool/tool_manager.h>
#include <eda_draw_frame.h>
#include <view/view.h>
#include <gal/graphics_abstraction_layer.h>
#include <base_screen.h>
#include <tool/common_control.h>
#include <id.h>
#include <kiface_base.h>
#include <dialogs/dialog_configure_paths.h>
#include <eda_doc.h>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>

#define URL_GET_INVOLVED QString( "https://go.kicad.org/contribute/" )
#define URL_DONATE QString( "https://go.kicad.org/app-donate" )
#define URL_DOCUMENTATION QString( "https://go.kicad.org/docs/" )

QString COMMON_CONTROL::m_bugReportUrl =
        QString( "https://gitlab.com/kicad/code/kicad/-/issues/new?issuable_template=bare&issue"
                 "[description]=%1" );

QString COMMON_CONTROL::m_bugReportTemplate = QString(
        "```\n"
        "%1\n"
        "```" );

void COMMON_CONTROL::Reset( RESET_REASON aReason )
{
    m_frame = getEditFrame<EDA_BASE_FRAME>();
}

int COMMON_CONTROL::OpenPreferences( const TOOL_EVENT& aEvent )
{
    m_frame->ShowPreferences( QString(), QString() );
    return 0;
}

int COMMON_CONTROL::ConfigurePaths( const TOOL_EVENT& aEvent )
{
    if( KIFACE* pcbnew = m_frame->Kiway().KiFACE( KIWAY::FACE_PCB, false ) )
    {
        try
        {
            pcbnew->CreateKiWindow( m_frame, DIALOG_CONFIGUREPATHS, &m_frame->Kiway() );
        }
        catch( ... )
        {
            // Do nothing here.
            // A error message is displayed after trying to load _pcbnew.kiface.
        }
    }
    else
    {
        DIALOG_CONFIGURE_PATHS dlg( m_frame );
        if( dlg.exec() == QDialog::Accepted )
            m_frame->Kiway().CommonSettingsChanged( ENVVARS_CHANGED );
    }

    return 0;
}

int COMMON_CONTROL::ShowLibraryTable( const TOOL_EVENT& aEvent )
{
    if( aEvent.IsAction( &ACTIONS::showSymbolLibTable ) )
    {
        try
        {
            if( KIFACE* kiface = m_frame->Kiway().KiFACE( KIWAY::FACE_SCH ) )
                kiface->CreateKiWindow( m_frame, DIALOG_SCH_LIBRARY_TABLE, &m_frame->Kiway() );
        }
        catch( ... )
        {
            // _eeschema.kiface is not available: it contains the library table dialog.
            // Do nothing here.
            // A error message is displayed after trying to load _eeschema.kiface.
        }
    }
    else if( aEvent.IsAction( &ACTIONS::showFootprintLibTable ) )
    {
        try
        {
            if( KIFACE* kiface = m_frame->Kiway().KiFACE( KIWAY::FACE_PCB ) )
                kiface->CreateKiWindow( m_frame, DIALOG_PCB_LIBRARY_TABLE, &m_frame->Kiway() );
        }
        catch( ... )
        {
            // _pcbnew.kiface is not available: it contains the library table dialog.
            // Do nothing here.
            // A error message is displayed after trying to load _pcbnew.kiface.
        }
    }
    else if( aEvent.IsAction( &ACTIONS::showDesignBlockLibTable ) )
    {
        try
        {
            if( KIFACE* kiface = m_frame->Kiway().KiFACE( KIWAY::FACE_SCH ) )
                kiface->CreateKiWindow( m_frame, DIALOG_DESIGN_BLOCK_LIBRARY_TABLE,
                                        &m_frame->Kiway() );
        }
        catch( ... )
        {
            // _eeschema.kiface is not available: it contains the library table dialog.
            // Do nothing here.
            // A error message is displayed after trying to load _eeschema.kiface.
        }
    }

    return 0;
}

void showFrame( EDA_BASE_FRAME* aFrame )
{
    if( aFrame->isMinimized() )
        aFrame->showNormal();

    aFrame->raise();
    aFrame->activateWindow();

    if( QWidget* blocking_win = aFrame->Kiway().GetBlockingDialog() )
    {
        blocking_win->raise();
        blocking_win->activateWindow();
    }
}

int COMMON_CONTROL::ShowPlayer( const TOOL_EVENT& aEvent )
{
    FRAME_T       playerType = aEvent.Parameter<FRAME_T>();
    KIWAY_PLAYER* editor = m_frame->Kiway().Player( playerType, true );

    if( editor == nullptr )
    {
        // Cannot open/create the editor frame
        return 0;
    }

    showFrame( editor );

    return 0;
}

int COMMON_CONTROL::ShowProjectManager( const TOOL_EVENT& aEvent )
{
    EDA_BASE_FRAME* top = static_cast<EDA_BASE_FRAME*>( m_frame->Kiway().GetTop() );

    if( top && top->GetFrameType() == KICAD_MAIN_FRAME_T )
    {
        showFrame( top );
    }
    else
    {
        QMessageBox::information( m_frame, QString(),
                                  _( "Can not switch to project manager in stand-alone mode." ) );
    }

    return 0;
}

int COMMON_CONTROL::ShowHelp( const TOOL_EVENT& aEvent )
{
    QString helpFile;
    QString msg;

    const QString baseUrl = URL_DOCUMENTATION + GetMajorMinorVersion() + QString( "/" )
                           + Pgm().GetLocale()->GetName().section( '_', 0, 0 ) + QString( "/" );

    if( aEvent.IsAction( &ACTIONS::gettingStarted ) )
    {
        const QString names[2] = {
                QString( "getting_started_in_kicad" ),
                QString( "Getting_Started_in_KiCad" )
        };

        for( const QString& name : names )
        {
            helpFile = SearchHelpFileFullPath( name );

            if( !helpFile.isEmpty() )
                break;
        }

        if( helpFile.isEmpty() )
        {
            msg = QString( _( "Help file '%1' or\n'%2' could not be found.\n"
                             "Do you want to access the KiCad online help?" ) )
                    .arg( names[0] ).arg( names[1] );
            
            QMessageBox msgBox( m_frame );
            msgBox.setText( msg );
            msgBox.setWindowTitle( _( "File Not Found" ) );
            msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
            msgBox.setDefaultButton( QMessageBox::No );

            if( msgBox.exec() != QMessageBox::Yes )
                return -1;

            helpFile = baseUrl + names[0] + QString( "/" );
        }
    }
    else
    {
        QString base_name = m_frame->help_name();

        helpFile = SearchHelpFileFullPath( base_name );

        if( helpFile.isEmpty() )
        {
            msg = QString( _( "Help file '%1' could not be found.\n"
                             "Do you want to access the KiCad online help?" ) )
                    .arg( base_name );
            
            QMessageBox msgBox( m_frame );
            msgBox.setText( msg );
            msgBox.setWindowTitle( _( "File Not Found" ) );
            msgBox.setStandardButtons( QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
            msgBox.setDefaultButton( QMessageBox::No );

            if( msgBox.exec() != QMessageBox::Yes )
                return -1;

            helpFile = baseUrl + base_name + QString( "/" );
        }
    }

    GetAssociatedDocument( m_frame, helpFile, &m_frame->Prj() );
    return 0;
}

int COMMON_CONTROL::About( const TOOL_EVENT& aEvent )
{
    void ShowAboutDialog( EDA_BASE_FRAME * aParent );
    ShowAboutDialog( m_frame );
    return 0;
}

int COMMON_CONTROL::ListHotKeys( const TOOL_EVENT& aEvent )
{
    DisplayHotkeyList( m_frame );
    return 0;
}

int COMMON_CONTROL::GetInvolved( const TOOL_EVENT& aEvent )
{
    if( !QDesktopServices::openUrl( QUrl( URL_GET_INVOLVED ) ) )
    {
        QString msg = QString( _( "Could not launch the default browser.\n"
                                 "For information on how to help the KiCad project, visit %1" ) )
                        .arg( URL_GET_INVOLVED );
        QMessageBox::information( m_frame, _( "Get involved with KiCad" ), msg );
    }

    return 0;
}

int COMMON_CONTROL::Donate( const TOOL_EVENT& aEvent )
{
    if( !QDesktopServices::openUrl( QUrl( URL_DONATE ) ) )
    {
        QString msg = QString( _( "Could not launch the default browser.\n"
                                 "To donate to the KiCad project, visit %1" ) )
                        .arg( URL_DONATE );
        QMessageBox::information( m_frame, _( "Donate to KiCad" ), msg );
    }

    return 0;
}

int COMMON_CONTROL::ReportBug( const TOOL_EVENT& aEvent )
{
    if( WarnUserIfOperatingSystemUnsupported() )
        return 0;

    QString version = GetVersionInfoData( m_frame->GetUntranslatedAboutTitle(), false, true );
    QString message = m_bugReportTemplate.arg( version );

    KICAD_CURL_EASY kcurl;
    QString url_string = m_bugReportUrl.arg( QString::fromStdString( kcurl.Escape( std::string( message.toUtf8() ) ) ) );
    QDesktopServices::openUrl( QUrl( url_string ) );

    return 0;
}

void COMMON_CONTROL::setTransitions()
{
    Go( &COMMON_CONTROL::OpenPreferences,    ACTIONS::openPreferences.MakeEvent() );
    Go( &COMMON_CONTROL::ConfigurePaths,     ACTIONS::configurePaths.MakeEvent() );
    Go( &COMMON_CONTROL::ShowLibraryTable,   ACTIONS::showSymbolLibTable.MakeEvent() );
    Go( &COMMON_CONTROL::ShowLibraryTable,   ACTIONS::showFootprintLibTable.MakeEvent() );
    Go( &COMMON_CONTROL::ShowLibraryTable,   ACTIONS::showDesignBlockLibTable.MakeEvent() );
    Go( &COMMON_CONTROL::ShowPlayer,         ACTIONS::showSymbolBrowser.MakeEvent() );
    Go( &COMMON_CONTROL::ShowPlayer,         ACTIONS::showSymbolEditor.MakeEvent() );
    Go( &COMMON_CONTROL::ShowPlayer,         ACTIONS::showFootprintBrowser.MakeEvent() );
    Go( &COMMON_CONTROL::ShowPlayer,         ACTIONS::showFootprintEditor.MakeEvent() );
    Go( &COMMON_CONTROL::ShowProjectManager, ACTIONS::showProjectManager.MakeEvent() );

    Go( &COMMON_CONTROL::ShowHelp,           ACTIONS::gettingStarted.MakeEvent() );
    Go( &COMMON_CONTROL::ShowHelp,           ACTIONS::help.MakeEvent() );
    Go( &COMMON_CONTROL::ListHotKeys,        ACTIONS::listHotKeys.MakeEvent() );
    Go( &COMMON_CONTROL::GetInvolved,        ACTIONS::getInvolved.MakeEvent() );
    Go( &COMMON_CONTROL::Donate,             ACTIONS::donate.MakeEvent() );
    Go( &COMMON_CONTROL::ReportBug,          ACTIONS::reportBug.MakeEvent() );
    Go( &COMMON_CONTROL::About,              ACTIONS::about.MakeEvent() );
}