#include <caller/call/contextresponse.hpp>

CALLER_BEGIN

RequestResponsePtr RequestResponse::make(RequestPtr req, ResponsePtr resp)
{
    return std::make_shared<RequestResponse>(req, resp);
}

RequestResponse::RequestResponse(RequestPtr req, ResponsePtr resp)
    : _M_Request(req), _M_Response(resp)
{

}

RequestResponse::~RequestResponse()
{

}

RequestPtr RequestResponse::request() const
{
    return _M_Request;
}

void RequestResponse::setRequest(const RequestPtr &context)
{
    _M_Request = context;
}

ResponsePtr RequestResponse::response() const
{
    return _M_Response;
}

void RequestResponse::setResponse(const ResponsePtr &response)
{
    _M_Response = response;
}

CALLER_END
