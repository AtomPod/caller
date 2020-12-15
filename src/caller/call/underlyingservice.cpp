#include <caller/call/underlyingservice.hpp>
#include <caller/message/messagefactory.hpp>
#include <caller/route/router.hpp>
#include <caller/call/pipeline.hpp>

CALLER_BEGIN

const UnderlyingService::Config UnderlyingService::defaultConfig;

UnderlyingService::UnderlyingService(const Config *config)
    : _M_MessageFactory(nullptr),
      _M_Router(nullptr),
      _M_Config(config)
{
    init();
}

UnderlyingService::~UnderlyingService()
{

}

MessageFactoryPtr UnderlyingService::messageFactory() const
{
    return _M_MessageFactory;
}

RouterPtr UnderlyingService::router() const
{
    return _M_Router;
}

void UnderlyingService::init()
{
    if (_M_Config == nullptr) {
        _M_Config = &defaultConfig;
    }

    if (_M_Config->router() != nullptr) {
        _M_Router = _M_Config->router();
    } else {
        _M_Router = Router::make(_M_Config->routerGroupMask());
    }

    if (_M_Config->messageFactory() != nullptr) {
        _M_MessageFactory = _M_Config->messageFactory();
    } else {
        _M_MessageFactory = MessageFactory::make();
    }

    if (_M_Config->readPipeline() != nullptr) {
        _M_ReadPipeline = _M_Config->readPipeline();
    } else {
        _M_ReadPipeline = Pipeline::make(nullptr);
    }

    if (_M_Config->writePipeline() != nullptr) {
        _M_WritePipeline = _M_Config->writePipeline();
    } else {
        _M_WritePipeline = Pipeline::make(nullptr);
    }
}

PipelinePtr UnderlyingService::readPipeline() const
{
    return _M_ReadPipeline;
}

PipelinePtr UnderlyingService::writePipeline() const
{
    return _M_WritePipeline;
}

const UnderlyingService::Config *UnderlyingService::config() const
{
    return _M_Config;
}

void UnderlyingService::addWriteHandler(const PipelineStageWrapper::Handler &handler)
{
    if (_M_WritePipeline == nullptr || handler == nullptr)
        return;

    _M_WritePipeline->append(PipelineStageWrapper::make(handler));
}

void UnderlyingService::addReadHandler(const PipelineStageWrapper::Handler &handler)
{
    if (_M_ReadPipeline == nullptr || handler == nullptr)
        return;

    _M_ReadPipeline->append(PipelineStageWrapper::make(handler));
}

void UnderlyingService::addWriteHandler(const PipelineStagePtr &handler)
{
    if (_M_WritePipeline == nullptr || handler == nullptr)
        return;

    _M_WritePipeline->append(handler);
}

void UnderlyingService::addReadHandler(const PipelineStagePtr &handler)
{
    if (_M_ReadPipeline == nullptr || handler == nullptr)
        return;

    _M_ReadPipeline->append(handler);
}

UnderlyingService::Config::Config() : m_routerGroupMask(0xFFFFFFFFFFFFFFFF),
                                      m_router(nullptr),
                                      m_messageFactory(nullptr),
                                      m_writePipeline(nullptr)
{

}

ID UnderlyingService::Config::routerGroupMask() const
{
    return m_routerGroupMask;
}

void UnderlyingService::Config::setRouterGroupMask(const ID &routerGroupMask)
{
    m_routerGroupMask = routerGroupMask;
}

RouterPtr UnderlyingService::Config::router() const
{
    return m_router;
}

void UnderlyingService::Config::setRouter(const RouterPtr &router)
{
    m_router = router;
}

MessageFactoryPtr UnderlyingService::Config::messageFactory() const
{
    return m_messageFactory;
}

void UnderlyingService::Config::setMessageFactory(const MessageFactoryPtr &messageFactory)
{
    m_messageFactory = messageFactory;
}

PipelinePtr UnderlyingService::Config::writePipeline() const
{
    return m_writePipeline;
}

void UnderlyingService::Config::setWritePipeline(const PipelinePtr &writePipeline)
{
    m_writePipeline = writePipeline;
}

PipelinePtr UnderlyingService::Config::readPipeline() const
{
    return m_readPipeline;
}

void UnderlyingService::Config::setReadPipeline(const PipelinePtr &readPipeline)
{
    m_readPipeline = readPipeline;
}

CALLER_END
