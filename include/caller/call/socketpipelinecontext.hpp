#ifndef CALLER_SOCKETPIPELINECONTEXT_HPP
#define CALLER_SOCKETPIPELINECONTEXT_HPP
#include <caller/call/sockethandler.hpp>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/call/pipelinecontext.hpp>
#include <caller/call/pipeline.hpp>

CALLER_BEGIN

class SocketWriteStage;
typedef RefPtr<SocketWriteStage> SocketWriteStagePtr;

PipelineStagePtr CreateStreamReadStage(uint64_t buffersize = 4096);

class CALLER_DLL_EXPORT SocketPipelineContext : public PipelineContext, public std::enable_shared_from_this<SocketPipelineContext>
{
public:
    static PipelineContextPtr make(SocketHandler *handler);
public:
    SocketPipelineContext(SocketHandler *handler);
    virtual ~SocketPipelineContext() override;
public:
    virtual Future<void>        connect(const Endpoint &endpoint)                   override;
    virtual Future<void>        disconnect()                                        override;
    virtual Future<void>        close()                                             override;
    virtual Future<size_t>      read(ByteBuffer buffer)                             override;
    virtual Future<size_t>      write(const any &object, const ByteBuffer &buffer)  override;
    virtual Executor*           executor()                                          override;
public:
    virtual void                notifyActive() override;
    virtual void                notifyInactive() override;
    virtual void                notifyReadComplete() override;
    virtual void                notifyWriteComplete() override;
public:
    virtual PipelinePtr         readPipeline()          override { return _M_ReadPipeline;  }
    virtual PipelinePtr         writePipeline()         override { return _M_WritePipeline; }
protected:
    void                        notifyException(const std::exception &e);
private:
    SocketHandler         *_M_Handler;
    SocketWriteStagePtr   _M_SocketWriteStage;
    PipelinePtr           _M_ReadPipeline;
    PipelinePtr           _M_WritePipeline;
};

CALLER_END

#endif // SOCKETPIPELINECONTEXT_HPP
