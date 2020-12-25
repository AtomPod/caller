#ifndef CALLER_ROUTE_DECLARE_HPP
#define CALLER_ROUTE_DECLARE_HPP
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class Route;
typedef RefPtr<Route>       RoutePtr;
typedef WeakRefPtr<Route>   RouteWeakPtr;

class Router;
typedef RefPtr<Router>      RouterPtr;

class Event;
typedef RefPtr<Event>       EventPtr;

CALLER_END

#endif // CALLER_ROUTE_DECLARE_HPP
