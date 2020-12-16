#include <caller/route/persistencerouter.hpp>
//#include <caller/call/response.hpp>
#include <caller/message/message.hpp>

CALLER_BEGIN

PersistenceRouter::PersistenceRouter()
{

}

PersistenceRouter::~PersistenceRouter()
{

}

void PersistenceRouter::post(ResponsePtr resp)
{
//    MessagePtr message = resp->rawMessage();
//    if (message == nullptr)
//        return;
//    ID id = message->id();

//    for (auto route : _M_Routes) {
//        if (route->match(id)) {
//            route->post(resp);
//        }
//    }
}

bool PersistenceRouter::add(RoutePtr route)
{
    auto where = std::find(_M_Routes.begin(), _M_Routes.end(), route);
    if (where != _M_Routes.end()) {
        return false;
    }

    _M_Routes.push_back(route);
    return true;
}

void PersistenceRouter::remove(RoutePtr route)
{
    UNUSED(std::remove(_M_Routes.begin(), _M_Routes.end(), route));
}

CALLER_END
