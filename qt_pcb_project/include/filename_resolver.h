// QT_TRANSFORMATION_COMPLETED

#ifndef FILENAME_RESOLVER_H
#define FILENAME_RESOLVER_H

#include <list>
#include <map>
#include <vector>
#include <QString>

class PROJECT;
class PGM_BASE;
class EMBEDDED_FILES;

struct SEARCH_PATH
{
    QString m_Alias;           // Alias to the base path.
    QString m_Pathvar;         // Base path as stored in the configuration file.
    QString m_Pathexp;         // Expanded base path.
    QString m_Description;     // Description of the aliased path.
};


class FILENAME_RESOLVER
{
public:
    FILENAME_RESOLVER();

    bool Set3DConfigDir( const QString& aConfigDir );

    bool SetProject( const PROJECT* aProject, bool* flgChanged = nullptr );

    QString GetProjectDir() const;

    void SetProgramBase( PGM_BASE* aBase );

    bool UpdatePathList( const std::vector<SEARCH_PATH>& aPathList );

    QString ResolvePath( const QString& aFileName, const QString& aWorkingPath,
                          std::vector<const EMBEDDED_FILES*> aEmbeddedFilesStack );

    QString ShortenPath( const QString& aFullPathName );

    const std::list<SEARCH_PATH>* GetPaths() const;

    bool SplitAlias( const QString& aFileName, QString& anAlias, QString& aRelPath ) const;

    bool ValidateFileName( const QString& aFileName, bool& hasAlias ) const;

    bool GetKicadPaths( std::list< QString >& paths ) const;

private:
    bool createPathList( void );

    bool addPath( const SEARCH_PATH& aPath );

    void checkEnvVarPath( const QString& aPath );

    QString                m_configDir;     // 3D configuration directory.
    std::list<SEARCH_PATH> m_paths;         // List of base paths to search from.
    int                    m_errflags;
    PGM_BASE*              m_pgm;
    const PROJECT*         m_project;
    QString                m_curProjDir;
};

#endif  // FILENAME_RESOLVER_H
