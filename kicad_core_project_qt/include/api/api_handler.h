#ifndef KICAD_API_HANDLER_H
#define KICAD_API_HANDLER_H

#include <functional>
#include <optional>

#include <fmt/format.h>
#include <tl/expected.hpp>

#include <QString>
#include <QObject>

// #include <google/protobuf/message.h>

#include <kicommon.h>
// #include <api/common/envelope.pb.h>
#include <core/typeinfo.h>

// using kiapi::common::ApiRequest, kiapi::common::ApiResponse;
// using kiapi::common::ApiResponseStatus, kiapi::common::ApiStatusCode;

// typedef tl::expected<ApiResponse, ApiResponseStatus> API_RESULT;

// template <typename T>
// using HANDLER_RESULT = tl::expected<T, ApiResponseStatus>;


// template <typename RequestMessageType>
// struct HANDLER_CONTEXT
// {
//     std::string ClientName;
//     RequestMessageType Request;
// };


class KICOMMON_API API_HANDLER
{
public:
    API_HANDLER() {}

    virtual ~API_HANDLER() {}

    /**
     * Attempt to handle the given API request, if a handler exists in this class for the message.
     * @param aMsg is a request to attempt to handle
     * @return a response to send to the client, or an appropriate error
     */
    // API_RESULT Handle( ApiRequest& aMsg );

protected:

    /**
     * A handler for outer messages (envelopes) that will unpack to inner messages and call a
     * specific handler function.  @see registerHandler.
     */
    // typedef std::function<HANDLER_RESULT<ApiResponse>( ApiRequest& )> REQUEST_HANDLER;

    /**
     * Registers an API command handler for the given message types.
     *
     * When an API request matching the given type comes in, the handler will be called and its
     * response will be packed into an envelope for sending back to the API client.
     *
     * If the given message does not unpack into the request type, an envelope is returned with
     * status AS_BAD_REQUEST, which probably indicates corruption in the message.
     *
     * @tparam RequestType is a protobuf message type containing a command
     * @tparam ResponseType is a protobuf message type containing a command response
     * @tparam HandlerType is the implied type of the API_HANDLER subclass
     * @param aHandler is the handler function for the given request and response types
     */
    /*
    template <class RequestType, class ResponseType, class HandlerType>
    void registerHandler( HANDLER_RESULT<ResponseType> ( HandlerType::*aHandler )(
            const HANDLER_CONTEXT<RequestType>& ) )
    {
        std::string typeName { RequestType().GetTypeName() };

        Q_ASSERT_X( !m_handlers.contains( typeName ), "registerHandler",
                     QString( "Duplicate API handler for type %1" ).arg( QString::fromStdString( typeName ) ).toLocal8Bit() );

        m_handlers[typeName] =
                [this, aHandler]( ApiRequest& aRequest ) -> API_RESULT
                {
                    HANDLER_CONTEXT<RequestType> ctx;
                    ApiResponse envelope;

                    if( !tryUnpack( aRequest, envelope, ctx.Request ) )
                        return envelope;

                    ctx.ClientName = aRequest.header().client_name();

                    HANDLER_RESULT<ResponseType> response =
                            std::invoke( aHandler, static_cast<HandlerType*>( this ), ctx );

                    if( response.has_value() )
                    {
                        envelope.mutable_status()->set_status( ApiStatusCode::AS_OK );
                        envelope.mutable_message()->PackFrom( *response );
                        return envelope;
                    }
                    else
                    {
                        return tl::unexpected( response.error() );
                    }
                };
    }
    */

    /// Maps type name (without the URL prefix) to a handler method
    // std::map<std::string, REQUEST_HANDLER> m_handlers;

    static const QString m_defaultCommitMessage;

private:

    /*
    template<typename MessageType>
    bool tryUnpack( ApiRequest& aRequest, ApiResponse& aReply, MessageType& aDest )
    {
        if( !aRequest.message().UnpackTo( &aDest ) )
        {
            std::string msg = fmt::format( "could not unpack message of type {} from request",
                                           aDest.GetTypeName() );
            aReply.mutable_status()->set_status( ApiStatusCode::AS_BAD_REQUEST );
            aReply.mutable_status()->set_error_message( msg );
            return false;
        }

        return true;
    }
    */
};

#endif //KICAD_API_HANDLER_H
