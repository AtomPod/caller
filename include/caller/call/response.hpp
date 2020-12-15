#ifndef CALLER_CALL_RESPONSE_HPP
#define CALLER_CALL_RESPONSE_HPP
#include <caller/core.hpp>
#include <caller/route/route.hpp>
#include <caller/call/declare.hpp>
#include <caller/message/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/common/any.hpp>
#include <caller/common/id.hpp>
#include <caller/common/error.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Response
{
public:
    enum StatusCode {
        NONE = 0,
        OK,
        CANCELED,
        TIMEOUT,
        SERVICE_ERROR
    };
public:
    Response(size_t buffsize = 0);
    virtual ~Response();
public:
    static ResponsePtr make(size_t buffsize = 0);
public:
    ByteBuffer &dataBuffer();
    ByteBuffer dataBuffer() const;
    void setDataBuffer(const ByteBuffer &dataBuffer);

    MessagePtr  rawMessage() const;
    void        setRawMessage(const MessagePtr &rawMessage);

    int statusCode() const;
    void setStatusCode(int statusCode);

    std::string errorMessage() const;
    void setErrorMessage(const std::string &errorMessage);

    Route::ID id() const;
    void setID(const ID &id);

    Error error() const;
    void setError(const Error &error);
private:
    ByteBuffer      _M_DataBuffer;
    MessagePtr      _M_RawMessage;
    int             _M_StatusCode;
    std::string     _M_ErrorMessage;
    Route::ID       _M_ID;
    Error           _M_Error;
};

CALLER_END

#endif // CALLER_CALL_RESPONSE_HPP
