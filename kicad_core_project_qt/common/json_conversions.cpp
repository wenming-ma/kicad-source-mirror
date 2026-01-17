
#include <json_common.h>
#include <json_conversions.h>

// Specializations to allow directly reading/writing QStrings from JSON
void to_json( nlohmann::json& aJson, const QString& aString )
{
    aJson = aString.toStdString();
}


void from_json( const nlohmann::json& aJson, QString& aString )
{
    aString = QString::fromStdString( aJson.get<std::string>() );
}
