#include <caller/call/service.hpp>
#include <caller/call/request.hpp>
#include <caller/call/response.hpp>
#include <caller/route/router.hpp>
#include <caller/middleware/router_pipeline_stage.hpp>

CALLER_BEGIN

Service::Service()
{

}

Service::~Service()
{

}

int Service::connectivityState() const
{
    return _M_ConnectivityState.load(std::memory_order_acquire);
}

void Service::setConnectivityState(const int &connectivityState)
{
    int oldState = _M_ConnectivityState.exchange(connectivityState, std::memory_order_acq_rel);
    if (oldState != connectivityState) {
        onConnectivityStateChanged(connectivityState);
    }
}

CALLER_END
