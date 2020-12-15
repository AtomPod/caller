#include <caller/call/response.hpp>
#include <memory>

CALLER_BEGIN

Response::Response(size_t buffsize) : _M_DataBuffer(buffsize),
    _M_RawMessage(nullptr), _M_Error(Error::Nil)
{

}

Response::~Response()
{

}

ResponsePtr Response::make(size_t buffsize)
{
    return ::std::make_shared<Response>(buffsize);
}

ByteBuffer &Response::dataBuffer()
{
    return _M_DataBuffer;
}

ByteBuffer Response::dataBuffer() const
{
    return _M_DataBuffer;
}

void Response::setDataBuffer(const ByteBuffer &dataBuffer)
{
    _M_DataBuffer = dataBuffer;
}

MessagePtr Response::rawMessage() const
{
    return _M_RawMessage;
}

void Response::setRawMessage(const MessagePtr &rawMessage)
{
    _M_RawMessage = rawMessage;
}

int Response::statusCode() const
{
    return _M_StatusCode;
}

void Response::setStatusCode(int M_StatusCode)
{
    _M_StatusCode = M_StatusCode;
}

std::string Response::errorMessage() const
{
    return _M_ErrorMessage;
}

void Response::setErrorMessage(const std::string &errorMessage)
{
    _M_ErrorMessage = errorMessage;
}

Route::ID Response::id() const
{
    return _M_ID;
}

void Response::setID(const ID &id)
{
    _M_ID = id;
}

Error Response::error() const
{
    return _M_Error;
}

void Response::setError(const Error &error)
{
    _M_Error = error;
}

CALLER_END
