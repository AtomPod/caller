#ifndef CALLER_CALL_REQUEST_HPP
#define CALLER_CALL_REQUEST_HPP
#include <atomic>
#include <caller/core.hpp>
#include <caller/route/route.hpp>
#include <caller/call/declare.hpp>
#include <caller/message/declare.hpp>
#include <caller/context/context.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/common/any.hpp>
#include <caller/common/id.hpp>
#include <caller/async/future.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Request
{
public:
    typedef  std::chrono::steady_clock::duration Duration;
public:
    static RequestPtr make(size_t buffsize = 0);
public:
    Request(size_t buffsize = 0);
    virtual ~Request();
public:

    ByteBuffer &dataBuffer();
    ByteBuffer dataBuffer() const;
    void       setDataBuffer(const ByteBuffer &dataBuffer);

    MessagePtr rawMessage() const;
    void       setRawMessage(const MessagePtr &rawMessage);

    std::string method() const;
    void        setMethod(const std::string &method);

    Route::ID id() const;
    void      setID(const ID &id);

    bool canceled() const;
    void cancel();

    FutureInterface<RequestResponsePtr> response() const;
    void setResponse(const FutureInterface<RequestResponsePtr> &response);

    ContextPtr context() const;
    void setContext(const ContextPtr &context);
private:
    std::string                         _M_Method;
    ByteBuffer                          _M_DataBuffer;
    MessagePtr                          _M_RawMessage;
    Duration                            _M_ExpireAt;
    Route::ID                           _M_ID;
    std::atomic_bool                    _M_Canceled;
    FutureInterface<RequestResponsePtr> _M_Response;
    ContextPtr                          _M_Context;
};

CALLER_END
CALLER_CALL_REQUEST_HPP
#endif // CALL_REQUEST_HPP
