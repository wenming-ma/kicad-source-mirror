
#ifndef KIWAY_H_
#define KIWAY_H_

// KIWAY and KIFACE classes for communication between process modules
// DSOs can send events between themselves using event handler interfaces
// Each project has its own KIWAY with actual PROJECT data structure


#include <atomic>
#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtWidgets/QWidget>
#include <QtWidgets/QMainWindow>
#include <import_export.h>
#include <search_stack.h>
#include <project.h>
#include <frame_type.h>
#include <mail_type.h>
#include <ki_exception.h>


#define KIFACE_VERSION      1
#define KIFACE_GETTER       KIFACE_1

// The KIFACE acquisition function is declared extern "C" so its name should not
// be mangled.
#define KIFACE_INSTANCE_NAME_AND_VERSION   "KIFACE_1"

#ifndef SWIG
#if defined(__linux__) || defined(__FreeBSD__)
 #define LIB_ENV_VAR    QStringLiteral( "LD_LIBRARY_PATH" )
#elif defined(Q_OS_MAC)
 #define LIB_ENV_VAR    QStringLiteral( "DYLD_LIBRARY_PATH" )
#elif defined(_WIN32)
 #define LIB_ENV_VAR    QStringLiteral( "PATH" )
#else
 #error Platform support missing
#endif
#endif  // SWIG

class QSettings;
class QWidget;
class PGM_BASE;
class KIWAY;
class KIWAY_PLAYER;
class QMainWindow;
class TOOL_ACTION;
class JOB;
class REPORTER;
class PROGRESS_REPORTER;


// KIFACE implements a participant in the KIWAY communication system
// Minimalistic software bus for communications between DLLs/DSOs
// Each project has its own KIWAY with actual PROJECT data structure
struct KIFACE
{
    // The order of functions establishes the vtable sequence, do not change the
    // order of functions in this listing unless you recompile all clients of
    // this interface.

    virtual ~KIFACE() throw() {}

#define KFCTL_STANDALONE ( 1 << 0 )        ///< Running as a standalone Top.
#define KFCTL_CPP_PROJECT_SUITE ( 1 << 1 ) ///< Running under C++ project mgr, possibly with others.
#define KFCTL_CLI ( 1 << 2 )               ///< Running as CLI app


    // Called once after DSO is loaded for process level initialization
    virtual bool OnKifaceStart( PGM_BASE* aProgram, int aCtlBits, KIWAY* aKiway ) = 0;

    // Called once before DSO is unloaded
    virtual void OnKifaceEnd() = 0;

    // Reloads global state
    virtual void Reset() = 0;

    // Create a QWidget for the current project
    virtual QWidget* CreateKiWindow( QWidget* aParent, int aClassId,
                                     KIWAY* aKIWAY, int aCtlBits = 0 ) = 0;

    // Saving a file under a different name delegated to KIFACEs
    virtual void SaveFileAs( const QString& srcProjectBasePath,
                             const QString& srcProjectName,
                             const QString& newProjectBasePath,
                             const QString& newProjectName,
                             const QString& srcFilePath,
                             QString& aErrors )
    {
        // If a KIFACE owns files then it needs to implement this
    }

    // Return pointer to the requested object
    virtual void* IfaceOrAddress( int aDataId ) = 0;

    // Append this Kiface's registered actions to the given list
    virtual void GetActions( std::vector<TOOL_ACTION*>& aActions ) const = 0;

    virtual int HandleJob( JOB* aJob, REPORTER* aReporter, PROGRESS_REPORTER* aProgressReporter )
    {
        return 0;
    }

    virtual bool HandleJobConfig( JOB* aJob, QWidget* aParent )
    {
        return 0;
    }
};


// Minimalistic software bus for communications between DLLs/DSOs
// Makes it possible to send custom events between DSOs
// Each project has its own KIWAY with PROJECT data structure
class KICOMMON_API KIWAY : public QObject
{
    friend struct PGM_SINGLE_TOP;        // can use set_kiface()

public:
    // Known KIFACE implementations
    enum FACE_T
    {
        FACE_SCH,               // eeschema DSO
        FACE_PCB,               // pcbnew DSO
        FACE_CVPCB,
        FACE_GERBVIEW,
        FACE_PL_EDITOR,
        FACE_PCB_CALCULATOR,
        FACE_BMP2CMP,
        FACE_PYTHON,

        KIWAY_FACE_COUNT
    };

    ~KIWAY() throw () {}

    // Simple mapping function returning FACE_T for aFrameType
    static FACE_T KifaceType( FRAME_T aFrameType );

    // If you change the vtable, recompile all of KiCad

    // Return the KIFACE* given a FACE_T
    virtual KIFACE* KiFACE( FACE_T aFaceId, bool doLoad = true );

    // Return the KIWAY_PLAYER* given a FRAME_T
    virtual KIWAY_PLAYER* Player( FRAME_T aFrameType, bool doCreate = true,
                                  QMainWindow* aParent = nullptr );

    // Call the KIWAY_PLAYER::Close function on the window
    virtual bool PlayerClose( FRAME_T aFrameType, bool doForce );

    // Call the KIWAY_PLAYER::Close function on all windows
    virtual bool PlayersClose( bool doForce );

    // Notifies a Kiway that a player has been closed
    void PlayerDidClose( FRAME_T aFrameType );

    // Send payload to destination from source
    virtual void ExpressMail( FRAME_T aDestination, MAIL_T aCommand, std::string& aPayload,
                              QWidget* aSource = nullptr );

    // Append all registered actions to the given list
    virtual void GetActions( std::vector<TOOL_ACTION*>& aActions ) const;

    // Return the PROJECT associated with this KIWAY
    virtual PROJECT& Prj() const;

    // Change the language and call ShowChangedLanguage on all KIWAY_PLAYERs
    virtual void SetLanguage( int aLanguage );

    // Call CommonSettingsChanged on all KIWAY_PLAYERs
    virtual void CommonSettingsChanged( int aFlags = 0 );

    // Call ProjectChanged on all KIWAY_PLAYERs
    virtual void ProjectChanged();

    KIWAY( int aCtlBits, QMainWindow* aTop = nullptr );

    // Overwrite ctl bits for switching between standalone and manager mode
    void SetCtlBits( int aCtlBits ) { m_ctl = aCtlBits; }

    // Tell this KIWAY about the top most frame in the program
    void SetTop( QMainWindow* aTop );
    QMainWindow* GetTop() { return m_top; }

    void OnKiCadExit();

    void OnKiwayEnd();

    bool ProcessEvent( QEvent& aEvent );

    int  ProcessJob( KIWAY::FACE_T aFace, JOB* aJob, REPORTER* aReporter = nullptr,
                     PROGRESS_REPORTER* aProgressReporter = nullptr );
    bool ProcessJobConfigDialog( KIWAY::FACE_T aFace, JOB* aJob, QWidget* aWindow );

    // Get the window pointer to the blocking dialog
    QWidget* GetBlockingDialog();
    void SetBlockingDialog( QWidget* aWin );

private:
    // Get the [path &] name of the DSO holding the requested FACE_T
    const QString dso_search_path( FACE_T aFaceId );

    bool set_kiface( FACE_T aFaceType, KIFACE* aKiface )
    {
        if( (unsigned) aFaceType < (unsigned) KIWAY_FACE_COUNT )
        {
            m_kiface[aFaceType] = aKiface;
            return true;
        }

        return false;
    }

    // Return reference of KIWAY_PLAYER having the type aFrameType if exists
    KIWAY_PLAYER* GetPlayerFrame( FRAME_T aFrameType );

    static KIFACE*  m_kiface[KIWAY_FACE_COUNT];
    static int      m_kiface_version[KIWAY_FACE_COUNT];

    int             m_ctl;

    QMainWindow*    m_top;      // Usually m_top is the Project manager

    int             m_blockingDialog;

    // Array to store window ID of PLAYER frames which were run
    // Empty entries are represented by -1
    // Call QWidget::find to check if still exists
    std::atomic<int> m_playerFrameId[KIWAY_PLAYER_COUNT];
};


#ifndef SWIG
// provided by single_top.cpp and kicad.cpp;
extern KIWAY Kiway;
// whereas python launchers: single_top.py and project manager instantiate as a python object
#endif


// Point to the one and only KIFACE export
// Export address looked up via symbolic string, should be extern "C"
// Function called only once, DSO may support multiple Top windows
typedef KIFACE* KIFACE_GETTER_FUNC( int* aKIFACEversion, int aKIWAYversion, PGM_BASE* aProgram );


#ifndef SWIG

// No name mangling. Each KIFACE (DSO/DLL) will implement this once
extern "C" {
#if defined(BUILD_KIWAY_DLL)
    KIFACE_API KIFACE* KIFACE_GETTER(  int* aKIFACEversion, int aKIWAYversion, PGM_BASE* aProgram );
#else
    KIFACE* KIFACE_GETTER(  int* aKIFACEversion, int aKIWAYversion, PGM_BASE* aProgram );
#endif
}

#endif  // SWIG

#endif  // KIWAY_H_
