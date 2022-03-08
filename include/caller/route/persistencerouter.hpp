#ifndef CALLER_PERSISTENCEROUTER_HPP
#define CALLER_PERSISTENCEROUTER_HPP
#include <caller/route/router.hpp>
#include <mutex>
#include <list>

CALLER_BEGIN

/**
 * @brief The PersistenceRouter class 持久性路由，进行匹配，匹配完成执行但不删除
 */
class CALLER_DLL_EXPORT PersistenceRouter : public Router
{
    typedef std::list<RoutePtr>     RouteList;
    typedef std::mutex              Mutex;
    typedef std::unique_lock<Mutex> Locker;
public:
    PersistenceRouter();
    virtual ~PersistenceRouter() override;
public:
    virtual void     post(const EventPtr &event)         override;
    virtual bool     add(RoutePtr route)                 override;
    virtual void     remove(RoutePtr route)              override;
private:
    Mutex            _M_Mutex;
    RouteList        _M_Routes;
};

CALLER_END

#endif // PERSISTENCEROUTER_HPP
