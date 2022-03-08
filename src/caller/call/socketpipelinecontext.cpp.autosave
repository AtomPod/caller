#include <caller/call/socketpipelinecontext.hpp>
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
        auto next = nextStage();
        if (next != nullptr) {
            next->handleRead(context, buffer, object);
        }
    }
public:
    virtual void pipelineReadComplete(PipelineContextPtr ctx) override {
        ctx->read(_M_ReadBuffer);
    }

    virtual void pipelineActive(PipelineContextPtr ctx) override {
        ctx->read(_M_ReadBuffer);
    }

    virtual void causeException(PipelineContextPtr ctx, const std::exception &) override {
        ctx->read(_M_ReadBuffer);
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

class SocketWriteStage : public PipelineWriteStage
{
public:
    SocketWriteStage(SocketHandler* handler) : _M_Handler(handler) {

    }

    virtual ~SocketWriteStage() override = default;
public:
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

//        future.whenFinished([context, futureSender](size_t size) mutable {
//            context->notifyWriteComplete();
//            futureSender.reportResult(size);
//        }).whenCanceled([context, futureSender](const std::error_code &ec, const std::exception_ptr &e) mutable {
//            UNUSED(ec);
//            UNUSED(e);
//            futureSender.reportErrorCode(ec);
//            if (ec != asio::error::not_connected &&
//                ec != asio::error::not_socket &&
//                ec != asio::error::bad_descriptor) {
//                context->notifyInactive();
//            }
//        });
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
    SocketHandler *_M_Handler;
    FutureInterface<size_t> _M_LastFuture;
};


PipelineContextPtr IOPipelineContext::make(SocketHandler *handler)
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
                self->notifyException(ExceptionFromBoth, e);
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
                self->notifyException(ExceptionFromBoth, e);
            }
        }
    }));

    return future;
}

Future<void> IOPipelineContext::close()
{
    return _M_Handler->close();
}

Future<size_t> IOPipelineContext::read(ByteBuffer buffer)
{
    Future<size_t> future = _M_Handler->read(buffer);

    auto self = shared_from_this();
    future.addListener(CreateFutureEventListenerFunction([self, buffer](const FutureEvent &e) mutable {
        if (e.isFinished()) {
            size_t* bytes = e.resultPtr<size_t>();
            buffer.resize(*bytes);

            try {
                self->_M_ReadPipeline->handleRead(self, buffer, any());
            } catch (const std::exception &e) {
                self->notifyException(ExceptionFromRead, e);
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

//    future.whenFinished([self, buffer](size_t bytes) mutable {
//        buffer.resize(bytes);

//        try {
//            self->_M_ReadPipeline->handleRead(self, buffer, any());
//        } catch (const std::exception &e) {
//            self->notifyException(e);
//        }

//    }).whenCanceled([self](const std::error_code &ec, const std::exception_ptr &e) {
//        UNUSED(ec);
//        UNUSED(e);

//        if (ec != asio::error::not_connected &&
//            ec != asio::error::not_socket &&
//            ec != asio::error::bad_descriptor) {
//            self->notifyInactive();
//        }
//    });

    return future;
}

Future<size_t> IOPipelineContext::write(const any &object, const ByteBuffer &buffer)
{
    FutureInterface<size_t> futureSender;
    auto self = shared_from_this();
    executor()->execute([self, object, buffer, futureSender]() {
        try {
            self->_M_SocketWriteStage->setLastFuture(futureSender);
            self->_M_WritePipeline->handleWrite(self, buffer, object);
        } catch (const std::exception &e) {
            self->notifyException(ExceptionFromWrite, e);
        }
    });
    return futureSender.future();
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
        _M_ReadPipeline->pipelineActive(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromRead, e);
    }

    try {
        _M_WritePipeline->pipelineActive(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromWrite, e);
    }
}

void IOPipelineContext::notifyInactive()
{
    auto self = shared_from_this();
    try {
        _M_ReadPipeline->pipelineInactive(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromRead, e);
    }

    try {
        _M_WritePipeline->pipelineInactive(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromWrite, e);
    }
}

void IOPipelineContext::notifyReadComplete()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineReadComplete(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromRead, e);
    }
}

void IOPipelineContext::notifyWriteComplete()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineWriteComplete(self);
    } catch (const std::exception &e) {
        notifyException(ExceptionFromWrite, e);
    }
}

void IOPipelineContext::notifyException(const ExceptionFrom &from, const std::exception &e)
{
    auto self = shared_from_this();
    if (from & ExceptionFromRead) {
        _M_ReadPipeline->causeException(self, e);
    }

    if (from & ExceptionFromWrite) {
        _M_WritePipeline->causeException(self, e);
    }
}

IOPipelineContext::IOPipelineContext(SocketHandler *handler) : _M_Handler(handler) {
    _M_SocketWriteStage = NewRefPtr<SocketWriteStage>(handler);
    _M_WritePipeline    = Pipeline::make(_M_SocketWriteStage);
    _M_ReadPipeline     = Pipeline::make(PipelineStage::create<ReadCompleteStage>());
}

PipelineStagePtr CreateStreamReadStage(uint64_t buffersize)
{
    return PipelineStage::create<StreamReadStage>(buffersize);
}

CALLER_END
