#include <caller/route/routefunc.hpp>
#include <cassert>

CALLER_BEGIN

RouteFunc::RouteFunc(const Handler &handler)
    : _M_Handler(handler)
{
    assert(_M_Handler != nullptr);
}

RouteFunc::RouteFunc(const Route::ID &id, const RouteFunc::Handler &handler)
    : RouteFunc(handler)
{
    setID(id);
}

RouteFunc::RouteFunc(RouteFunc::Handler &&handler)
    : _M_Handler(std::move(handler))
{
    assert(_M_Handler != nullptr);
}

RouteFunc::RouteFunc(const Route::ID &id, RouteFunc::Handler &&handler)
    : RouteFunc(std::move(handler))
{
    setID(id);
}

caller::RouteFunc::~RouteFunc()
{

}

RouteFunc::Handler RouteFunc::handler() const
{
    return _M_Handler;
}

void RouteFunc::setHandler(const Handler &handler)
{
    _M_Handler = handler;
}

void RouteFunc::post(ResponsePtr resp)
{
    if (_M_Handler != nullptr) {
        _M_Handler(request(), resp);
    }
}

RoutePtr makeRouteFromFunc(const Route::ID &id, const RouteFunc::Handler &h)
{
    return std::make_shared<RouteFunc>(id, h);
}

RoutePtr routeFromFunc(const Route::ID &id, RouteFunc::Handler &&h)
{
    return std::make_shared<RouteFunc>(id, std::move(h));
}

CALLER_END
