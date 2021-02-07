#ifndef CALLER_FUTUREINTERFACE_HPP
#define CALLER_FUTUREINTERFACE_HPP
#include <caller/core.hpp>
#include <caller/common/refptr.hpp>
#include <caller/common/error.hpp>
#include <sigslot/signal.hpp>
#include <mutex>
#include <functional>
#include <iostream>

CALLER_BEGIN

class FutureEvent
{
public:
    typedef unsigned char           StorageElement;
    typedef RefPtr<StorageElement>  Storage;
public:
    enum EventType {
       None,
       Finished,
       Canceled
    };
public:
    FutureEvent() : _M_Event(None), _M_Storage(nullptr), _M_ExceptionPtr(nullptr) {}
    FutureEvent(const EventType &t) : _M_Event(t), _M_Storage(nullptr), _M_ExceptionPtr(nullptr) {}

public:
    EventType event() const;
    void setEvent(const EventType &event);

    Storage storage() const;
    void setStorage(const Storage &storage);

    Error errorCode() const;
    void setErrorCode(const Error &errorCode);

    std::exception_ptr exceptionPtr() const;
    void setExceptionPtr(const std::exception_ptr &exceptionPtr);

public:
    bool isFinished()   const { return _M_Event == Finished; }
    bool isCanceled()   const { return _M_Event == Canceled; }
    bool isError()      const { return isCanceled() && _M_ErrorCode; }
    bool isException()  const { return isCanceled() && _M_ExceptionPtr != nullptr; }
    bool hasResult()    const { return isFinished() && _M_Storage != nullptr; }

    template<typename T>
    T result() const {
        if (!hasResult()) {
            return T();
        }

        StorageElement* elements = _M_Storage.get();
        if (elements == nullptr) {
            return T();
        }

        T* typedPtr = reinterpret_cast<T*>(elements);
        if (typedPtr == nullptr) {
            return T();
        }

        return *typedPtr;
    }

    template<typename T>
    T* resultPtr() const {
        StorageElement* elements = _M_Storage.get();
        if (elements == nullptr) {
            return nullptr;
        }
        return reinterpret_cast<T*>(elements);
    }

private:
    EventType               _M_Event;
    Storage                 _M_Storage;
    Error                   _M_ErrorCode;
    std::exception_ptr      _M_ExceptionPtr;
};

class FutureEventListener
{
public:
    template<typename T, typename... Args>
    static RefPtr<FutureEventListener> create(Args&&... args) {
        return NewRefPtr<T>(std::forward<Args>(args)...);
    }
public:
    FutureEventListener() = default;
    virtual ~FutureEventListener() = default;
public:
    virtual void postEvent(const FutureEvent &) = 0;
};

class FutureInterfaceBaseImpl;
class FutureInterfaceBase
{
    friend class FutureInterfaceBaseImpl;
    typedef RefPtr<FutureInterfaceBaseImpl> PrivatePtr;
public:
    enum class WaitStatus {
      Success,
      Timeout
    };

    enum State {
      Pending  = 1,
      Finished = 2,
      Canceled = 4
    };
public:
    typedef std::function<void(int)>    CallOut;
    typedef std::function<void()>       CancelFunc;
public:
    FutureInterfaceBase(size_t storeSpace = 0);
    ~FutureInterfaceBase();
public:
    void reportErrorCode(const Error &errorcode);
    void reportException(const std::exception_ptr &e);
    void reportFinished();
    void reportCanceled();
public:
    void        addListener(RefPtr<FutureEventListener> listener);
    void        removeListener(RefPtr<FutureEventListener> listener);
    void        wait();
public:
    bool isPending()  const;
    bool isFinished() const;
    bool isCanceled() const;
public:
    std::error_code     errorCode()  const;
    std::exception_ptr  exception()  const;
    std::mutex          &mutex()     const;
protected:
    void                *store()     const;
protected:
    bool queryState(int state) const;
private:
    PrivatePtr _M_Ptr;
};

template<typename T>
class Future;

template<typename T>
class FutureInterface : public FutureInterfaceBase
{
public:
    FutureInterface() : FutureInterfaceBase(sizeof (T)) {}
public:
    void reportResult(const T &result);
    T    result();

    Future<T> future();
};

template <typename T>
inline void FutureInterface<T>::reportResult(const T &result) {
    std::unique_lock<std::mutex> locker(mutex());
    if (isPending()) {
        void *data = store();
        new(data)T(result);
        locker.unlock();

        reportFinished();
    }
}

template <typename T>
inline T FutureInterface<T>::result() {
    std::unique_lock<std::mutex> locker(mutex());
    if (isCanceled()) {
        locker.unlock();
        std::exception_ptr e = exception();
        if (e != nullptr) {
            std::rethrow_exception(e);
        }
        return T();
    }
    return *reinterpret_cast<T*>(store());
}

template<>
class FutureInterface<void> : public FutureInterfaceBase {
public:
    FutureInterface() {}
public:
    Future<void> future();
};

template <typename T>
inline FutureInterface<T> makeEmptyFutureInterface() {
    FutureInterface<T> i;
    i.reportCanceled();
    return i;
}

CALLER_END

#endif // CALLER_FUTUREINTERFACE_HPP
