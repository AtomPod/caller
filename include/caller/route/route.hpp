#pragma once
#ifndef CALLER_ROUTE_H_
#define CALLER_ROUTE_H_

#include <caller/core.hpp>
#include <caller/common/id.hpp>
#include <caller/call/declare.hpp>
#include <caller/context/context.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Route
{
public:
    static const Context::KeyType MetaTag;
public:
    enum Type {
      TEMPORARY,
      PERSISTENCE
    };
public:
    typedef CALLER ID  ID;
public:
    Route();
    virtual ~Route();

public:
    ID   id() const;
    void setID(const ID &id);

    Type type() const;
    void setType(const Type &type);

    ID mask() const;
    void setMask(const ID &mask);
public:
    virtual bool match(const ID &id) const;
    virtual void post(ResponsePtr resp) = 0;

    RequestPtr  request() const;
    void        setRequest(const RequestPtr &request);
private:
    ID                  _M_ID;
    ID                  _M_Mask;
    Type                _M_Type;
    RequestPtr          _M_Request;
};


CALLER_END

#endif //CALLER_ROUTE_H_
