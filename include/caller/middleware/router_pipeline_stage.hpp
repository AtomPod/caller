#ifndef CALLER_ROUTER_INTERCEPTOR_H
#define CALLER_ROUTER_INTERCEPTOR_H
#include <caller/middleware/declare.hpp>
#include <caller/route/router.hpp>
#include <caller/call/pipelinestagewrapper.hpp>
#include <caller/context/context.hpp>

CALLER_BEGIN

class RouterPipelineStage : public std::enable_shared_from_this<RouterPipelineStage>
{
public:
    static void setRouteToContext(IOContextPtr ctx, RoutePtr route);
public:
    typedef PipelineStageWrapper::Handler Handler;
public:
    static const Context::KeyType   MetaTag;
public:
    static RouterPipelineStagePtr make(RouterPtr router, RouterPtr routeRegister = nullptr);
public:
    RouterPipelineStage(RouterPtr router, RouterPtr routeRegister = nullptr);
    virtual ~RouterPipelineStage();
public:
    Handler requestStage();
    Handler responseStage();
public:
    RouterPtr router() const;
private:
    mutable RouterPtr   _M_Router;
    mutable RouterPtr   _M_RouteRegister;
};

CALLER_END

#endif // CALLER_ROUTER_INTERCEPTOR_H
