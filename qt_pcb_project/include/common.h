
#ifndef INCLUDE__COMMON_H_
#define INCLUDE__COMMON_H_

#include <kicommon.h>
#include <functional>
#include <memory>

#include <QtCore/QSettings>
#include <QtCore/QString>
#include <QtCore/QProcess>

class PROJECT;
class SEARCH_STACK;
class REPORTER;

KICOMMON_API QString SearchHelpFileFullPath( const QString& aBaseName );

KICOMMON_API bool EnsureFileDirectoryExists( QString*        aTargetFullFileName,
                                             const QString&  aBaseFilename,
                                             REPORTER*       aReporter = nullptr );

KICOMMON_API QString EnsureFileExtension( const QString& aFilename, const QString& aExtension );

KICOMMON_API const QString ExpandEnvVarSubstitutions( const QString& aString,
                                                     const PROJECT* aProject );

#define FOR_ERC_DRC 1

KICOMMON_API QString ExpandTextVars( const QString& aSource,
                                     const std::function<bool( QString* )>* aResolver,
                                     int aFlags = 0 );

KICOMMON_API QString ExpandTextVars( const QString& aSource, const PROJECT* aProject,
                                     int aFlags = 0 );

KICOMMON_API QString GetGeneratedFieldDisplayName( const QString& aSource );

KICOMMON_API bool IsGeneratedField( const QString& aSource );

KICOMMON_API const QString ResolveUriByEnvVars( const QString& aUri, const PROJECT* aProject );


KICOMMON_API long long TimestampDir( const QString& aDirPath, const QString& aFilespec );


KICOMMON_API bool WarnUserIfOperatingSystemUnsupported();


#endif  // INCLUDE__COMMON_H_
