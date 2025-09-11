
#ifndef LAUNCH_EXT_H
#define LAUNCH_EXT_H

#include <kicommon.h>

class QString;

// Launches the given file or folder in the host OS
KICOMMON_API bool LaunchExternal( const QString& aPath );

#endif
