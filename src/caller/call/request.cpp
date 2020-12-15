#include <caller/call/request.hpp>

CALLER_BEGIN

RequestPtr Request::make(size_t buffsize)
{
    return std::make_shared<Request>(buffsize);
}

Request::Request(size_t buffsize)
    : _M_DataBuffer(buffsize), _M_RawMessage(nullptr), _M_ExpireAt(0), _M_ID(0),
      _M_Canceled(false)
{

}

Request::~Request()
{

}

ByteBuffer &Request::dataBuffer()
{
    return _M_DataBuffer;
}

ByteBuffer Request::dataBuffer() const
{
    return _M_DataBuffer;
}

void Request::setDataBuffer(const ByteBuffer &dataBuffer)
{
    _M_DataBuffer = dataBuffer;
}

MessagePtr Request::rawMessage() const
{
    return _M_RawMessage;
}

void Request::setRawMessage(const MessagePtr &rawMessage)
{
    _M_RawMessage = rawMessage;
}

std::string Request::method() const
{
    return _M_Method;
}

void Request::setMethod(const std::string &method)
{
    _M_Method = method;
}

Route::ID Request::id() const
{
    return _M_ID;
}

void Request::setID(const ID &id)
{
    _M_ID = id;
}

bool Request::canceled() const
{
    return _M_Canceled.load(std::memory_order_acquire);
}

void Request::cancel()
{
    _M_Canceled.store(true, std::memory_order_release);
}

FutureInterface<RequestResponsePtr> Request::response() const
{
    return _M_Response;
}

void Request::setResponse(const FutureInterface<RequestResponsePtr> &M_Response)
{
    _M_Response = M_Response;
}

ContextPtr Request::context() const
{
    return _M_Context;
}

void Request::setContext(const ContextPtr &M_Context)
{
    _M_Context = M_Context;
}

CALLER_END
