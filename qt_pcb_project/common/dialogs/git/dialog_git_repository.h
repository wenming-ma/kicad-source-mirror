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
        m_ConnType->SetSelection( static_cast<int>( aType ) );
        updateAuthControls();
    }

    KIGIT_COMMON::GIT_CONN_TYPE GetRepoType() const
    {
        return static_cast<KIGIT_COMMON::GIT_CONN_TYPE>( m_ConnType->GetSelection() );
    }

    void    SetRepoName( const QString& aName ) { m_txtName->SetValue( aName ); }
    QString GetRepoName() const { return m_txtName->GetValue(); }

    void    SetRepoURL( const QString& aURL ) { m_txtURL->SetValue( aURL ); }
    QString GetRepoURL() const { return m_txtURL->GetValue(); }

    QString GetBareRepoURL() const
    {
        QString url = m_txtURL->GetValue();

        if( url.startsWith( "https://" ) )
            url = url.mid( 8 );
        else if( url.startsWith( "http://" ) )
            url = url.mid( 7 );
        else if( url.startsWith( "ssh://" ) )
            url = url.mid( 6 );

        return url;
    }

    const QString& GetFullURL() const { return m_fullURL; }

    void    SetUsername( const QString& aUsername ) { m_txtUsername->SetValue( aUsername ); }
    QString GetUsername() const { return m_txtUsername->GetValue(); }

    void    SetPassword( const QString& aPassword ) { m_txtPassword->SetValue( aPassword ); }
    QString GetPassword() const { return m_txtPassword->GetValue(); }

    void    SetRepoSSHPath( const QString& aPath ) { m_fpSSHKey->SetFileName( aPath ); m_prevFile = aPath; }
    QString GetRepoSSHPath() const { return m_fpSSHKey->GetFileName().GetFullPath(); }

    void    SetEncrypted( bool aEncrypted = true );

private:
    void OnUpdateUI( QEvent* event ) override;
    void OnLocationExit( QFocusEvent* event ) override;
    void OnOKClick( QEvent* event ) override;

    void OnSelectConnType( QEvent* event ) override;
    void OnTestClick( QEvent* event ) override;

    void OnFileUpdated( QEvent* event ) override;
    void onCbCustom( QEvent* event ) override;

    void setDefaultSSHKey();

    void updateAuthControls();
    void updateURLData();
    bool extractClipboardData();

    std::tuple<bool,QString,QString,QString> isValidHTTPS( const QString& url );
    std::tuple<bool,QString, QString> isValidSSH( const QString& url );

private:
    git_repository* m_repository;
    QString         m_fullURL;

    QString         m_prevFile;

    bool            m_tempRepo;
    QString         m_tempPath;
};

#endif /* DIALOG_GIT_REPOSITORY_H_ */