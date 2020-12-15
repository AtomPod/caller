#ifndef CALLER_ROUTER_HPP
#define CALLER_ROUTER_HPP
#include <caller/core.hpp>
#include <caller/route/route.hpp>
#include <caller/route/declare.hpp>
#include <functional>
#include <list>
#include <caller/context/context.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Router : public Route
{
public:
    typedef std::list<RoutePtr> Routes;
public:
    static const Context::KeyType   MetaTag;
public:
    static RouterPtr make(ID groupMask = 0xFFFFFFFF);
public:
    typedef std::function<bool(RoutePtr)> RouteCallback;
public:
    Router();
    virtual ~Router() override;
public:
    virtual void     post(ResponsePtr resp)       override;
    virtual bool     add(RoutePtr route)                = 0;
    virtual void     remove(RoutePtr route)             = 0;
};

CALLER_END

#endif // CALLER_ROUTER_HPP
