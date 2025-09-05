
#include <cstring>

#include <core/ignore.h>
#include <macros.h>
#include <kiway.h>
#include <kiway_player.h>
#include <kiway_express.h>
#include <pgm_base.h>
#include <config.h>
#include <core/arraydim.h>
#include <id.h>
#include <kiplatform/app.h>
#include <kiplatform/environment.h>
#include <settings/settings_manager.h>
#include <tool/action_manager.h>
#include <logging.h>

#include <QLibrary>
#include <QStandardPaths>
#include <QDebug>
#include <QCoreApplication>
#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QCloseEvent>
#include <confirm.h>

#ifdef KICAD_USE_SENTRY
#include <sentry.h>
#endif

KIFACE* KIWAY::m_kiface[KIWAY_FACE_COUNT];
int     KIWAY::m_kiface_version[KIWAY_FACE_COUNT];



KIWAY::KIWAY( int aCtlBits, QMainWindow* aTop ):
     m_ctl( aCtlBits ), m_top( nullptr ), m_blockingDialog( -1 )
{
    SetTop( aTop );     // hook player_destroy_handler() into aTop.

    // Set the array of all known frame window IDs to empty = -1,
    // once they are be created, they are added with FRAME_T as index to this array.
    // Note: A non empty entry does not mean the frame still exists.
    //   It means only the frame was created at least once. It can be destroyed after.
    //   These entries are not cleared automatically on window closing. The purpose is just
    //   to allow a call to QWidget::find() using a FRAME_T frame type
    for( int n = 0; n < KIWAY_PLAYER_COUNT; n++ )
        m_playerFrameId[n] = -1;
}


#if 0
// Any event types derived from QEvent, like window destroy events, are
// handled through Qt's event system. The m_top window should receive all destroy events
// originating from KIWAY_PLAYERs. This handler eavesdrops on that event stream
// looking for KIWAY_PLAYERs being closed.

void KIWAY::player_destroy_handler( QCloseEvent& event )
{
    // Currently : do nothing
    event.accept();  // accept the close event
}
#endif


void KIWAY::SetTop( QMainWindow* aTop )
{
#if 0
    // Qt event handling would be different - using QObject::connect
    // and QWidget's closeEvent() virtual method override instead of
    // the old wxWidgets Connect/Disconnect pattern
#endif

    m_top = aTop;
}


const QString KIWAY::dso_search_path( FACE_T aFaceId )
{
    const char*   name;

    switch( aFaceId )
    {
    case FACE_SCH:              name = KIFACE_PREFIX "eeschema";            break;
    case FACE_PCB:              name = KIFACE_PREFIX "pcbnew";              break;
    case FACE_CVPCB:            name = KIFACE_PREFIX "cvpcb";               break;
    case FACE_GERBVIEW:         name = KIFACE_PREFIX "gerbview";            break;
    case FACE_PL_EDITOR:        name = KIFACE_PREFIX "pl_editor";           break;
    case FACE_PCB_CALCULATOR:   name = KIFACE_PREFIX "pcb_calculator";      break;
    case FACE_BMP2CMP:          name = KIFACE_PREFIX "bitmap2component";    break;
    case FACE_PYTHON:           name = KIFACE_PREFIX "kipython";            break;

    default:
        Q_ASSERT_X( false, "KIWAY::dso_search_path", "caller has a bug, passed a bad aFaceId" );
        return QString();
    }

#if !defined(Q_OS_MAC)
    QString path;

    if( m_ctl & (KFCTL_STANDALONE | KFCTL_CPP_PROJECT_SUITE) )
    {
        // The 2 *.cpp program launchers: single_top.cpp and kicad.cpp expect
        // the *.kiface's to reside in same directory as their binaries do.
        path = QCoreApplication::applicationFilePath();
    }

    QFileInfo fn( path );
#else
    // we have the dso's in main OSX bundle kicad.app/Contents/PlugIns
    QFileInfo fn( Pgm().GetExecutablePath() );
    QString dirPath = fn.absolutePath();
    dirPath += "/Contents";
    dirPath += "/PlugIns";
    fn = QFileInfo( dirPath );
#endif

    QString baseName = QString::fromUtf8( name );
    QString finalPath = fn.absolutePath() + "/" + baseName;

    // To speed up development, it's sometimes nice to run kicad from inside
    // the build path.  In that case, each program will be in a subdirectory.
    // To find the DSOs, we need to go up one directory and then enter a subdirectory.
    if( !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
    {
#if defined(Q_OS_MAC)
        // On Mac, all of the kifaces are placed in the kicad.app bundle, even though the individual
        // standalone binaries are placed in separate bundles before the make install step runs.
        // So, we have to jump up to the kicad directory, then the PlugIns section of the kicad
        // bundle.
        QFileInfo execInfo( QCoreApplication::applicationFilePath() );
        QString macPath = execInfo.absolutePath();
        
        // Remove 4 directories and rebuild path
        QStringList pathParts = macPath.split( '/', Qt::SkipEmptyParts );
        if( pathParts.size() >= 4 )
        {
            pathParts.removeLast();
            pathParts.removeLast();
            pathParts.removeLast();
            pathParts.removeLast();
        }
        macPath = "/" + pathParts.join( "/" );
        macPath += "/kicad";
        macPath += "/kicad.app";
        macPath += "/Contents";
        macPath += "/PlugIns";
        finalPath = macPath + "/" + baseName;
#else
        const char*   dirName;

        // The subdirectories usually have the same name as the kiface
        switch( aFaceId )
        {
            case FACE_PL_EDITOR: dirName = "pagelayout_editor";   break;
            case FACE_PYTHON:    dirName = "scripting";           break;
            default:             dirName = name + 1;              break;
        }

        QDir parentDir( fn.absolutePath() );
        parentDir.cdUp();
        parentDir.cd( QString::fromUtf8( dirName ) );
        finalPath = parentDir.absolutePath() + "/" + baseName;
#endif
    }

    // Here a "suffix" == an extension with a preceding '.',
    // so skip the preceding '.' to get an extension
    QString extension = QString::fromUtf8( &KIFACE_SUFFIX[1] );
    finalPath += "." + extension;

    return finalPath;
}


PROJECT& KIWAY::Prj() const
{
    return Pgm().GetSettingsManager().Prj();
}


KIFACE* KIWAY::KiFACE( FACE_T aFaceId, bool doLoad )
{
    // Since this will be called from python, cannot assume that code will
    // not pass a bad aFaceId.
    if( (unsigned) aFaceId >= arrayDim( m_kiface ) )
    {
        Q_ASSERT_X( false, "KIWAY::KiFACE", "caller has a bug, passed a bad aFaceId" );
        return nullptr;
    }

    // return the previously loaded KIFACE, if it was.
    if( m_kiface[aFaceId] )
        return m_kiface[aFaceId];

    // DSO with KIFACE has not been loaded yet, does caller want to load it?
    if( doLoad )
    {
        QString dname = dso_search_path( aFaceId );

        // Insert DLL search path for kicad_3dsg from build dir
        if( !qEnvironmentVariable( "KICAD_RUN_FROM_BUILD_DIR" ).isEmpty() )
        {
            QFileInfo myPath( QCoreApplication::applicationFilePath() );

            if( !myPath.absolutePath().endsWith( "pcbnew" ) )
            {
                QDir dir( myPath.absolutePath() );
                dir.cdUp();
                dir.cd( "pcbnew" );
                KIPLATFORM::APP::AddDynamicLibrarySearchPath( dir.absolutePath() );
            }
        }

        QString msg;

#ifdef KICAD_WIN32_VERIFY_CODESIGN
        bool codeSignOk = KIPLATFORM::ENV::VerifyFileSignature( dname );
        if( !codeSignOk )
        {
            msg.Printf( _( "Failed to verify kiface library '%s' signature." ), dname );
            THROW_IO_ERROR( msg );
        }
#endif

        QLibrary dso;

        void*   addr = nullptr;

        // For some reason QLibrary::load() crashes in some languages
        // (chinese for instance) when loading the dynamic library.
        // The crash happens for Eeschema.
        // So switch to "C" locale during loading (LC_COLLATE is enough).
        int lc_new_type = LC_COLLATE;
        std::string user_locale = setlocale( lc_new_type, nullptr );
        setlocale( lc_new_type, "C" );

        dso.setFileName( dname );
        bool success = dso.load();

        setlocale( lc_new_type, user_locale.c_str() );

#ifdef KICAD_USE_SENTRY
        if( Pgm().IsSentryOptedIn() )
        {
            msg = QString::asprintf( "Loading kiface %d", aFaceId );
            sentry_value_t crumb = sentry_value_new_breadcrumb( "navigation", msg.toUtf8().data() );
            sentry_value_set_by_key( crumb, "category", sentry_value_new_string( "kiway.kiface" ) );
            sentry_value_set_by_key( crumb, "level", sentry_value_new_string( "info" ) );
            sentry_add_breadcrumb( crumb );
        }
#endif

        if( !success )
        {
            // Failure: error reporting UI was done via qDebug().
            // No further reporting required here.  Apparently this is not true on all
            // platforms and/or Qt builds and KiCad will crash.  Throwing the exception
            // here and catching it in the KiCad launcher resolves the crash issue.

            msg = QString( _( "Failed to load kiface library '%1'." ) ).arg( dname );
            THROW_IO_ERROR( msg );
        }
        else if( ( addr = dso.resolve( KIFACE_INSTANCE_NAME_AND_VERSION ) ) == nullptr )
        {
            // Failure: error reporting UI was done via qDebug().
            // No further reporting required here.  Assume the same thing applies here as
            // above with the load() call.  This has not been tested.
            msg = QString( _( "Could not read instance name and version from kiface library '%1'." ) ).arg( dname );
            THROW_IO_ERROR( msg );
        }
        else
        {
            KIFACE_GETTER_FUNC* ki_getter = (KIFACE_GETTER_FUNC*) addr;

            KIFACE* kiface = ki_getter( &m_kiface_version[aFaceId], KIFACE_VERSION, &Pgm() );

            // KIFACE_GETTER_FUNC function comment (API) says the non-NULL is unconditional.
            Q_ASSERT_X( kiface, "KIWAY::KiFACE", "attempted DSO has a bug, failed to return a KIFACE*" );

            void* dsoHandle = dso.handle();

            bool startSuccess = false;

            // Give the DSO a single chance to do its "process level" initialization.
            // "Process level" specifically means stay away from any projects in there.

            try
            {
                startSuccess = kiface->OnKifaceStart( &Pgm(), m_ctl, this );
            }
            catch (...)
            {
                // OnKiFaceStart may generate an exception
                // Before we continue and ultimately unload our module to retry we need
                // to process the exception before we delete the free the memory space the
                // exception resides in
                Pgm().HandleException( std::current_exception() );
            }

            if( startSuccess )
            {
                return m_kiface[aFaceId] = kiface;
            }
            else
            {
                // Usually means canceled initial global library setup
                // But it could have been an exception/failure
                // Let the module go out of scope to unload
                dso.unload();

                return nullptr;
            }
        }
    }

    return nullptr;
}


KIWAY::FACE_T KIWAY::KifaceType( FRAME_T aFrameType )
{
    switch( aFrameType )
    {
    case FRAME_SCH:
    case FRAME_SCH_SYMBOL_EDITOR:
    case FRAME_SCH_VIEWER:
    case FRAME_SYMBOL_CHOOSER:
    case FRAME_SIMULATOR:
        return FACE_SCH;

    case FRAME_PCB_EDITOR:
    case FRAME_FOOTPRINT_EDITOR:
    case FRAME_FOOTPRINT_VIEWER:
    case FRAME_FOOTPRINT_CHOOSER:
    case FRAME_FOOTPRINT_WIZARD:
    case FRAME_PCB_DISPLAY3D:
        return FACE_PCB;

    case FRAME_CVPCB:
    case FRAME_CVPCB_DISPLAY:
        return FACE_CVPCB;

    case FRAME_PYTHON:
        return FACE_PYTHON;

    case FRAME_GERBER:
        return FACE_GERBVIEW;

    case FRAME_PL_EDITOR:
        return FACE_PL_EDITOR;

    case FRAME_CALC:
        return FACE_PCB_CALCULATOR;

    case FRAME_BM2CMP:
        return FACE_BMP2CMP;

    default:
        return FACE_T( -1 );
    }
}


KIWAY_PLAYER* KIWAY::GetPlayerFrame( FRAME_T aFrameType )
{
    int storedId = m_playerFrameId[aFrameType];

    if( storedId == -1 )
        return nullptr;

    QWidget* frame = nullptr;
    for( QWidget* widget : QApplication::allWidgets() )
    {
        if( widget->winId() == storedId )
        {
            frame = widget;
            break;
        }
    }

    // Since QWidget searching is not cheap (especially if the window does not exist),
    // clear invalid entries to save CPU on repeated calls that do not lead to frame creation
    if( !frame )
        m_playerFrameId[aFrameType].compare_exchange_strong( storedId, -1 );

    return static_cast<KIWAY_PLAYER*>( frame );
}


KIWAY_PLAYER* KIWAY::Player( FRAME_T aFrameType, bool doCreate, QWidget* aParent )
{
    // Since this will be called from python, cannot assume that code will
    // not pass a bad aFrameType.
    if( (unsigned) aFrameType >= KIWAY_PLAYER_COUNT )
    {
        Q_ASSERT_X( false, "KIWAY::Player", "caller has a bug, passed a bad aFrameType" );
        return nullptr;
    }

    // return the previously opened window
    KIWAY_PLAYER* frame = GetPlayerFrame( aFrameType );

    if( frame )
        return frame;

    if( doCreate )
    {
        try
        {
#ifdef KICAD_USE_SENTRY
            if( Pgm().IsSentryOptedIn() )
            {
                QString msg = QString::asprintf( "Creating window type %d", aFrameType );
                sentry_value_t crumb = sentry_value_new_breadcrumb( "navigation", msg.toUtf8().data() );
                sentry_value_set_by_key( crumb, "category",
                                         sentry_value_new_string( "kiway.player" ) );
                sentry_value_set_by_key( crumb, "level", sentry_value_new_string( "info" ) );
                sentry_add_breadcrumb( crumb );
            }
#endif

            FACE_T  face_type = KifaceType( aFrameType );
            KIFACE* kiface = KiFACE( face_type );

            if( !kiface )
                return nullptr;

            frame = (KIWAY_PLAYER*) kiface->CreateKiWindow(
                                            aParent,    // Parent window of frame in modal mode,
                                                        // NULL in non modal mode
                                            aFrameType,
                                            this,
                                            m_ctl       // questionable need, these same flags
                                                        // were passed to KIFACE::OnKifaceStart()
                                            );
            if( frame )
                m_playerFrameId[aFrameType].store( frame->winId() );

            return frame;
        }
        catch( ... )
        {
            Pgm().HandleException( std::current_exception() );
            qCritical() << _( "Error loading editor." );
        }
    }

    return nullptr;
}


bool KIWAY::PlayerClose( FRAME_T aFrameType, bool doForce )
{
    // Since this will be called from python, cannot assume that code will
    // not pass a bad aFrameType.
    if( (unsigned) aFrameType >= KIWAY_PLAYER_COUNT )
    {
        Q_ASSERT_X( false, "KIWAY::PlayerClose", "caller has a bug, passed a bad aFrameType" );
        return false;
    }

    KIWAY_PLAYER* frame = GetPlayerFrame( aFrameType );

    if( frame == nullptr ) // Already closed
        return true;

#ifdef KICAD_USE_SENTRY
    if( Pgm().IsSentryOptedIn() )
    {
        QString msg = QString::asprintf( "Closing window type %d", aFrameType );
        sentry_value_t crumb = sentry_value_new_breadcrumb( "navigation", msg.toUtf8().data() );
        sentry_value_set_by_key( crumb, "category",
                                 sentry_value_new_string( "kiway.playerclose" ) );
        sentry_value_set_by_key( crumb, "level", sentry_value_new_string( "info" ) );
        sentry_add_breadcrumb( crumb );
    }
#endif

    if( frame->NonUserClose( doForce ) )
    {
        m_playerFrameId[aFrameType] = -1;
        return true;
    }

    return false;
}


bool KIWAY::PlayersClose( bool doForce )
{
    bool ret = true;

    for( unsigned i=0; i < KIWAY_PLAYER_COUNT;  ++i )
        ret = ret && PlayerClose( FRAME_T( i ), doForce );

    return ret;
}


void KIWAY::PlayerDidClose( FRAME_T aFrameType )
{
    m_playerFrameId[aFrameType] = -1;
}


void KIWAY::ExpressMail( FRAME_T aDestination, MAIL_T aCommand, std::string& aPayload,
                         QWidget* aSource )
{
    KIWAY_EXPRESS   mail( aDestination, aCommand, aPayload, aSource );

    ProcessEvent( mail );
}


void KIWAY::GetActions( std::vector<TOOL_ACTION*>& aActions ) const
{
    for( TOOL_ACTION* action : ACTION_MANAGER::GetActionList() )
        aActions.push_back( action );
}


void KIWAY::SetLanguage( int aLanguage )
{
    QString errMsg;
    bool    ret = false;

    {
        // Only allow the traces to be logged by Qt. We use our own system to log when the
        // OS doesn't support the language, so we want to hide the Qt error.
        Pgm().SetLanguageIdentifier( aLanguage );
        ret = Pgm().SetLanguage( errMsg );
    }

    if( !ret )
    {
        QString lang;

        for( unsigned ii = 0;  LanguagesList[ii].m_KI_Lang_Identifier != 0; ii++ )
        {
            if( aLanguage == LanguagesList[ii].m_KI_Lang_Identifier )
            {
                if( LanguagesList[ii].m_DoNotTranslate )
                    lang = QString::fromUtf8( LanguagesList[ii].m_Lang_Label );
                else
                    lang = QString::fromUtf8( LanguagesList[ii].m_Lang_Label );

                break;
            }
        }

        DisplayErrorMessage( nullptr,
                             QString( _( "Unable to switch language to %1" ) ).arg( lang ),
                             errMsg );
        return;
    }

#if 1
    // This is a risky hack that goes away if we allow the language to be
    // set only from the top most frame if !Kiface.IsSingle()

    // Only for the C++ project manager, and not for the python one and not for
    // single_top do we look for the EDA_BASE_FRAME as the top level window.
    // For single_top this is not needed because that window is registered in
    // the array below.
    if( m_ctl & KFCTL_CPP_PROJECT_SUITE )
    {
        // A dynamic_cast could be better, but creates link issues
        // (some basic_frame functions not found) on some platforms,
        // so a static_cast is used.
        EDA_BASE_FRAME* top = static_cast<EDA_BASE_FRAME*>( m_top );

        if ( top )
        {
            top->ShowChangedLanguage();
            QEvent e( QEvent::Type( EDA_LANG_CHANGED ) );
            QCoreApplication::sendEvent( top, &e );
        }
    }
#endif

    for( unsigned i=0;  i < KIWAY_PLAYER_COUNT;  ++i )
    {
        KIWAY_PLAYER* frame = GetPlayerFrame( ( FRAME_T )i );

        if( frame )
        {
            frame->ShowChangedLanguage();
            QEvent e( QEvent::Type( EDA_LANG_CHANGED ) );
            QCoreApplication::sendEvent( frame, &e );
        }
    }
}


void KIWAY::CommonSettingsChanged( int aFlags )
{
    if( m_ctl & KFCTL_CPP_PROJECT_SUITE )
    {
        // A dynamic_cast could be better, but creates link issues
        // (some basic_frame functions not found) on some platforms,
        // so a static_cast is used.
        EDA_BASE_FRAME* top = static_cast<EDA_BASE_FRAME*>( m_top );

        if( top )
            top->CommonSettingsChanged( aFlags );
    }

    for( unsigned i=0;  i < KIWAY_PLAYER_COUNT;  ++i )
    {
        KIWAY_PLAYER* frame = GetPlayerFrame( ( FRAME_T )i );

        if( frame )
            frame->CommonSettingsChanged( aFlags );
    }
}


void KIWAY::ProjectChanged()
{
#ifdef KICAD_USE_SENTRY
    sentry_value_t crumb = sentry_value_new_breadcrumb( "navigation", "Changing project" );
    sentry_value_set_by_key( crumb, "category", sentry_value_new_string( "kiway.projectchanged" ) );
    sentry_value_set_by_key( crumb, "level", sentry_value_new_string( "info" ) );
    sentry_add_breadcrumb( crumb );
#endif

    if( m_ctl & KFCTL_CPP_PROJECT_SUITE )
    {
        // A dynamic_cast could be better, but creates link issues
        // (some basic_frame functions not found) on some platforms,
        // so a static_cast is used.
        EDA_BASE_FRAME* top = static_cast<EDA_BASE_FRAME*>( m_top );

        if( top )
            top->ProjectChanged();
    }

    for( unsigned i=0;  i < KIWAY_PLAYER_COUNT;  ++i )
    {
        KIWAY_PLAYER* frame = GetPlayerFrame( ( FRAME_T )i );

        if( frame )
            frame->ProjectChanged();
    }
}


QWidget* KIWAY::GetBlockingDialog()
{
    if( m_blockingDialog == -1 )
        return nullptr;
        
    for( QWidget* widget : QApplication::allWidgets() )
    {
        if( widget->winId() == m_blockingDialog )
            return widget;
    }
    return nullptr;
}


void KIWAY::SetBlockingDialog( QWidget* aWin )
{
    if( !aWin )
        m_blockingDialog = -1;
    else
        m_blockingDialog = aWin->winId();
}


bool KIWAY::ProcessEvent( QEvent& aEvent )
{
    KIWAY_EXPRESS* mail = dynamic_cast<KIWAY_EXPRESS*>( &aEvent );

    if( mail )
    {
        FRAME_T dest = mail->Dest();

        // see if recipient is alive
        KIWAY_PLAYER* alive = Player( dest, false );

        if( alive )
        {
#if 1
            return QCoreApplication::sendEvent( alive, &aEvent );
#else
            alive->KiwayMailIn( *mail );
            return true;
#endif
        }
    }

    return false;
}


int KIWAY::ProcessJob( KIWAY::FACE_T aFace, JOB* job, REPORTER* aReporter, PROGRESS_REPORTER* aProgressReporter )
{
    KIFACE* kiface = KiFACE( aFace );

    return kiface->HandleJob( job, aReporter, aProgressReporter );
}


bool KIWAY::ProcessJobConfigDialog( KIWAY::FACE_T aFace, JOB* aJob, QWidget* aWindow )
{
    KIFACE* kiface = KiFACE( aFace );

    return kiface->HandleJobConfig( aJob, aWindow );
}


void KIWAY::OnKiCadExit()
{
    if( m_ctl & KFCTL_CPP_PROJECT_SUITE )
    {
        // A dynamic_cast could be better, but creates link issues
        // (some basic_frame functions not found) on some platforms,
        // so a static_cast is used.
        EDA_BASE_FRAME* top = static_cast<EDA_BASE_FRAME*>( m_top );

        if( top )
            top->Close( false );
    }
}


void KIWAY::OnKiwayEnd()
{
    for( KIFACE* i : m_kiface )
    {
        if( i )
            i->OnKifaceEnd();
    }
}
