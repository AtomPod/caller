#include <caller/call/asio/asiotcpservice.hpp>
#include <caller/call/pipeline.hpp>
#include <caller/call/request.hpp>

CALLER_BEGIN

AsioTcpServicePtr AsioTcpService::make(AsioExecutionContext &context, const Config *config)
{
    return std::make_shared<AsioTcpService>(context, config);
}

AsioTcpService::AsioTcpService(AsioExecutionContext &context, const Config *config)
    : AsioService(context, config), _M_DataBuffer(8192), _M_Connection(nullptr)
    , _M_Config(config)
{
    _M_Connection = AsioTcpConnection::make(this);
    _M_Connection->connectStateSignal([this](int newState) {
        this->setConnectivityState(newState);
    });
    _M_Connection->connect(config->endpoint(), nullptr);
}

void AsioTcpService::onConnectivityStateChanged(int newState)
{
    if (newState == Connection::Ready) {
        readData();
    }
}

void AsioTcpService::onData(caller::ByteBuffer data)
{
    // 接收数据，那么对于socket来说，都与service同一个线程，所以直接处理
    PipelinePtr pipeline = this->readPipeline();
    if (pipeline != nullptr) {
        IOContextPtr innerContext = IOContext::make();
        pipeline->propagate(innerContext, data, shared_from_this());
    }
}

PipelineStagePtr AsioTcpService::transportStage() const
{
    auto _self = shared_from_this();
    return PipelineStageWrapper::make([_self](IOContextPtr ctx,
                                      ByteBuffer data,
                                      ServicePtr service,
                                      PipelineStagePtr next) {
        int state = _self->_M_Connection->connectivityState();
        if (state != Connection::Ready) {
            RequestPtr request = ctx->request();
            if (request != nullptr) {
                FutureInterface<RequestResponsePtr> futureInterface = request->response();
                futureInterface.reportErrorCode(1);
            }
            return;
        }

        _self->_M_Connection->write(data,
        [ctx, data, service, next](std::error_code e, size_t size) {
            UNUSED(size);
            if (!e && next != nullptr) {
                next->propagate(ctx, data, service);
            }
        });
    });
}

Future<RequestResponsePtr> AsioTcpService::invoke(IOContextPtr ctx, RequestPtr request)
{
    int state = connectivityState();
    if (state > Connection::Connecting && state < Connection::TransitentFailure) {
        return  Future<RequestResponsePtr>();
    }

    PipelinePtr pipeline = this->writePipeline();
    if (pipeline != nullptr) {
        FutureInterface<RequestResponsePtr> promise;
        auto _self = shared_from_this();
        request->setResponse(promise);

        // 推送到service执行线程中执行
        post([ctx, request, _self, pipeline]() {
            // 防止Context污染
            IOContextPtr innerContext = IOContext::make();
//            innerContext->setParent(ctx);
            innerContext->setRequest(request);

            pipeline->propagate(innerContext, request->dataBuffer(), _self);
        });
        return promise.future();
    }
    return Future<RequestResponsePtr>();
}

void AsioTcpService::readData()
{
    auto _self = shared_from_this();
    _M_DataBuffer.resize(0);
    _M_Connection->read(_M_DataBuffer, [_self](std::error_code e, size_t size) {
        if (!e) {
            auto dataBuffer = _self->_M_DataBuffer;
            dataBuffer.resize(size);
            _self->onData(dataBuffer);
            _self->readData();
        }
    });
}

AsioTcpConnection::Endpoint AsioTcpService::Config::endpoint() const
{
    return m_endpoint;
}

void AsioTcpService::Config::setEndpoint(const AsioTcpConnection::Endpoint &endpoint)
{
    m_endpoint = endpoint;
}

CALLER_END
