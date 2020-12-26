#ifndef CALLER_FUTURE_HPP
#define CALLER_FUTURE_HPP
#include <caller/core.hpp>
#include <caller/async/futureinterface.hpp>
#include <caller/executor/executor.hpp>
#include <caller/executor/threadexecutioncontext.hpp>
#include <iostream>

CALLER_BEGIN

template <typename T>
class Future
{
public:
    Future() : m_interface(makeEmptFutureInterface<T>()){}
    Future(FutureInterface<T> *i) : m_interface(*i) {}

public:
    void wait() { m_interface.wait(); }
public:
    bool isPending()  const { return m_interface.isPending();  }
    bool isFinished() const { return m_interface.isFinished(); }
    bool isCanceled() const { return m_interface.isCanceled(); }
public:
    T            result()         const;
    void         addListener(RefPtr<FutureEventListener> listener) { m_interface.addListener(listener); }
    void         removeListener(RefPtr<FutureEventListener> listener) { m_interface.removeListener(listener); }
private:
    mutable FutureInterface<T> m_interface;
};


template<typename T>
inline T Future<T>::result() const {
    m_interface.wait();
    return m_interface.result();
}

template<typename T>
inline Future<T> FutureInterface<T>::future() {
    return Future<T>(this);
}

template <>
class Future<void>
{
public:
    Future() : m_interface(makeEmptFutureInterface<void>()) {}
    Future(FutureInterface<void> *i) : m_interface(*i) {}

public:
    void wait() { m_interface.wait(); }
public:
    bool isPending()  const { return m_interface.isPending();  }
    bool isFinished() const { return m_interface.isFinished(); }
    bool isCanceled() const { return m_interface.isCanceled(); }
public:
    void    addListener(RefPtr<FutureEventListener> listener) { m_interface.addListener(listener); }
    void    removeListener(RefPtr<FutureEventListener> listener) { m_interface.removeListener(listener); }
private:
    mutable FutureInterface<void> m_interface;
};

inline Future<void> FutureInterface<void>::future() {
    return Future<void>(this);
}

CALLER_END

#endif // CALLER_FUTURE_HPP
