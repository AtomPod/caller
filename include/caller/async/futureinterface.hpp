#ifndef CALLER_FUTUREINTERFACE_HPP
#define CALLER_FUTUREINTERFACE_HPP
#include <caller/core.hpp>
#include <caller/common/refptr.hpp>
#include <sigslot/signal.hpp>
#include <mutex>
#include <functional>
#include <iostream>

CALLER_BEGIN

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
    void reportErrorCode(const std::error_code &errorcode);
    void reportException(const std::exception_ptr &e);
    void reportFinished();
    void reportCanceled();
public:
    CancelFunc listen(const CallOut &out);
    void       wait();
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
inline FutureInterface<T> makeEmptFutureInterface() {
    FutureInterface<T> i;
    i.reportCanceled();
    return i;
}

CALLER_END

#endif // CALLER_FUTUREINTERFACE_HPP
