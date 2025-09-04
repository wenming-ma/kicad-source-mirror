

#ifndef ENV_PATHS_H
#define ENV_PATHS_H

#include <QString>
#include <settings/environment.h>

class QFileInfo;
class PROJECT;

QString NormalizePath( const QFileInfo& aFilePath, const ENV_VAR_MAP* aEnvVars,
                        const PROJECT* aProject );

QString NormalizePath( const QFileInfo& aFilePath, const ENV_VAR_MAP* aEnvVars,
                        const QString& aProjectPath );

QString ResolveFile( const QString& aFileName, const ENV_VAR_MAP* aEnvVars,
                      const PROJECT* aProject );

#endif /* ENV_PATHS_H */
