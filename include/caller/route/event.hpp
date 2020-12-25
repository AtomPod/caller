#ifndef CALLER_EVENT_HPP
#define CALLER_EVENT_HPP
#include <caller/core.hpp>
#include <caller/common/id.hpp>
#include <caller/common/any.hpp>
#include <caller/route/declare.hpp>
#include <caller/common/error.hpp>

CALLER_BEGIN



class CALLER_DLL_EXPORT Event {
public:
    typedef ID 				ID;
    typedef CALLER any		Any;

public:
    static EventPtr make(const ID &id, const ID &sequenceNumber, const Any &payload, const Error &e = Error());
public:
    Event(const ID &id, const ID &sequenceNumber, const Any &payload, const Error &e);
    virtual ~Event();
public:
    virtual  ID  		id()                     const;
    virtual  ID         sequenceNumber()        const;
    virtual  Error		error()  ;
    virtual  Any		payload();
private:
    ID		_M_ID;
    ID      _M_SequenceNumber;
    Error   _M_ErrCode;
    Any		_M_Payload;
};

CALLER_END

#endif // CALLER_EVENT_HPP
