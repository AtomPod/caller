#ifndef CALLER_BROADCASTROUTER_HPP
#define CALLER_BROADCASTROUTER_HPP
#include <caller/core.hpp>
#include <caller/route/router.hpp>
#include <list>

CALLER_BEGIN

/**
 * @brief The BroadcastRouter class 【非线程安全】广播型路由，不进行匹配，
 * 均进行执行
 */
class CALLER_DLL_EXPORT BroadcastRouter : public Router
{
    typedef std::list<RoutePtr> RouteList;
public:
    BroadcastRouter();
    virtual ~BroadcastRouter() override;

public:
    virtual void     post(const EventPtr &event)  override;
    virtual bool     add(RoutePtr route)          override;
    virtual void     remove(RoutePtr route)       override;

private:
    RouteList       _M_Routes;
};

CALLER_END

#endif // CALLER_BROADCASTROUTER_HPP
