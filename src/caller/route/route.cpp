#include <caller/route/route.hpp>
#include <iostream>

CALLER_BEGIN

const Context::KeyType Route::MetaTag = "caller-route-meta-key";

Route::Route() : _M_ID(0), _M_Mask(UINT64_MAX), _M_Type(TEMPORARY)
{

}

Route::ID Route::id() const {
    return _M_ID;
}

void Route::setID(const ID &id)
{
    _M_ID = id;
}

Route::Type Route::type() const
{
    return _M_Type;
}

void Route::setType(const Type &type)
{
    _M_Type = type;
}

bool Route::match(const Route::ID &id) const
{
    return (this->_M_ID & mask()) == (id & mask());
}

RequestPtr Route::request() const
{
    return _M_Request;
}

void Route::setRequest(const RequestPtr &request)
{
    _M_Request = request;
}

Route::ID Route::mask() const
{
    return _M_Mask;
}

void Route::setMask(const ID &mask)
{
    _M_Mask = mask;
}

Route::~Route()
{

}

CALLER_END
