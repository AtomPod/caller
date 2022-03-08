#include <caller/route/broadcastrouter.hpp>

CALLER_BEGIN

BroadcastRouter::BroadcastRouter()
{

}

BroadcastRouter::~BroadcastRouter()
{

}

void BroadcastRouter::post(const EventPtr &event)
{
    for (auto route : _M_Routes) {
        route->post(event);
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
