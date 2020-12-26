#pragma once
#ifndef CALLER_CONTEXT_H_
#define CALLER_CONTEXT_H_

#include <caller/core.hpp>
#include <unordered_map>
#include <string>
#include <chrono>
#include <caller/common/any.hpp>
#include <atomic>
#include <optional>
#include <caller/context/declare.hpp>
#include <caller/common/id.hpp>
#include <caller/async/future.hpp>
#include <sigslot/signal.hpp>
#include <unordered_set>

CALLER_BEGIN

class CALLER_DLL_EXPORT Context : public std::enable_shared_from_this<Context> {
    friend class ContextBase;
public:
    typedef CALLER Variant ValueType;
    typedef ::std::string  KeyType;
    typedef ::std::chrono::steady_clock             Clock;
    typedef Clock::time_point                       Time;
    typedef ::std::error_code                       Error;
    typedef ::std::optional<Time>                   OptionalTime;
    typedef Future<Error>                           Channel;
protected:
    typedef FutureInterface<Error>                  ChannelSender;
public:
    Context()           = default;
    virtual ~Context()  = default;
public:
    virtual Error           Err()                           =   0;
    virtual ValueType       Value(const KeyType &key)       =   0;
    virtual OptionalTime    Deadline()                      =   0;
    virtual Channel         Done()                          =   0;
protected:
    virtual void            addChild(ContextPtr child)       =   0;
    virtual void            removeChild(ContextPtr child)    =   0;
public:
    typedef std::function<void()>               CancelFunc;
    typedef std::pair<ContextPtr, CancelFunc>   ContextCancelPair;

    static  ContextPtr          backgroundContext();

    static  ContextPtr          withValue(ContextPtr parent, const KeyType &key, const ValueType &value);
    static  ContextCancelPair   withCancel(ContextPtr parent);
};

class CALLER_DLL_EXPORT ContextBase : public Context
{
    typedef RefPtr<ContextBase>    ContextBasePtr;
    typedef std::unordered_set<ContextPtr>  ChildrenSet;
    friend class Context;
public:
    ContextBase();
    virtual ~ContextBase();
public:
    virtual Error           Err()                       ;
    virtual ValueType       Value(const KeyType &key)   ;
    virtual OptionalTime    Deadline()                  ;
    virtual Channel         Done()                     ;

protected:
    void        setParent(const ContextPtr &parent);
    void        setError(const Error &err);
    ContextPtr  parent();

    virtual void           addChild(ContextPtr child)     ;
    virtual void           removeChild(ContextPtr child)  ;
protected:
    std::mutex                  _M_Mutex;
    Error                       _M_Err;
    ContextWeakPtr              _M_Parent;
    ChannelSender               _M_DoneSender;
    RefPtr<FutureEventListener> _M_ParentChannelListener;
    ChildrenSet                 _M_Childrens;
};

CALLER_END

#endif //CALLER_CONTEXT_H_
