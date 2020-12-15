#ifndef ROUTEFUNC_HPP
#define ROUTEFUNC_HPP
#include <caller/route/route.hpp>
#include <caller/route/declare.hpp>
#include <caller/call/iocontext.hpp>
#include <caller/call/response.hpp>
#include <functional>

CALLER_BEGIN

class CALLER_DLL_EXPORT RouteFunc : public Route
{
public:
    typedef std::function<void(RequestPtr,ResponsePtr)> Handler;
public:
    RouteFunc(const Handler &handler);
    RouteFunc(const ID &id, const Handler &handler);
    RouteFunc(Handler &&handler);
    RouteFunc(const ID &id, Handler &&handler);
    virtual ~RouteFunc();

public:
    Handler handler() const;
    void setHandler(const Handler &handler);

protected:
    virtual void post(ResponsePtr resp);

private:
    Handler _M_Handler;
};

RoutePtr routeFromFunc(const Route::ID &id, const RouteFunc::Handler &h);
RoutePtr routeFromFunc(const Route::ID &id,  RouteFunc::Handler &&h);
CALLER_END

#endif // ROUTEFUNC_HPP
