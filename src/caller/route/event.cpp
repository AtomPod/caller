#include <caller/route/event.hpp>

CALLER_BEGIN

EventPtr Event::make(const Event::ID &id, const ID &sequenceNumber, const Event::Any &payload, const Error &e)
{
    return NewRefPtr<Event>(id, sequenceNumber, payload, e);
}

Event::Event(const ID &id, const ID &sequenceNumber,  const Any &payload, const Error &e)
    : _M_ID(id), _M_SequenceNumber(sequenceNumber), _M_ErrCode(e), _M_Payload(payload)
{

}

Event::~Event()
{

}

Event::ID Event::id() const
{
    return _M_ID;
}

Event::ID Event::sequenceNumber() const
{
    return _M_SequenceNumber;
}

Error Event::error()
{
    return _M_ErrCode;
}

Event::Any Event::payload()
{
    return _M_Payload;
}

CALLER_END
