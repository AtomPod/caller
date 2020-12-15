#ifndef CALLER_PERSISTENCEROUTER_HPP
#define CALLER_PERSISTENCEROUTER_HPP
#include <caller/route/router.hpp>
#include <list>

CALLER_BEGIN

class PersistenceRouter : public Router
{
    typedef std::list<RoutePtr> RouteList;
public:
    PersistenceRouter();
    virtual ~PersistenceRouter() override;
public:
    virtual void     post(ResponsePtr resp)         override;
    virtual bool     add(RoutePtr route)            override;
    virtual void     remove(RoutePtr route)         override;
private:
    RouteList        _M_Routes;
};

CALLER_END

#endif // PERSISTENCEROUTER_HPP
