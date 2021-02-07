#ifndef CALLER_PIPELINECONTEXT_HPP
#define CALLER_PIPELINECONTEXT_HPP
#include <caller/call/declare.hpp>
#include <caller/call/endpoint.hpp>
#include <caller/async/future.hpp>
#include <caller/common/any.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/executor/executor.hpp>

CALLER_BEGIN

class IOHandler;
class CALLER_DLL_EXPORT PipelineContext
{
public:
    PipelineContext();
    virtual ~PipelineContext();
public:
    virtual Future<void>        connect(const Endpoint &endpoint)                   = 0;
    virtual Future<void>        disconnect()                                        = 0;
    virtual Future<void>        close()                                             = 0;
    virtual void                read(ByteBuffer buffer)                             = 0;
    virtual void                write(const Any &object, const ByteBuffer &buffer)  = 0;
    virtual Executor*           executor()                                          = 0;
    virtual IOHandler*          handler()                                           = 0;

    virtual void                notifyActive()          = 0;
    virtual void                notifyInactive()        = 0;
    virtual void                notifyReadComplete()    = 0;
    virtual void                notifyWriteComplete()   = 0;
public:
    virtual PipelinePtr         pipeline()          = 0;
};

CALLER_END

#endif // CALLER_PIPELINECONTEXT_HPP
