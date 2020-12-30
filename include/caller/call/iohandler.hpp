#ifndef CALLER_IOHANDLER_HPP
#define CALLER_IOHANDLER_HPP

#include <caller/core.hpp>
#include <caller/async/future.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/call/endpoint.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT IOHandler
{
public:
    IOHandler();
    virtual ~IOHandler();
public:
    virtual Future<void>        bind(const Endpoint &endpoint)      = 0;
    virtual Future<void>        connect(const Endpoint &endpoint)   = 0;
    virtual Future<void>        disconnect()                        = 0;
    virtual Future<void>        close()                             = 0;
    virtual Future<size_t>      read(ByteBuffer buffer)             = 0;
    virtual Future<size_t>      write(const ByteBuffer &buffer)     = 0;
    virtual Executor*           executor()                          = 0;
    virtual Future<void>        closeFuture()                      = 0;
};

CALLER_END

#endif // CALLER_IOHANDLER_HPP
