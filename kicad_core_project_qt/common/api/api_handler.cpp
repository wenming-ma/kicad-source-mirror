#include <api/api_handler.h>
#include <QString>
#include <QObject>

using kiapi::common::ApiRequest, kiapi::common::ApiResponse, kiapi::common::ApiResponseStatus;


const QString API_HANDLER::m_defaultCommitMessage = QObject::tr( "Modification from API" );


API_RESULT API_HANDLER::Handle( ApiRequest& aMsg )
{
    ApiResponseStatus status;

    if( !aMsg.has_message() )
    {
        status.set_status( ApiStatusCode::AS_BAD_REQUEST );
        status.set_error_message( "request has no inner message" );
        return tl::unexpected( status );
    }

    std::string typeName;

    if( !google::protobuf::Any::ParseAnyTypeUrl( aMsg.message().type_url(), &typeName ) )
    {
        status.set_status( ApiStatusCode::AS_BAD_REQUEST );
        status.set_error_message( "could not parse inner message type" );
        return tl::unexpected( status );
    }

    auto it = m_handlers.find( typeName );

    if( it != m_handlers.end() )
    {
        REQUEST_HANDLER& handler = it->second;
        return handler( aMsg );
    }

    status.set_status( ApiStatusCode::AS_UNHANDLED );
    return tl::unexpected( status );
}
