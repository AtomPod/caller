#ifndef CALLER_ROUTER_HPP
#define CALLER_ROUTER_HPP
#include <caller/core.hpp>
#include <caller/route/route.hpp>
#include <caller/route/declare.hpp>
#include <functional>
#include <list>
#include <caller/context/context.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Router : public Route {
public:
    template<typename T, typename... Args>
    static RouterPtr create(Args&& ...args) {
        return NewRefPtr<T>(std::forward<Args>(args)...);
    }
public:
    Router();
    virtual ~Router() override;
public:
    virtual bool     match(const EventPtr &event)  const    override;
    virtual void     post(const EventPtr &event)            override;
    virtual bool     add(RoutePtr route)                    = 0;
    virtual void     remove(RoutePtr route)                 = 0;
};

CALLER_END

#endif // CALLER_ROUTER_HPP
