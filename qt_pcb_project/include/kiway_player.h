
#ifndef KIWAY_PLAYER_H_
#define KIWAY_PLAYER_H_

#include <QMainWindow>
#include <QTimer>
#include <QAction>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <vector>
#include <kiway_holder.h>
#include <eda_base_frame.h>

class KIWAY;
class PROJECT;
struct KIFACE;
class KIFACE_BASE;
class TOOL_MANAGER;
class KIWAY_EXPRESS;

class QEventLoop;
class QTcpServer;
class QTcpSocket;
class QSocketNotifier;


#ifdef SWIG
class KIWAY_PLAYER : public QMainWindow, public KIWAY_HOLDER
#else
class KIWAY_PLAYER : public EDA_BASE_FRAME
#endif
{
public:
    KIWAY_PLAYER( KIWAY* aKiway, QWidget* aParent, FRAME_T aFrameType,
                  const QString& aTitle, const QPoint& aPos, const QSize& aSize,
                  long aStyle, const QString& aFrameName, const EDA_IU_SCALE& aIuScale );

    ~KIWAY_PLAYER() throw();

    // For the aCtl argument of OpenProjectFiles()
#define KICTL_NONKICAD_ONLY     (1<<0)   ///< chosen file is non-KiCad according to user
#define KICTL_KICAD_ONLY        (1<<1)   ///< chosen file is from KiCad according to user
#define KICTL_CREATE            (1<<2)   ///< caller thinks requested project files may not exist.
#define KICTL_IMPORT_LIB        (1<<3)   ///< import all footprints into a project library.
#define KICTL_REVERT            (1<<4)   ///< reverting to a previously-saved (KiCad) file.

    virtual bool OpenProjectFiles( const std::vector<QString>& aFileList, int aCtl = 0 )
    {
        // overload me for your QMainWindow type.

        // Any overload should probably do this also:
        // Prj().MaybeLoadProjectSettings();

        // Then update the window title.

        return false;
    }


    virtual bool ShowModal( QString* aResult = nullptr,
                            QWidget* aResultantFocusWindow = nullptr );

    virtual void KiwayMailIn( KIWAY_EXPRESS& aEvent );

    bool Destroy();

    bool IsModal() const override       { return m_modal; }
    void SetModal( bool aIsModal )      { m_modal = aIsModal; }

    bool IsDismissed();

    void DismissModal( bool aRetVal, const QString& aResult = QString() );

    /* interprocess communication */
    void CreateServer( int service, bool local = true );
    void OnSockRequest();
    void OnSockRequestServer();

    virtual void ExecuteRemoteCommand( const char* cmdline ){}

protected:

    void kiway_express( KIWAY_EXPRESS& aEvent );

    void language_change( QAction* action );

    // variables for modal behavior support, only used by a few derivatives.
    bool            m_modal;        // true if frame is intended to be modal, not modeless

    /// Points to nested event_loop. NULL means not modal and dismissed.
    QEventLoop*     m_modal_loop;
    QWidget*        m_modal_resultant_parent; // the window caller in modal mode
    QString         m_modal_string;
    bool            m_modal_ret_val;    // true if a selection was made

    QTcpServer*             m_socketServer;
    std::vector<QTcpSocket*>  m_sockets;         // Interprocess communication.

};


// pseudo code for OpenProjectFiles
#if 0

bool OpenProjectFiles( const std::vector<QString>& aFileList, int aCtl = 0 )
{
    if( aFileList.size() != 1 )
    {
        complain via UI.
        return false
    }

    Q_ASSERT( aFileList[0] is absolute );      // bug in single_top.cpp or project manager.

    if( !Pgm().LockFile( fullFileName ) )
    {
        DisplayError( this, _( "This file is already open." ) );
        return false;
    }

    if current open project files have been modified
    {
        ask if user wants to save them and if yes save.
    }

    unload any currently open project files.

    Prj().SetProjectFullName( )

    if( aFileList[0] does not exist )
    {
        notify user file does not exist and ask if he wants to create it
        if( yes )
        {
            create empty project file(s)
            mark file as modified.

            use the default project config file.
        }
        else
            return false
    }
    else
    {
        load aFileList[0]

        use the project config file for project given by aFileList[0]s full path.
    }

    UpdateFileHistory( g_RootSheet->GetScreen()->GetFileName() );

    /* done in ReDraw typically:
    UpdateTitle();
    */

    show contents.
}

#endif

#endif // KIWAY_PLAYER_H_
