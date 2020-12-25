#include <caller/route/router.hpp>
#include <caller/route/simplerouter.hpp>

CALLER_BEGIN


Router::Router()
{

}

Router::~Router()
{

}

bool Router::match(const EventPtr &event) const
{
    UNUSED(event);
    return true;
}

void Router::post(const EventPtr &event)
{
    UNUSED(event);
}

CALLER_END
