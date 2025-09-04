
#ifndef JSON_SCHEMA_VALIDATOR_H
#define JSON_SCHEMA_VALIDATOR_H

#include <QFileInfo>
#include <kicommon.h>
#include <json_common.h>
#include <nlohmann/json-schema.hpp>

class KICOMMON_API JSON_SCHEMA_VALIDATOR
{
public:
    JSON_SCHEMA_VALIDATOR( const QFileInfo& aSchemaFile );

    ~JSON_SCHEMA_VALIDATOR() = default;

    nlohmann::json Validate( const nlohmann::json& aJson,
                             nlohmann::json_schema::error_handler& aErrorHandler,
                             const nlohmann::json_uri& aInitialUri = nlohmann::json_uri("#") ) const;

private:
    nlohmann::json_schema::json_validator m_validator;
};

#endif //JSON_SCHEMA_VALIDATOR_H
