#ifndef CALLER_UNDERLYINGSERVICE_HPP
#define CALLER_UNDERLYINGSERVICE_HPP
#include <caller/call/service.hpp>
#include <caller/message/declare.hpp>
#include <caller/call/pipelinestagewrapper.hpp>

CALLER_BEGIN

class UnderlyingService : public Service
{
public:
    class Config {
    public:
        Config();
    public:
        ID routerGroupMask() const;
        void setRouterGroupMask(const ID &routerGroupMask);

        RouterPtr router() const;
        void setRouter(const RouterPtr &router);

        MessageFactoryPtr messageFactory() const;
        void setMessageFactory(const MessageFactoryPtr &messageFactory);

        PipelinePtr readPipeline() const;
        void setReadPipeline(const PipelinePtr &readPipeline);

        PipelinePtr writePipeline() const;
        void setWritePipeline(const PipelinePtr &writePipeline);

    private:
        ID                  m_routerGroupMask;
        RouterPtr           m_router;
        MessageFactoryPtr   m_messageFactory;
        PipelinePtr         m_writePipeline;
        PipelinePtr         m_readPipeline;
    };
public:
    static const Config defaultConfig;
public:
    UnderlyingService(const Config *config = &defaultConfig);
    virtual ~UnderlyingService();

public:
    virtual PipelineStagePtr transportStage() const = 0;

public:
    MessageFactoryPtr   messageFactory() const;
    RouterPtr           router() const;

    PipelinePtr         writePipeline() const;
    PipelinePtr         readPipeline() const;

    const Config *      config() const;

public:
    void addWriteHandler(const PipelineStageWrapper::Handler &handler);
    void addReadHandler(const PipelineStageWrapper::Handler &handler);

    void addWriteHandler(const PipelineStagePtr &handler);
    void addReadHandler(const PipelineStagePtr &handler);

protected:
    void                init();

private:
    MessageFactoryPtr  _M_MessageFactory;
    RouterPtr          _M_Router;
    const Config      *_M_Config;
    PipelinePtr        _M_WritePipeline;
    PipelinePtr        _M_ReadPipeline;
};

CALLER_END

#endif // UNDERLYINGSERVICE_HPP
