#include <caller/route/router.hpp>
#include <caller/route/simplerouter.hpp>

CALLER_BEGIN

const Context::KeyType Router::MetaTag = "caller-router-meta-key";

Router::Router()
{

}

Router::~Router()
{

}

void Router::post(ResponsePtr resp)
{
    UNUSED(resp);
}

RouterPtr Router::make(ID groupMask)
{
    return std::make_shared<SimpleRouter>(groupMask);
}

CALLER_END
