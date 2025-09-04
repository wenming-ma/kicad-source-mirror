
#ifndef KICAD_JSON_CONVERSIONS_H
#define KICAD_JSON_CONVERSIONS_H

#include <kicommon.h>
#include <nlohmann/json_fwd.hpp>
#include <QString>

// Specializations to allow directly reading/writing QStrings from JSON

KICOMMON_API void to_json( nlohmann::json& aJson, const QString& aString );

KICOMMON_API void from_json( const nlohmann::json& aJson, QString& aString );

#endif //KICAD_JSON_CONVERSIONS_H
