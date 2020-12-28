#include <caller/call/iopipelinecontext.hpp>
#include <caller/call/pipelinewritestage.hpp>
#include <caller/call/pipelinereadstage.hpp>
#include <caller/async/futureeventlistener.hpp>

CALLER_BEGIN


class StreamReadStage : public PipelineReadStage
{
public:
    StreamReadStage(size_t buffersize = 4096) : _M_ReadBuffer(buffersize) {}
    virtual ~StreamReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        UNUSED(buffer);
        UNUSED(object);
        context->notifyReadComplete();
    }
public:
    virtual void pipelineReadComplete(PipelineContextPtr ctx) override {
        ctx->read(_M_ReadBuffer);
        invokePipelineReadComplete(ctx);
    }

    virtual void pipelineActive(PipelineContextPtr ctx) override {
        ctx->read(_M_ReadBuffer);
        invokePipelineActive(ctx);
    }

    virtual void causeException(PipelineContextPtr ctx, const std::exception &e) override {
        ctx->read(_M_ReadBuffer);
        invokeCauseException(ctx, e);
    }
private:
    ByteBuffer _M_ReadBuffer;
};

class ReadCompleteStage : public PipelineReadStage
{
public:
    ReadCompleteStage()  {}
    virtual ~ReadCompleteStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        UNUSED(buffer);
        UNUSED(object);
        context->notifyReadComplete();
    }
};

class IOWriteStage : public PipelineStage
{
public:
    IOWriteStage(IOHandler* handler) : PipelineStage(PipelineStage::StageTypeBoth), _M_Handler(handler) {

    }

    virtual ~IOWriteStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object)  override {
        invokeReader(context, buffer, object);
    }

    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        UNUSED(object);

        FutureInterface<size_t> futureSender = _M_LastFuture;
        Future<size_t> future = _M_Handler->write(buffer);
        future.addListener(CreateFutureEventListenerFunction(
            [context, futureSender](const FutureEvent &e) mutable {

                if (e.isFinished()) {
                    context->notifyWriteComplete();
                    size_t* result = e.resultPtr<size_t>();
                    futureSender.reportResult(result != nullptr ? *result : 0);
                } else {
                    if (e.isError()) {
                        Error ec = e.errorCode();

                        futureSender.reportErrorCode(e.errorCode());

                        if (ec != GenericError::not_connected &&
                            ec != GenericError::not_a_socket &&
                            ec != GenericError::bad_file_descriptor) {
                            context->notifyInactive();
                        }
                    } else if (e.isException()) {
                        futureSender.reportException(e.exceptionPtr());
                    } else {
                        futureSender.reportCanceled();
                    }
                }
        }));
    }

public:
    FutureInterface<size_t> lastFuture() const
    {
        return _M_LastFuture;
    }

    void setLastFuture(const FutureInterface<size_t> &lastFuture)
    {
        _M_LastFuture = lastFuture;
    }

private:
    IOHandler *_M_Handler;
    FutureInterface<size_t> _M_LastFuture;
};


PipelineContextPtr IOPipelineContext::make(IOHandler *handler)
{
    return NewRefPtr<IOPipelineContext>(handler);
}

IOPipelineContext::~IOPipelineContext()
{

}

Future<void> IOPipelineContext::connect(const Endpoint &endpoint)
{
    Future<void> future = _M_Handler->connect(endpoint);

    auto self = shared_from_this();
    future.addListener(CreateFutureEventListenerFunction([self](const FutureEvent &e) {
        if (e.isFinished()) {
            try {
                self->notifyActive();
            } catch (const std::exception &e) {
                self->notifyException(e);
            }
        }
    }));

    return future;
}

Future<void> IOPipelineContext::disconnect()
{
    Future<void> future = _M_Handler->disconnect();

    auto self = shared_from_this();
    future.addListener(CreateFutureEventListenerFunction([self](const FutureEvent &e) {
        if (e.isFinished()) {
            try {
                self->notifyInactive();
            } catch (const std::exception &e) {
                self->notifyException(e);
            }
        }
    }));

    return future;
}

Future<void> IOPipelineContext::close()
{
    return _M_Handler->close();
}

void IOPipelineContext::read(ByteBuffer buffer)
{
    Future<size_t> future = _M_Handler->read(buffer);

    auto self = shared_from_this();
    future.addListener(CreateFutureEventListenerFunction([self, buffer](const FutureEvent &e) mutable {
        if (e.isFinished()) {
            size_t* bytes = e.resultPtr<size_t>();
            buffer.resize(*bytes);

            try {
                self->_M_Pipeline->handleRead(self, buffer, any());
            } catch (const std::exception &e) {
                self->notifyException(e);
            }
        } else {
            if (e.isError()) {
                Error ec = e.errorCode();
                if (ec != GenericError::not_connected &&
                    ec != GenericError::not_a_socket &&
                    ec != GenericError::bad_file_descriptor) {
                    self->notifyInactive();
                }
            }
        }
    }));
}

void IOPipelineContext::write(const any &object, const ByteBuffer &buffer)
{
    FutureInterface<size_t> futureSender;
    auto self = shared_from_this();
    executor()->execute([self, object, buffer, futureSender]() {
        try {
            self->_M_SocketWriteStage->setLastFuture(futureSender);
            self->_M_Pipeline->handleWrite(self, buffer, object);
        } catch (const std::exception &e) {
            self->notifyException(e);
        }
    });
}

Executor *IOPipelineContext::executor()
{
    return _M_Handler->executor();
}

IOHandler *IOPipelineContext::handler()
{
    return _M_Handler;
}

void IOPipelineContext::notifyActive()
{
    auto self = shared_from_this();
    try {
        _M_Pipeline->pipelineActive(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void IOPipelineContext::notifyInactive()
{
    auto self = shared_from_this();
    try {
        _M_Pipeline->pipelineInactive(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void IOPipelineContext::notifyReadComplete()
{
    try {
        auto self = shared_from_this();
        _M_Pipeline->pipelineReadComplete(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void IOPipelineContext::notifyWriteComplete()
{
    try {
        auto self = shared_from_this();
        _M_Pipeline->pipelineWriteComplete(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void IOPipelineContext::notifyException(const std::exception &e)
{
    auto self = shared_from_this();
    _M_Pipeline->causeException(self, e);
}

IOPipelineContext::IOPipelineContext(IOHandler *handler) : _M_Handler(handler) {
    _M_SocketWriteStage = NewRefPtr<IOWriteStage>(handler);
    _M_Pipeline         = Pipeline::make(CALLER CreateStreamReadStage(4096), _M_SocketWriteStage);
}

PipelineStagePtr CreateStreamReadStage(uint64_t buffersize)
{
    return PipelineStage::create<StreamReadStage>(buffersize);
}

CALLER_END
