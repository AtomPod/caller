#ifndef CALLER_TEMPORARYROUTER_HPP
#define CALLER_TEMPORARYROUTER_HPP
#include <caller/core.hpp>
#include <caller/route/router.hpp>
#include <list>

CALLER_BEGIN

/**
 * @brief The TemporaryRouter class 【非线程安全】储存临时路由，匹配一次后就失效
 */
class CALLER_DLL_EXPORT TemporaryRouter : public Router
{
    typedef std::list<RoutePtr> RouteList;
public:
    TemporaryRouter();
    virtual ~TemporaryRouter() override;
public:
    virtual void     post(const EventPtr &event)    override;

    /**
     * @brief add    添加路由到列表中
     * @param route
     * @return route的id存在时，返回false，否则返回true
     */
    virtual bool     add(RoutePtr route)            override;
    virtual void     remove(RoutePtr route)         override;
private:
    RouteList        _M_Routes;
};

CALLER_END

#endif // CALLER_TEMPORARYROUTER_HPP
