#pragma once
#ifndef CALLER_ROUTE_H_
#define CALLER_ROUTE_H_

#include <caller/core.hpp>
#include <caller/common/id.hpp>
#include <caller/call/declare.hpp>
#include <caller/context/context.hpp>
#include <caller/route/event.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Route : public std::enable_shared_from_this<Route>
{
public:
    typedef CALLER ID  ID;
public:
    Route();
    virtual ~Route();

public:
    // 对应匹配的ID（事件ID）
    ID   id() const;
    void setID(const ID &id);

    // ID 掩码，用于匹配使用，默认为全匹配
    ID idMask() const;
    void setIDMask(const ID &idMask);

    // 序号
    ID sequenceNumber() const;
    void setSequenceNumber(const ID &sequenceNumber);

    // 序号掩码，用于匹配时使用，默认为全匹配
    ID sequenceNumberMask() const;
    void setSequenceNumberMask(const ID &sequenceNumberMask);
public:
    virtual bool match(const EventPtr &event) const;
    virtual void post(const EventPtr &event) = 0;

private:
    ID                  _M_ID;
    ID                  _M_IDMask;
    ID                  _M_SequenceNumber;
    ID                  _M_SequenceNumberMask;
};


CALLER_END

#endif //CALLER_ROUTE_H_
