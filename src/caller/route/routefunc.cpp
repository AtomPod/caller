#include <caller/route/routefunc.hpp>
#include <cassert>

CALLER_BEGIN

RouteFunc::RouteFunc(const Handler &handler)
    : _M_Handler(handler)
{
    assert(_M_Handler != nullptr);
}

RouteFunc::RouteFunc(const Route::ID &id, const ID &sequence, const RouteFunc::Handler &handler)
    : RouteFunc(handler)
{
    setID(id);
    setSequenceNumber(sequence);
}

RouteFunc::RouteFunc(RouteFunc::Handler &&handler)
    : _M_Handler(std::move(handler))
{
    assert(_M_Handler != nullptr);
}

RouteFunc::RouteFunc(const Route::ID &id, const ID &sequence, RouteFunc::Handler &&handler)
    : RouteFunc(std::move(handler))
{
    setID(id);
    setSequenceNumber(sequence);
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

void RouteFunc::post(const EventPtr &event)
{
    if (_M_Handler != nullptr) {
        _M_Handler(event);
    }
}

RoutePtr makeRouteFromFunc(const Route::ID &id, const Route::ID &sequence, const RouteFunc::Handler &h)
{
    return NewRefPtr<RouteFunc>(id, sequence, h);
}

RoutePtr RouteFromFunc(const Route::ID &id, const Route::ID &sequence,  RouteFunc::Handler &&h)
{
    return NewRefPtr<RouteFunc>(id, sequence, std::move(h));
}

CALLER_END
