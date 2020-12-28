#ifndef CALLER_IOPIPELINECONTEXT_HPP
#define CALLER_IOPIPELINECONTEXT_HPP
#include <caller/call/sockethandler.hpp>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/call/pipelinecontext.hpp>
#include <caller/call/pipeline.hpp>

CALLER_BEGIN

class IOWriteStage;
typedef RefPtr<IOWriteStage> IOWriteStagePtr;

PipelineStagePtr CreateStreamReadStage(uint64_t buffersize = 4096);

class CALLER_DLL_EXPORT IOPipelineContext : public PipelineContext, public std::enable_shared_from_this<IOPipelineContext>
{
public:
    static PipelineContextPtr make(IOHandler *handler);
public:
    IOPipelineContext(IOHandler *handler);
    virtual ~IOPipelineContext() override;
public:
    virtual Future<void>        connect(const Endpoint &endpoint)                   override;
    virtual Future<void>        disconnect()                                        override;
    virtual Future<void>        close()                                             override;
    virtual void                read(ByteBuffer buffer)                             override;
    virtual void                write(const any &object, const ByteBuffer &buffer)  override;
    virtual Executor*           executor()                                          override;
    virtual IOHandler*          handler()                                           override;
public:
    virtual void                notifyActive() override;
    virtual void                notifyInactive() override;
    virtual void                notifyReadComplete() override;
    virtual void                notifyWriteComplete() override;
public:
    virtual PipelinePtr         pipeline()          override { return _M_Pipeline;  }
protected:
    void                        notifyException(const std::exception &e);
private:
    IOHandler             *_M_Handler;
    IOWriteStagePtr       _M_SocketWriteStage;
    PipelinePtr           _M_Pipeline;
};

CALLER_END

#endif // SOCKETPIPELINECONTEXT_HPP
