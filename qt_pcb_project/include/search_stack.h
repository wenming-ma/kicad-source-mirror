// QT_TRANSFORMATION_COMPLETED

#ifndef SEARCH_STACK_H_
#define SEARCH_STACK_H_

#include <kicommon.h>
#include <QString>
#include <QStringList>
#include <QDir>
#include <project.h>


class KICOMMON_API SEARCH_STACK : public PROJECT::_ELEM
{
public:
    PROJECT::ELEM ProjectElementType() override { return PROJECT::ELEM::SEARCH_STACK; }

#if defined(DEBUG)
    void Show( const QString& aPrefix ) const;
#endif

    QString FilenameWithRelativePathInSearchList( const QString& aFullFilename,
                                                   const QString& aBaseDir );

    void AddPaths( const QString& aPaths, int aIndex = -1 );

    void RemovePaths( const QString& aPaths );

    static int Split( QStringList* aResult, const QString& aPathString );

#if 1   // this function is so poorly designed it deserves not to exist.
    const QString LastVisitedPath( const QString& aSubPathToSearch = QString() );
#endif

    // Basic path list operations to replace wxPathList functionality
    void Add( const QString& path );
    void AddEnvList( const QString& envVariable );
    void Clear();
    QString FindValidPath( const QString& file ) const;
    QString FindAbsoluteValidPath( const QString& file ) const;
    int GetCount() const;
    QString Item( int index ) const;

private:
    QStringList m_paths;

};

#endif  // SEARCH_STACK_H_
