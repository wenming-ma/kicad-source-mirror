// QT_TRANSFORMATION_COMPLETED

#ifndef GESTFICH_H
#define GESTFICH_H

#include <kicommon.h>
#include <QString>
#include <QProcess>
#include <QDir>
#include <QFileInfo>
#include <QIODevice>
#include <vector>


#define UNIX_STRING_DIR_SEP "/"
#define WIN_STRING_DIR_SEP  "\\"

/* Forward class declarations. */
class EDA_LIST_DIALOG;


KICOMMON_API bool OpenPDF( const QString& file );

KICOMMON_API void KiCopyFile( const QString& aSrcPath, const QString& aDestPath,
                              QString& aErrors );

KICOMMON_API int ExecuteFile( const QString& aEditorName,
                              const QString& aFileName = QString(),
                              QProcess* aCallback = nullptr, bool aFileForKicad = true );

KICOMMON_API void QuoteString( QString& string );

KICOMMON_API QString FindKicadFile( const QString& shortname );

KICOMMON_API extern QString QuoteFullPath( QFileInfo& fn );


KICOMMON_API bool RmDirRecursive( const QString& aDirName, QString* aErrors = nullptr );

KICOMMON_API bool CopyDirectory( const QString& aSourceDir, const QString& aDestDir,
                                 QString& aErrors );

KICOMMON_API bool CopyFilesOrDirectory( const QString& aSourceDir, const QString& aDestDir,
                                        QString& aErrors, int& fileCopiedCount,
                                        const std::vector<QString>& aExclusions );

KICOMMON_API bool AddDirectoryToZip( QIODevice& aZip,
                                     const QString& aSourceDir,
                                     QString& aErrors,
                                     const QString& aParentDir = QString() );

#endif /* GESTFICH_H */
