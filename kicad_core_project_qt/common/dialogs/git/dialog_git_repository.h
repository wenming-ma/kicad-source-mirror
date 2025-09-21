
#ifndef DIALOG_GIT_REPOSITORY_H_
#define DIALOG_GIT_REPOSITORY_H_

#include "dialog_git_repository_base.h"

#include <git/kicad_git_common.h>
#include <git2.h>

class DIALOG_GIT_REPOSITORY : public DIALOG_GIT_REPOSITORY_BASE
{
public:
    DIALOG_GIT_REPOSITORY( QWidget* aParent, git_repository* aRepository,
                           QString aURL = QString() );
    ~DIALOG_GIT_REPOSITORY() override;

    void SetRepoType( KIGIT_COMMON::GIT_CONN_TYPE aType )
    {
        m_ConnType->setCurrentIndex( static_cast<int>( aType ) );
        updateAuthControls();
    }

    KIGIT_COMMON::GIT_CONN_TYPE GetRepoType() const
    {
        return static_cast<KIGIT_COMMON::GIT_CONN_TYPE>( m_ConnType->currentIndex() );
    }

    void     SetRepoName( const QString& aName ) { m_txtName->setValue( aName ); }
    QString GetRepoName() const { return m_txtName->value(); }

    void     SetRepoURL( const QString& aURL ) { m_txtURL->setValue( aURL ); }
    QString GetRepoURL() const { return m_txtURL->value(); }

    /**
     * @brief Get the Bare Repo U R L object
     *
     * @return QString without the protocol
     */
    QString GetBareRepoURL() const
    {
        QString url = m_txtURL->value();

        if( url.startsWith( "https://" ) )
            url = url.mid( 8 );
        else if( url.startsWith( "http://" ) )
            url = url.mid( 7 );
        else if( url.startsWith( "ssh://" ) )
            url = url.mid( 6 );

        return url;
    }

    const QString& GetFullURL() const { return m_fullURL; }

    void     SetUsername( const QString& aUsername ) { m_txtUsername->setValue( aUsername ); }
    QString GetUsername() const { return m_txtUsername->value(); }

    void     SetPassword( const QString& aPassword ) { m_txtPassword->setValue( aPassword ); }
    QString GetPassword() const { return m_txtPassword->value(); }

    void     SetRepoSSHPath( const QString& aPath ) { m_fpSSHKey->setFileName( aPath ); m_prevFile = aPath; }
    QString GetRepoSSHPath() const { return m_fpSSHKey->fileName().absoluteFilePath(); }

    void     SetEncrypted( bool aEncrypted = true );

private:
    void OnUpdateUI();
    void OnLocationExit();
    void OnOKClick();

    void OnSelectConnType();
    void OnTestClick();

    void OnFileUpdated();
    void onCbCustom();

    void setDefaultSSHKey();

    void updateAuthControls();
    void updateURLData();
    bool extractClipboardData();

    std::tuple<bool,QString,QString,QString> isValidHTTPS( const QString& url );
    std::tuple<bool,QString, QString> isValidSSH( const QString& url );

private:
    git_repository* m_repository;
    QString        m_fullURL;

    QString        m_prevFile;

    bool            m_tempRepo;
    QString        m_tempPath;
};

#endif /* DIALOG_GIT_REPOSITORY_H_ */