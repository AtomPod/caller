#ifndef ROUTEFUNC_HPP
#define ROUTEFUNC_HPP
#include <caller/route/route.hpp>
#include <caller/route/declare.hpp>
#include <functional>

CALLER_BEGIN

class CALLER_DLL_EXPORT RouteFunc : public Route
{
public:
    typedef std::function<void(const EventPtr &)> Handler;
public:
    RouteFunc(const Handler &handler);
    RouteFunc(const ID &id, const Route::ID &sequence, const Handler &handler);
    RouteFunc(Handler &&handler);
    RouteFunc(const ID &id, const Route::ID &sequence, Handler &&handler);
    virtual ~RouteFunc();

public:
    Handler handler() const;
    void setHandler(const Handler &handler);

protected:
    virtual void post(const EventPtr &event);

private:
    Handler _M_Handler;
};

RoutePtr RouteFromFunc(const Route::ID &id,  const Route::ID &sequence, const RouteFunc::Handler &h);
RoutePtr RouteFromFunc(const Route::ID &id,  const Route::ID &sequence, RouteFunc::Handler &&h);
CALLER_END

#endif // ROUTEFUNC_HPP
