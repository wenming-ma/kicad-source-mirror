
#ifndef INCLUDE__LOCK_FILE_H_
#define INCLUDE__LOCK_FILE_H_

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QDebug>
#include <QString>
#include <QTextStream>
#include <json_common.h>
#include <wildcards_and_files_ext.h>

#define LCK "KICAD_LOCKING"

class LOCKFILE
{
public:
    LOCKFILE( const QString &filename, bool aRemoveOnRelease = true ) :
            m_originalFile( filename ), m_fileCreated( false ), m_status( false ),
            m_removeOnRelease( aRemoveOnRelease ), m_errorMsg( "" )
    {
        if( filename.isEmpty() )
            return;

        qDebug() << "Trying to lock" << filename;
        QFileInfo fn( filename );
        QString lockName = FILEEXT::LockFilePrefix + fn.baseName();
        QString lockExt = fn.suffix() + '.' + FILEEXT::LockFileExtension;
        QString lockPath = fn.path() + "/" + lockName + "." + lockExt;

        QFileInfo dirInfo( fn.path() );
        if( !dirInfo.isWritable() )
        {
            qDebug() << "File is not writable:" << filename;
            m_status = true;
            m_removeOnRelease = false;
            return;
        }

        m_lockFilename = lockPath;

        QFile file;
        try
        {
            bool lock_success = false;
            bool rw_success = false;

            {
                file.setFileName( m_lockFilename );
                lock_success = file.open( QIODevice::WriteOnly | QIODevice::NewOnly );

                if( !lock_success )
                {
                    file.setFileName( m_lockFilename );
                    rw_success = file.open( QIODevice::ReadOnly );
                }
            }

            if( lock_success )
            {
                // Lock file doesn't exist, create one
                m_fileCreated = true;
                m_status = true;
                m_username = QString::fromLocal8Bit( qgetenv("USER") );
                if( m_username.isEmpty() )
                    m_username = QString::fromLocal8Bit( qgetenv("USERNAME") );
                m_hostname = QString::fromLocal8Bit( qgetenv("COMPUTERNAME") );
                if( m_hostname.isEmpty() )
                    m_hostname = QString::fromLocal8Bit( qgetenv("HOSTNAME") );
                nlohmann::json j;
                j["username"] = m_username.toStdString();
                j["hostname"] = m_hostname.toStdString();
                std::string lock_info = j.dump();
                file.write( lock_info.c_str() );
                file.close();
                qDebug() << "Locked" << filename;
            }
            else if( rw_success )
            {
                // Lock file already exists, read the details
                QByteArray lockData = file.readAll();
                QString lock_info = QString::fromUtf8( lockData );
                nlohmann::json j = nlohmann::json::parse( lock_info.toStdString() );
                m_username = QString::fromStdString( j["username"].get<std::string>() );
                m_hostname = QString::fromStdString( j["hostname"].get<std::string>() );
                file.close();
                m_errorMsg = "Lock file already exists";
                qDebug() << "Existing Lock for" << filename;
            }
            else
            {
                throw std::runtime_error( "Failed to open lock file" );
            }
        }
        catch( std::exception& e )
        {
            qDebug() << "Got an error trying to lock" << filename << ":" << e.what();

            // Delete lock file if it was created above but we threw an exception somehow
            if( m_fileCreated )
            {
                QFile::remove( m_lockFilename );
                m_fileCreated = false;
            }

            m_errorMsg = "Failed to access lock file";
            m_status = false;
        }
    }

    ~LOCKFILE()
    {
        UnlockFile();
    }

    void UnlockFile()
    {
        qDebug() << "Unlocking" << m_lockFilename;

        // Delete lock file only if the file was created in the constructor and if the file
        // contains the correct user and host names.
        if( m_fileCreated && checkUserAndHost() )
        {
            if( m_removeOnRelease )
                QFile::remove( m_lockFilename );

            m_fileCreated = false;
            m_status = false;
            m_errorMsg = QString();
        }
    }

    bool OverrideLock( bool aRemoveOnRelease = true )
    {
        qDebug() << "Overriding lock on" << m_lockFilename;

        if( !m_fileCreated )
        {
            try
            {
                QFile file;
                bool success = false;

                {
                    file.setFileName( m_lockFilename );
                    success = file.open( QIODevice::WriteOnly );
                }

                if( success )
                {
                    m_username = QString::fromLocal8Bit( qgetenv("USER") );
                    if( m_username.isEmpty() )
                        m_username = QString::fromLocal8Bit( qgetenv("USERNAME") );
                    m_hostname = QString::fromLocal8Bit( qgetenv("COMPUTERNAME") );
                    if( m_hostname.isEmpty() )
                        m_hostname = QString::fromLocal8Bit( qgetenv("HOSTNAME") );
                    nlohmann::json j;
                    j["username"] = m_username.toStdString();
                    j["hostname"] = m_hostname.toStdString();
                    std::string lock_info = j.dump();
                    file.write( lock_info.c_str() );
                    file.close();
                    m_fileCreated = true;
                    m_status = true;
                    m_removeOnRelease = aRemoveOnRelease;
                    m_errorMsg = QString();
                    qDebug() << "Successfully overrode lock on" << m_lockFilename;
                    return true;
                }

                return false;
            }
            catch( std::exception& e )
            {
                qDebug() << "Got exception trying to override lock on" << m_lockFilename << ":" << e.what();

                return false;
            }
        }
        else
        {
            qDebug() << "Upgraded lock on" << m_lockFilename << "to delete on release";
            m_removeOnRelease = aRemoveOnRelease;
        }

        return true;
    }

    bool IsLockedByMe()
    {
        QString currentUser = QString::fromLocal8Bit( qgetenv("USER") );
        if( currentUser.isEmpty() )
            currentUser = QString::fromLocal8Bit( qgetenv("USERNAME") );
        QString currentHost = QString::fromLocal8Bit( qgetenv("COMPUTERNAME") );
        if( currentHost.isEmpty() )
            currentHost = QString::fromLocal8Bit( qgetenv("HOSTNAME") );
        return m_username == currentUser && m_hostname == currentHost;
    }

    QString GetUsername(){ return m_username; }

    QString GetHostname(){ return m_hostname; }

    QString GetErrorMsg(){ return m_errorMsg; }

    bool Locked() const
    {
        return m_fileCreated;
    }

    bool Valid() const
    {
        return m_status;
    }

    explicit operator bool() const
    {
        return m_status;
    }

private:
    QString m_originalFile;
    QString m_lockFilename;
    QString m_username;
    QString m_hostname;
    bool m_fileCreated;
    bool m_status;
    bool m_removeOnRelease;
    QString m_errorMsg;

    bool checkUserAndHost()
    {
        QFileInfo fileName( m_lockFilename );

        if( !fileName.exists() )
        {
            qDebug() << "File does not exist:" << m_lockFilename;
            return false;
        }

        QFile file;

        try
        {
            file.setFileName( m_lockFilename );
            if( file.open( QIODevice::ReadOnly ) )
            {
                QByteArray lockData = file.readAll();
                QString lock_info = QString::fromUtf8( lockData );
                nlohmann::json j = nlohmann::json::parse( lock_info.toStdString() );

                if( m_username == QString::fromStdString( j["username"].get<std::string>() )
                        && m_hostname == QString::fromStdString( j["hostname"].get<std::string>() ) )
                {
                    qDebug() << "User and host match for lock" << m_lockFilename;
                    return true;
                }
            }
        }
        catch( std::exception &e )
        {
            qDebug() << "Got exception trying to check user/host for lock on" << m_lockFilename << ":" << e.what();
        }

        qDebug() << "User and host DID NOT match for lock" << m_lockFilename;
        return false;
    }
};


#endif  // INCLUDE__LOCK_FILE_H_
