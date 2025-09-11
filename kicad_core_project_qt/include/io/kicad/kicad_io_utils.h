
#pragma once

#include <QString>
#include <QIODevice>

#include <kicommon.h>

class OUTPUTFORMATTER;
class KIID;

namespace KICAD_FORMAT {

/**
 * Writes a boolean to the formatter, in the style (aKey [yes|no])
 *
 * @param aOut is the output formatter to write to
 * @param aKey is the name of the boolean flag
 * @param aValue is the value to write
 */
KICOMMON_API void FormatBool( OUTPUTFORMATTER* aOut, const QString& aKey, bool aValue );

KICOMMON_API void FormatUuid( OUTPUTFORMATTER* aOut, const KIID& aUuid );

/**
 * Write binary data to the formatter as base 64 encoded string.
 */
KICOMMON_API void FormatStreamData( OUTPUTFORMATTER& aOut, QIODevice& aStream );

KICOMMON_API void Prettify( std::string& aSource, bool aCompactSave );

} // namespace KICAD_FORMAT
