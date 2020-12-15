#ifndef CALLER_TRANSPORT_HPP
#define CALLER_TRANSPORT_HPP
#include <caller/core.hpp>
#include <caller/call/declare.hpp>
#include <caller/context/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/common/error.hpp>
#include <functional>

CALLER_BEGIN

class Transport : public std::enable_shared_from_this<Transport>
{
public:
    typedef std::function<void(ContextPtr, ByteBuffer)> Stream;
public:
    Transport();
    virtual ~Transport();
public:
    virtual void   handleStream(const Stream &s) = 0;
    virtual Error  write(const ByteBuffer &data)  = 0;
    virtual Error  close()                        = 0;
};

CALLER_END

#endif // CALLER_TRANSPORT_HPP
