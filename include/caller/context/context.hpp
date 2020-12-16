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
    typedef Channel::CancelFunc                     ChannelCancelFunc;
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
    typedef std::shared_ptr<ContextBase>    ContextBasePtr;
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
    std::mutex          _M_Mutex;
    Error               _M_Err;
    ContextWeakPtr      _M_Parent;
    ChannelSender       _M_DoneSender;
    ChannelCancelFunc   _M_ParentChannelCancel;
    ChildrenSet         _M_Childrens;
};

//class CALLER_DLL_EXPORT Context
//{
//public:
//    typedef CALLER Variant Value;
//public:
//    typedef ::std::string                           Key;
//    typedef ::std::unordered_map<Key, Value>        Metadata;
//    typedef ::std::chrono::steady_clock             Clock;
//    typedef Clock::time_point                       Time;
//    typedef Clock::duration                         Duration;

//    static Time now();
//    static Time fromNow(const Duration &d);
//public:
//    Context(ContextPtr ptr = nullptr);
//    virtual ~Context();
//public: // Metadata
//    // 非线程安全函数，当存在parent时，查询时，若不存在该context中
//    // 会调用parent的对应函数
//    void    setMetadata(const Key &key, const Value &value);
//    Value   metadata(const Key& key, bool* exsist = nullptr) const;
//    bool    hasMetadata(const Key &key);
//    bool    removeMetadata(const Key &key);

//    template<typename T>
//    T       metadata(const Key &key, bool* exists = nullptr) const {
//        bool _exists = false;
//        Value value = metadata(key, &_exists);
//        if (_exists) {
//            return value.convertTo<T>();
//        }

//        if (exists != nullptr) {
//            *exists = _exists;
//        }
//        return T();
//    }
//public: // Expire
//    // 非线程安全，当存在parent时，需要考虑多线程问题
//    // isExpired 会先调用parent进行判断，是否过期
//    // 所以当多线程使用时，不要动态调用setExpireFromNow和setExpireAt
//    void    setExpireFromNow(const Duration &d);
//    void    setExpireAt(const Time &expireat);
//    Time    expireAt()  const;
//    bool    isExpired() const;
//public: // Cancel
//    // 线程安全，采用原子/锁形式实现，所以即使多线程调用
//    // 也会符合要求
//    virtual void    cancel();
//    virtual bool    isCanceled() const;
//public:
//    // 非线程安全函数
//    ContextPtr      parent() const;
//    void            setParent(const ContextPtr &parent);

//private:
//    Metadata            _M_Metadata;
//    Time                _M_ExpireAt;
//    std::atomic_bool    _M_Canceled;
//    ContextPtr          _M_Parent;
//};

CALLER_END

#endif //CALLER_CONTEXT_H_
