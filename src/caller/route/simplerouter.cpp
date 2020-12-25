#include <caller/route/simplerouter.hpp>
#include <caller/route/route.hpp>
#include <iostream>


CALLER_BEGIN

SimpleRouter::SimpleRouter(ID groupMask) : _M_GroupMask(groupMask)
{

}

SimpleRouter::~SimpleRouter()
{

}

void SimpleRouter::post(const EventPtr &event)
{
    Routes routes;

    {
        std::unique_lock<std::mutex> locker(_M_Mutex);

        ID group    = _M_GroupMask & event->id();
        auto range  = _M_RouterList.equal_range(group);
        while (range.first != range.second) {
            auto route = range.first->second;
            if (route != nullptr && route->match(event)) {
                routes.push_back(route);
            }
            range.first++;
        }
    }

    for (auto route : routes) {
        route->post(event);
    }
}

bool SimpleRouter::add(RoutePtr route)
{
    if (route == nullptr) {
        return false;
    }

    std::unique_lock<std::mutex> locker(_M_Mutex);
    ID group = _M_GroupMask & route->id();
    _M_RouterList.insert(std::make_pair(group, route));
    return true;
}

void SimpleRouter::remove(RoutePtr route)
{
    if (route == nullptr)
        return;

    std::unique_lock<std::mutex> locker(_M_Mutex);
    ID group = _M_GroupMask & route->id();
    auto range = _M_RouterList.equal_range(group);
    while (range.first != range.second) {
        auto croute = range.first->second;
        if (croute != route) {
            _M_RouterList.erase(range.first);
            return;
        }
        range.first++;
    }
}

CALLER_END
