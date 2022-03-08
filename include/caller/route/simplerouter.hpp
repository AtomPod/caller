#ifndef SIMPLEROUTER_HPP
#define SIMPLEROUTER_HPP
#include <caller/route/router.hpp>
#include <list>
#include <mutex>
#include <unordered_map>

CALLER_BEGIN

class CALLER_DLL_EXPORT SimpleRouter : public Router
{
private:
    typedef std::list<RoutePtr>							 Routes;
    typedef std::unordered_multimap<Route::ID, RoutePtr> RouterList;
    typedef RouterList::iterator                         Iterator;
    typedef RouterList::const_iterator                   ConstIterator;
public:
    SimpleRouter(ID groupMask = 0xFFFFFFFF);
    virtual ~SimpleRouter() override;
public:
    virtual void     post(const EventPtr &event)  override;
    virtual bool     add(RoutePtr route)          override;
    virtual void     remove(RoutePtr route)       override;
private:
    mutable std::mutex       _M_Mutex;
    RouterList               _M_RouterList;
    ID                       _M_GroupMask;
};

CALLER_END

#endif // SIMPLEROUTER_HPP
