// Date for kicad build version

#ifndef KICAD_BUILD_VERSION_H
#define KICAD_BUILD_VERSION_H

#include <kicommon.h>
#include <tuple>

class QString;

KICOMMON_API QString GetBuildVersion();

KICOMMON_API QString GetBaseVersion();

KICOMMON_API QString GetPlatformGetBitnessName();

KICOMMON_API QString GetSemanticVersion();

KICOMMON_API QString GetMajorMinorVersion();

KICOMMON_API QString GetMajorMinorPatchVersion();

KICOMMON_API QString GetBuildDate();

KICOMMON_API QString GetCommitHash();

KICOMMON_API const std::tuple<int, int, int>& GetMajorMinorPatchTuple();

KICOMMON_API bool IsNightlyVersion();

KICOMMON_API void SetOpenGLInfo( const char* aRenderer, const char* aVendor, const char* aVersion );

KICOMMON_API QString GetVersionInfoData( const QString& aTitle, bool aHtml = false,
                                          bool aBrief = false );

#endif  // KICAD_BUILD_VERSION_H