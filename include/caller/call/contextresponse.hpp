#ifndef CALLER_CONTEXTRESPONSE_HPP
#define CALLER_CONTEXTRESPONSE_HPP
#include <caller/call/declare.hpp>

CALLER_BEGIN

class RequestResponse
{
public:
    static RequestResponsePtr make(RequestPtr req, ResponsePtr resp);
public:
    RequestResponse(RequestPtr req = nullptr, ResponsePtr resp = nullptr);
    ~RequestResponse();
public:
    RequestPtr request() const;
    void setRequest(const RequestPtr &request);

    ResponsePtr response() const;
    void setResponse(const ResponsePtr &response);

private:
    RequestPtr    _M_Request;
    ResponsePtr   _M_Response;
};

CALLER_END

#endif // CONTEXTRESPONSE_HPP
