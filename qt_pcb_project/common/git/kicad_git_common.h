#ifndef _GIT_COMMON_H_
#define _GIT_COMMON_H_

#include <git/kicad_git_errors.h>

#include <git2.h>
#include <mutex>
#include <set>
#include <QString>
#include <vector>
#include <string>

class KIGIT_COMMON
{

public:
    KIGIT_COMMON( git_repository* aRepo );
    KIGIT_COMMON( const KIGIT_COMMON& aOther );
    ~KIGIT_COMMON();

    git_repository* GetRepo() const;

    void SetRepo( git_repository* aRepo )
    {
        m_repo = aRepo;
    }

    QString GetCurrentBranchName() const;

    std::vector<std::string> GetBranchNames() const;

    /**
     * Return a vector of project files in the repository.  Sorted by the depth of
     * the project file in the directory tree
     *
     * @return std::vector<std::string> of project files
     */
    std::vector<std::string> GetProjectDirs();

    /**
     * Return a pair of sets of files that differ locally from the remote repository
     * The first set is files that have been committed locally but not pushed
     * The second set is files that have been committed remotely but not pulled
     */
    std::pair<std::set<QString>,std::set<QString>> GetDifferentFiles() const;

    enum class GIT_STATUS
    {
        GIT_STATUS_UNTRACKED,
        GIT_STATUS_CURRENT,
        GIT_STATUS_MODIFIED,        // File changed but not committed to local repository
        GIT_STATUS_ADDED,
        GIT_STATUS_DELETED,
        GIT_STATUS_BEHIND,          // File changed in remote repository but not in local
        GIT_STATUS_AHEAD,           // File changed in local repository but not in remote
        GIT_STATUS_CONFLICTED,
        GIT_STATUS_IGNORED,
        GIT_STATUS_LAST
    };

    enum class GIT_CONN_TYPE
    {
        GIT_CONN_HTTPS = 0,
        GIT_CONN_SSH,
        GIT_CONN_LOCAL,
        GIT_CONN_LAST
    };

    QString GetUsername() const { return m_username; }
    QString GetPassword() const { return m_password; }
    GIT_CONN_TYPE GetConnType() const;

    void SetUsername( const QString& aUsername ) { m_username = aUsername; }
    void SetPassword( const QString& aPassword ) { m_password = aPassword; }
    void SetSSHKey( const QString& aSSHKey );

    // Holds a temporary variable that can be used by the authentication callback
    // to remember which types of authentication have been tested so that we
    // don't loop forever.
    unsigned& TestedTypes() { return m_testedTypes; }

    // Returns true if the repository has local commits that have not been pushed
    bool HasLocalCommits() const;

    // Returns true if the repository has a remote that can be pushed to pulled from
    bool HasPushAndPullRemote() const;

    // Updates the password and remote information for the repository given the current branch
    void UpdateCurrentBranchInfo();

    QString GetGitRootDirectory() const;

    QString GetRemotename() const;

    void ResetNextKey() { m_nextPublicKey = 0; }

    QString GetNextPublicKey()
    {
        if( m_nextPublicKey >= static_cast<int>( m_publicKeys.size() ) )
            return QString();

        return QString::fromStdString(m_publicKeys[m_nextPublicKey++]);
    }

    void SetRemote( const QString& aRemote )
    {
        m_remote = aRemote;
        updateConnectionType();
    }

    int HandleSSHKeyAuthentication( git_cred** aOut, const QString& aUsername );

    int HandlePlaintextAuthentication( git_cred** aOut, const QString& aUsername );

    int HandleSSHAgentAuthentication( git_cred** aOut, const QString& aUsername );

    static QString GetLastGitError()
    {
        const git_error* error = git_error_last();

        if( error == nullptr )
            return QString( "No error" );

        return QString( error->message );
    }

protected:
    git_repository* m_repo;

    GIT_CONN_TYPE m_connType;
    QString m_remote;      // This is the full connection string
    QString m_hostname;    // This is just the hostname without the protocol, username, or password
    QString m_username;
    QString m_password;

    unsigned m_testedTypes;

    std::mutex m_gitActionMutex;

    // Make git handlers friends so they can access the mutex
    friend class GIT_PUSH_HANDLER;
    friend class GIT_PULL_HANDLER;
    friend class GIT_CLONE_HANDLER;

private:
    void updatePublicKeys();
    void updateConnectionType();

    std::vector<std::string> m_publicKeys;
    int m_nextPublicKey;

    // Create a dummy flag to tell if we have tested ssh agent credentials separately
    // from the ssh key credentials
    static const unsigned KIGIT_CREDENTIAL_SSH_AGENT = 1 << sizeof( m_testedTypes - 1 );
};

extern "C" int  progress_cb( const char* str, int len, void* data );
extern "C" void clone_progress_cb( const char* str, size_t len, size_t total, void* data );
extern "C" int transfer_progress_cb( const git_transfer_progress* aStats, void* aPayload );
extern "C" int update_cb( const char* aRefname, const git_oid* aFirst, const git_oid* aSecond,
                          void* aPayload );
extern "C" int push_transfer_progress_cb( unsigned int aCurrent, unsigned int aTotal,
                                          size_t aBytes, void* aPayload );
extern "C" int push_update_reference_cb( const char* aRefname, const char* aStatus,
                                         void* aPayload );

extern "C" int fetchhead_foreach_cb( const char*, const char*,
                                     const git_oid* aOID, unsigned int aIsMerge, void* aPayload );
extern "C" int credentials_cb( git_cred** aOut, const char* aUrl, const char* aUsername,
                               unsigned int aAllowedTypes, void* aPayload );

#endif // _GIT_COMMON_H_