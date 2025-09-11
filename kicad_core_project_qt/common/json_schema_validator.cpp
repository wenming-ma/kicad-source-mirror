
#include <fstream>
#include <QFileInfo>
#include <QDebug>

#include <json_schema_validator.h>
#include <locale_io.h>


JSON_SCHEMA_VALIDATOR::JSON_SCHEMA_VALIDATOR( const QFileInfo& aSchemaFile )
{
    std::ifstream schema_stream( aSchemaFile.absoluteFilePath().toStdString() );
    nlohmann::json schema;

    try
    {
        // For some obscure reason on MINGW, using UCRT option,
        // m_schema_validator.set_root_schema() hangs without switching to locale "C"
#if defined(__MINGW32__) && defined(_UCRT)
        LOCALE_IO dummy;
#endif

        schema_stream >> schema;
        m_validator.set_root_schema( schema );
    }
    catch( std::exception& e )
    {
        if( !aSchemaFile.exists() )
        {
            qDebug() << QString( "schema file '%1' not found" ).arg( aSchemaFile.absoluteFilePath() );
        }
        else
        {
            qDebug() << QString( "Error loading schema: %1" ).arg( e.what() );
        }
    }
}


nlohmann::json JSON_SCHEMA_VALIDATOR::Validate( const nlohmann::json& aJson,
                             nlohmann::json_schema::error_handler& aErrorHandler,
                             const nlohmann::json_uri& aInitialUri ) const
{
    return m_validator.validate( aJson, aErrorHandler, aInitialUri );
}
