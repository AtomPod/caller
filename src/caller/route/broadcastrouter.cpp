#include <caller/route/broadcastrouter.hpp>

CALLER_BEGIN

BroadcastRouter::BroadcastRouter()
{

}

BroadcastRouter::~BroadcastRouter()
{

}

void BroadcastRouter::post(ResponsePtr resp)
{
    for (auto route : _M_Routes) {
        route->post(resp);
    }
}

bool BroadcastRouter::add(RoutePtr route)
{
    _M_Routes.push_back(route);
    return true;
}

void BroadcastRouter::remove(RoutePtr route)
{
    _M_Routes.remove(route);
}

CALLER_END
