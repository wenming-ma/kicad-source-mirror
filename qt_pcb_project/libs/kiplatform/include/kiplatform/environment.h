#include <QString>

class QWidget;

namespace KIPLATFORM
{
    namespace ENV
    {
        /**
         * Perform environment initialization tasks. These tasks are called during the QApp
         * constructor and therefore won't have access to the underlying OS application.
         */
        void Init();

        /**
         * Move the specified file/directory to the trash bin/recycle bin.
         *
         * @param aPath is the absolute path of the file/directory to move to the trash
         * @param aError is the error message saying why the operation failed
         *
         * @return true if the operation succeeds, false if it fails (see the contents of aError)
         */
        bool MoveToTrash( const QString& aPath, QString& aError );

        /**
         * Determines if a given path is a network shared file apth
         * On Windows for example, any form of path is accepted drive map or UNC
         *
         * @param aPath is any kind of file path to be tested
         *
         * @return true if given path is on a network location
         */
        bool IsNetworkPath( const QString& aPath );


        /**
         * Retrieves the operating system specific path for a user's documents
         *
         * @return User documents path
         */
        QString GetDocumentsPath();

        /**
         * Retrieves the operating system specific path for a user's configuration store
         *
         * @return User config path
         */
        QString GetUserConfigPath();

        /**
         * Retrieves the operating system specific path for a user's data store
         *
         * @return User config path
         */
        QString GetUserDataPath();

        /**
         * Retrieves the operating system specific path for a user's local data store
         *
         * @return User config path
         */
        QString GetUserLocalDataPath();

        /**
         * Retrieves the operating system specific path for user's application cache
         *
         * @return User cache path
         */
        QString GetUserCachePath();

        struct PROXY_CONFIG
        {
            QString host;
            QString username;
            QString password;
        };

        /**
         * Retrieves platform level proxying requirements to reach the given url
         *
         * @param aURL The target url we will be requesting over http
         * @param aCfg The proxy config struct that will be populated
         *
         * @return True if successful fetched proxy info
         */
        bool GetSystemProxyConfig( const QString& aURL, PROXY_CONFIG& aCfg );

        /**
         * Validates the code signing signature of a given file
         * This is most likely only ever going to be applicable to Windows
         *
         * @return True if file signature passes
         */
        bool VerifyFileSignature( const QString& aPath );

        /**
         * Retrieves the app user model id, a special string used for taskbar grouping
         * on Windows 7 and later
         *
         * @returns The app user model id on Windows, empty on all other platforms
         */
        QString GetAppUserModelId();

        /**
         * Sets the relaunch command for taskbar pins, this is intended for Windows
         */
        void SetAppDetailsForWindow( QWidget* aWindow, const QString& aRelaunchCommand, const QString& aRelaunchDisplayName );

        /**
         * @returns the command line string used to launch the process
         */
        QString GetCommandLineStr();

        void AddToRecentDocs( const QString &aPath );
    }
}
