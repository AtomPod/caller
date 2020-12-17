#include <caller/call/asio/tcpsockethandler.hpp>
#include <caller/call/pipelinewritestage.hpp>
#include <caller/call/pipelinereadstage.hpp>

CALLER_BEGIN

class StreamReadStage : public PipelineReadStage
{
public:
    StreamReadStage() : _M_ReadBuffer(4096) {}
    virtual ~StreamReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        auto next = nextStage();
        if (next != nullptr) {
            next->handleRead(context, buffer, object);
        }
    }
public:
    virtual void pipelineActive(PipelineContextPtr ctx) override {
        ctx->read(_M_ReadBuffer);
    }

    virtual void causeException(PipelineContextPtr ctx, const std::exception &) override {
        ctx->read(_M_ReadBuffer);
    }

    virtual void pipelineReadComplete(PipelineContextPtr ctx)  override {
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
        future.whenFinished([context, futureSender](size_t size) mutable {
            context->notifyWriteComplete();
            futureSender.reportResult(size);
        }).whenCanceled([context](const std::error_code &ec, const std::exception_ptr &e) {
            UNUSED(ec);
            UNUSED(e);
            context->notifyInactive();
        });
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

AsioTcpSocketHandler::AsioTcpSocketHandler(AsioExecutionContext &context)
    : _M_ExecutionContext(context), _M_Socket(context.rawContext()),
      _M_ConnState(ConnectivityState::Idle), _M_Resolver(context.rawContext()),
      _M_DataBuffer(8192)
{

}

AsioTcpSocketHandler::~AsioTcpSocketHandler()
{

}

Future<void> AsioTcpSocketHandler::bind(const Endpoint &endpoint)
{
    return Future<void>();
}

Future<void> AsioTcpSocketHandler::connect(const Endpoint &endpoint)
{
    FutureInterface<void> futureSender;
    if (_M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Connecting ||
            _M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Ready) {
        futureSender.reportErrorCode(std::make_error_code(std::errc::already_connected));
        return futureSender.future();
    }

    resetSocket();
    if (_M_EndpointList.size() == 0) {
        resolverAddress(endpoint, futureSender);
    } else {
        connectWithEndpointList(_M_EndpointList, futureSender);
    }
    return futureSender.future();
}

Future<void> AsioTcpSocketHandler::disconnect()
{
    FutureInterface<void> futureSender;
    if (setConnectivityState(ConnectivityState::Shutdown)) {
        if (_M_Socket.is_open()) {
            std::error_code code;
            _M_Socket.shutdown(asio::socket_base::shutdown_both, code);
            _M_Socket.close(code);

            if (code) {
                futureSender.reportErrorCode(code);
            } else {
                futureSender.reportFinished();
            }
            return futureSender.future();
        }
    }
    futureSender.reportErrorCode(std::make_error_code(std::errc::not_connected));
    return futureSender.future();
}

Future<void> AsioTcpSocketHandler::close()
{
    return disconnect();
}

Future<size_t> AsioTcpSocketHandler::read(ByteBuffer buffer)
{
    FutureInterface<size_t> futureSender;
    readNext(buffer, futureSender);
    return futureSender.future();
}

Future<size_t> AsioTcpSocketHandler::write(const ByteBuffer &buffer)
{
    FutureInterface<size_t> futureSender;

    asio::async_write(_M_Socket, asio::const_buffer(buffer.data(), buffer.length()),
                      asio::transfer_all(),
                      [buffer, futureSender, this](std::error_code ec, size_t length) mutable {
        if (!ec) {
            futureSender.reportResult(length);
        } else {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureSender.reportErrorCode(ec);
        }
    });
    return futureSender.future();
}

Executor *AsioTcpSocketHandler::executor()
{
    return _M_ExecutionContext.executor();
}

void AsioTcpSocketHandler::resolverAddress(const Endpoint &endpoint, FutureInterface<void> futureSender)
{
    Protocol::resolver::query query(endpoint.host(),endpoint.port());
    _M_Resolver.async_resolve(query,
                              [this, futureSender](std::error_code e,
                              Protocol::resolver::iterator epIt) mutable {
        if (e) {
            futureSender.reportErrorCode(e);
            return;
        }

        this->_M_EndpointList.clear();
        auto end = Protocol::resolver::iterator();
        for (auto begin = epIt; begin != end; ++begin) {
            this->_M_EndpointList.push_back(begin->endpoint());
        }
        this->connectWithEndpointList(this->_M_EndpointList, futureSender);
    });
}

void AsioTcpSocketHandler::resetSocket()
{
    if (_M_Socket.is_open()) {
        ::asio::error_code ec;
        _M_Socket.shutdown(asio::socket_base::shutdown_both, ec);
        _M_Socket.close(ec);
        _M_Socket = Socket(_M_ExecutionContext.rawContext());
    }
}

void AsioTcpSocketHandler::connectWithEndpointList(
        const AsioTcpSocketHandler::EndpointList &endpoints,
        FutureInterface<void> futureInterface)
{
    setConnectivityState(ConnectivityState::Connecting);
    asio::async_connect(_M_Socket, endpoints, [this, futureInterface](std::error_code ec, Protocol::endpoint e) mutable {
        UNUSED(e);

        if (ec) {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureInterface.reportErrorCode(ec);
            return;
        }
        futureInterface.reportFinished();
    });
}

bool AsioTcpSocketHandler::setConnectivityState(const SocketHandler::ConnectivityState &state)
{
    if (_M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Shutdown) {
        return false;
    }

    int oldState = _M_ConnState.exchange(state, std::memory_order_acq_rel);
    if (oldState != state) {
        return true;
    }
    return false;
}

void AsioTcpSocketHandler::readNext(ByteBuffer buffer, FutureInterface<size_t> futureSender)
{
    _M_Socket.async_read_some(asio::buffer(buffer.data(), buffer.capacity()),
                              [buffer, futureSender, this](std::error_code ec, size_t length) mutable {
        if (ec) {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureSender.reportErrorCode(ec);
        } else {
            futureSender.reportResult(length);
        }
    });
}

PipelineContextPtr AsioTcpPipelineContext::make(AsioTcpSocketHandler *handler)
{
    return std::make_shared<AsioTcpPipelineContext>(handler);
}

AsioTcpPipelineContext::~AsioTcpPipelineContext()
{

}

Future<void> AsioTcpPipelineContext::connect(const Endpoint &endpoint)
{
    Future<void> future = _M_Handler->connect(endpoint);

    auto self = shared_from_this();
    future.whenFinished([self]() {
        try {
            self->notifyActive();
        } catch (const std::exception &e) {
            self->notifyException(e);
        }
    });

    return future;
}

Future<void> AsioTcpPipelineContext::disconnect()
{
    Future<void> future = _M_Handler->disconnect();

    auto self = shared_from_this();
    future.whenFinished([self]() {
        try {
            self->notifyInactive();
        } catch (const std::exception &e) {
            self->notifyException(e);
        }
    });

    return future;
}

Future<void> AsioTcpPipelineContext::close()
{
    return _M_Handler->close();
}

Future<size_t> AsioTcpPipelineContext::read(ByteBuffer buffer)
{
    Future<size_t> future = _M_Handler->read(buffer);

    auto self = shared_from_this();
    future.whenFinished([self, buffer](size_t bytes) mutable {
        buffer.resize(bytes);

        try {
            self->_M_ReadPipeline->handleRead(self, buffer, any());
        } catch (const std::exception &e) {
            self->notifyException(e);
        }

    }).whenCanceled([self](const std::error_code &ec, const std::exception_ptr &e) {
        UNUSED(ec);
        UNUSED(e);
        self->notifyInactive();
    });

    return future;
}

Future<size_t> AsioTcpPipelineContext::write(const any &object, const ByteBuffer &buffer)
{
    FutureInterface<size_t> futureSender;
    auto self = shared_from_this();
    executor()->execute([self, object, buffer, futureSender]() {
        try {
            self->_M_SocketWriteStage->setLastFuture(futureSender);
            self->_M_WritePipeline->handleWrite(self, buffer, object);
        } catch (const std::exception &e) {
            self->notifyException(e);
        }
    });
    return futureSender.future();
}

Executor *AsioTcpPipelineContext::executor()
{
    return _M_Handler->executor();
}

void AsioTcpPipelineContext::notifyActive()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineActive(self);
        _M_WritePipeline->pipelineActive(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void AsioTcpPipelineContext::notifyInactive()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineInactive(self);
        _M_WritePipeline->pipelineInactive(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void AsioTcpPipelineContext::notifyReadComplete()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineReadComplete(self);
        _M_WritePipeline->pipelineReadComplete(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void AsioTcpPipelineContext::notifyWriteComplete()
{
    try {
        auto self = shared_from_this();
        _M_ReadPipeline->pipelineWriteComplete(self);
        _M_WritePipeline->pipelineWriteComplete(self);
    } catch (const std::exception &e) {
        notifyException(e);
    }
}

void AsioTcpPipelineContext::notifyException(const std::exception &e)
{
    auto self = shared_from_this();
    _M_ReadPipeline->causeException(self, e);
    _M_WritePipeline->causeException(self, e);
}

AsioTcpPipelineContext::AsioTcpPipelineContext(AsioTcpSocketHandler *handler) : _M_Handler(handler) {
    _M_SocketWriteStage = std::make_shared<SocketWriteStage>(handler);
    _M_WritePipeline    = Pipeline::make(_M_SocketWriteStage);
    _M_ReadPipeline     = Pipeline::make(PipelineStage::create<ReadCompleteStage>());
    _M_ReadPipeline->append(PipelineStage::create<StreamReadStage>());
}

CALLER_END
