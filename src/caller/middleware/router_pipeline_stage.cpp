#include <caller/middleware/router_pipeline_stage.hpp>
#include <caller/call/iocontext.hpp>
#include <caller/call/request.hpp>
#include <caller/call/response.hpp>
#include <caller/call/contextresponse.hpp>
#include <caller/call/service.hpp>
#include <caller/route/routefunc.hpp>
#include <caller/message/message.hpp>
#include <cassert>

CALLER_BEGIN
const Context::KeyType RouterPipelineStage::MetaTag = "caller-router-pipeline-meta-tag";

RouterPipelineStagePtr RouterPipelineStage::make(RouterPtr router, RouterPtr routeRegister)
{
    return std::make_shared<RouterPipelineStage>(router, routeRegister);
}

RouterPipelineStage::RouterPipelineStage(RouterPtr router, RouterPtr routeRegister)
    : _M_Router(router), _M_RouteRegister(routeRegister)
{

}

RouterPipelineStage::~RouterPipelineStage()
{

}

RouterPipelineStage::Handler RouterPipelineStage::requestStage()
{
    assert(_M_Router != nullptr);

    auto _this = shared_from_this();
    return [_this](IOContextPtr context,
            ByteBuffer data,
            ServicePtr service,
            PipelineStagePtr next) {

        RouterPtr  routeRegister    = _this->_M_RouteRegister;
        RequestPtr request          = context->request();
        FutureInterface<RequestResponsePtr> futureInterface = request->response();

        RoutePtr route = CALLER routeFromFunc(request->id(), [futureInterface](RequestPtr req,
                                              ResponsePtr resp) mutable {
            futureInterface.reportResult(RequestResponse::make(req, resp));
        });
        route->setRequest(context->request());
        routeRegister->add(route);

        Future<RequestResponsePtr> canceled = futureInterface.future();

        canceled.whenCanceled([routeRegister, service, route](int ec, const std::exception_ptr &e) {
            UNUSED(ec);
            UNUSED(e);

            service->post([routeRegister, service, route]() {
                routeRegister->remove(route);
            });
        });

        if (next != nullptr) {
            next->propagate(context, data, service);
        }
    };
}

RouterPipelineStage::Handler RouterPipelineStage::responseStage()
{
    assert(_M_Router != nullptr);

    auto _this = shared_from_this();
    return [_this](IOContextPtr context,
            ByteBuffer data,
            ServicePtr service,
            PipelineStagePtr next) {
        auto response = context->response();
        if (response == nullptr) {
            return;
        }
        auto message = response->rawMessage();
        if (message == nullptr) {
            return;
        }

        _this->_M_Router->post(response);

        if (next != nullptr) {
            next->propagate(context, data, service);
        }
    };
}

RouterPtr RouterPipelineStage::router() const
{
    return _M_Router;
}

CALLER_END
