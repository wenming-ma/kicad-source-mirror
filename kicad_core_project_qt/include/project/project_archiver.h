
#ifndef KICAD_PROJECT_ARCHIVER_H
#define KICAD_PROJECT_ARCHIVER_H

#include <QString>
#include <kicommon.h>


class PROJECT;
class REPORTER;
class SETTINGS_MANAGER;


class KICOMMON_API PROJECT_ARCHIVER
{
public:
    PROJECT_ARCHIVER();

    ~PROJECT_ARCHIVER() = default;

    static bool AreZipArchivesIdentical( const QString& aZipFileA, const QString& aZipFileB,
                                         REPORTER& aReporter );

    static bool Archive( const QString& aSrcDir, const QString& aDestFile, REPORTER& aReporter,
                         bool aVerbose = true, bool aIncludeExtraFiles = false );

    static bool Unarchive( const QString& aSrcFile, const QString& aDestDir,
                           REPORTER& aReporter );
};

#endif // KICAD_PROJECT_ARCHIVER_H
