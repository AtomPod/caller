#include <caller/route/route.hpp>
#include <iostream>

CALLER_BEGIN

Route::Route() : _M_ID(0), _M_IDMask(UINT64_MAX), _M_SequenceNumber(0), _M_SequenceNumberMask(UINT64_MAX)
{

}

Route::ID Route::id() const {
    return _M_ID;
}

void Route::setID(const ID &id)
{
    _M_ID = id;
}

bool Route::match(const EventPtr &event) const
{
    return ((this->_M_ID & idMask()) == (event->id() & idMask())) &&
            ((event->sequenceNumber() & sequenceNumberMask()) == (sequenceNumber() & sequenceNumberMask()));
}

ID Route::idMask() const
{
    return _M_IDMask;
}

void Route::setIDMask(const ID &idMask)
{
    _M_IDMask = idMask;
}

ID Route::sequenceNumber() const
{
    return _M_SequenceNumber;
}

void Route::setSequenceNumber(const ID &sequenceNumber)
{
    _M_SequenceNumber = sequenceNumber;
}

Route::ID Route::sequenceNumberMask() const
{
    return _M_SequenceNumberMask;
}

void Route::setSequenceNumberMask(const ID &mask)
{
    _M_SequenceNumberMask = mask;
}

Route::~Route()
{

}

CALLER_END
