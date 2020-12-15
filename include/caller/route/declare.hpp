#ifndef CALLER_ROUTE_DECLARE_HPP
#define CALLER_ROUTE_DECLARE_HPP
#include <memory>
#include <caller/core.hpp>

CALLER_BEGIN

class Route;
typedef std::shared_ptr<Route> RoutePtr;
typedef std::weak_ptr<Route>   RouteWeakPtr;

class Router;
typedef std::shared_ptr<Router> RouterPtr;

CALLER_END

#endif // CALLER_ROUTE_DECLARE_HPP
