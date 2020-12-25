#include <caller/route/persistencerouter.hpp>

CALLER_BEGIN

PersistenceRouter::PersistenceRouter()
{

}

PersistenceRouter::~PersistenceRouter()
{

}

void PersistenceRouter::post(const EventPtr &e)
{
    Locker locker(_M_Mutex);
    for (auto route : _M_Routes) {
        if (route->match(e)) {
            route->post(e);
        }
    }
}

bool PersistenceRouter::add(RoutePtr route)
{
    Locker locker(_M_Mutex);
    auto where = std::find(_M_Routes.begin(), _M_Routes.end(), route);
    if (where != _M_Routes.end()) {
        return false;
    }

    _M_Routes.push_back(route);
    return true;
}

void PersistenceRouter::remove(RoutePtr route)
{
    Locker locker(_M_Mutex);
    UNUSED(std::remove(_M_Routes.begin(), _M_Routes.end(), route));
}

CALLER_END
