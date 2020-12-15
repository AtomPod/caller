#include <caller/route/temporaryrouter.hpp>
#include <caller/call/response.hpp>
#include <caller/message/message.hpp>
#include <algorithm>

CALLER_BEGIN

TemporaryRouter::TemporaryRouter()
{

}

TemporaryRouter::~TemporaryRouter()
{

}

void TemporaryRouter::post(ResponsePtr resp)
{
    MessagePtr message = resp->rawMessage();
    if (message == nullptr)
        return;

    ID id = message->id();
    for (auto beg = _M_Routes.begin(); beg != _M_Routes.end(); ) {
        auto route = *beg;

        if (route->match(id)) {
            beg = _M_Routes.erase(beg);
            route->post(resp);
        } else {
            ++beg;
        }
    }
}

bool TemporaryRouter::add(RoutePtr route)
{
    auto where = std::find_if(_M_Routes.begin(), _M_Routes.end(), [route](RoutePtr compare) {
         if (route->id() == compare->id()) {
             return true;
         }
         return false;
    });

    if (where != _M_Routes.end()) {
        return false;
    }

    _M_Routes.push_back(route);
    return true;
}

void TemporaryRouter::remove(RoutePtr route)
{
    auto where = std::find(_M_Routes.begin(), _M_Routes.end(), route);

    if (where != _M_Routes.end()) {
        _M_Routes.erase(where);
    }
}

CALLER_END
