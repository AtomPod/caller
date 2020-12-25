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
    typedef FutureInterfaceBase::CancelFunc CancelFunc;
    typedef std::function<void(T)>          FinishedCallback;
    typedef std::function<void(const std::error_code errcode, const std::exception_ptr &e)> CanceledCallback;
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
    Future<T>&   whenFinished(FinishedCallback cb, CancelFunc *cancel = nullptr);
    Future<T>&   whenCanceled(CanceledCallback cb, CancelFunc *cancel = nullptr);

    Future<T>&   whenFinished(ExecutionContext* executor,FinishedCallback cb, CancelFunc *cancel = nullptr);
    Future<T>&   whenCanceled(ExecutionContext* executor,CanceledCallback cb, CancelFunc *cancel = nullptr);
private:
    mutable FutureInterface<T> m_interface;
};

template<typename T>
inline Future<T>& Future<T>::whenFinished(FinishedCallback cb, CancelFunc *cancel) {
    return whenFinished(ThreadExecutionContext::executionContext(), std::move(cb), cancel);
}

template<typename T>
inline Future<T>& Future<T>::whenCanceled(CanceledCallback cb, CancelFunc *cancel) {
    return whenCanceled(ThreadExecutionContext::executionContext(), std::move(cb), cancel);
}

template<typename T>
inline Future<T>& Future<T>::whenFinished(ExecutionContext* context, FinishedCallback cb, CancelFunc *cancel) {
    Future<T> self = *this;

    auto cancelFunc = m_interface.listen([self, cb, context](int state) {
        if (state == FutureInterfaceBase::Finished) {
            Executor* executor = nullptr;
            if (context != nullptr && (executor = context->executor()) != nullptr) {
                auto r = self.result();
                executor->execute([r, cb]() {
                    cb(r);
                });
            } else {
                cb(self.result());
            }
        }
    });

    if (cancel != nullptr) {
        *cancel = std::move(cancelFunc);
    }

    return *this;
}

template<typename T>
inline Future<T>& Future<T>::whenCanceled(ExecutionContext* context, CanceledCallback cb, CancelFunc *cancel) {
    Future<T> self = *this;
    auto cancelFunc = m_interface.listen([self, cb, context](int state) {
        if (state == FutureInterfaceBase::Canceled) {
            Executor* executor = nullptr;
            if (context != nullptr && (executor = context->executor()) != nullptr ) {
                auto errcode   = self.m_interface.errorCode();
                auto exception = self.m_interface.exception();
                executor->execute([errcode, exception, cb]() {
                    cb(errcode, exception);
                });
            } else {
                cb(self.m_interface.errorCode(),self.m_interface.exception());
            }
        }
    });

    if (cancel != nullptr) {
        *cancel = std::move(cancelFunc);
    }

    return *this;
}

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
    typedef FutureInterfaceBase::CancelFunc CancelFunc;
    typedef std::function<void()> FinishedCallback;
    typedef std::function<void(const std::error_code& errcode, const std::exception_ptr &e)> CanceledCallback;
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
    Future<void>&   whenFinished(FinishedCallback cb, CancelFunc *cancel = nullptr);
    Future<void>&   whenCanceled(CanceledCallback cb, CancelFunc *cancel = nullptr);

    Future<void>&   whenFinished(ExecutionContext* executor,FinishedCallback cb, CancelFunc *cancel = nullptr);
    Future<void>&   whenCanceled(ExecutionContext* executor,CanceledCallback cb, CancelFunc *cancel = nullptr);
private:
    mutable FutureInterface<void> m_interface;
};

inline Future<void>& Future<void>::whenFinished(FinishedCallback cb, CancelFunc *cancel) {
    return whenFinished(ThreadExecutionContext::executionContext(), std::move(cb), cancel);
}

inline Future<void>& Future<void>::whenCanceled(CanceledCallback cb, CancelFunc *cancel) {
    return whenCanceled(ThreadExecutionContext::executionContext(), std::move(cb), cancel);
}

inline Future<void>& Future<void>::whenFinished(ExecutionContext* context, FinishedCallback cb, CancelFunc *cancel) {
    auto cancelFunc = m_interface.listen([cb, context](int state) {
        if (state == FutureInterfaceBase::Finished) {
            Executor* executor = nullptr;
            if (context != nullptr && (executor = context->executor()) != nullptr) {
                executor->execute(cb);
            } else {
                cb();
            }
        }
    });

    if (cancel != nullptr) {
        *cancel = std::move(cancelFunc);
    }

    return *this;
}

inline Future<void>& Future<void>::whenCanceled(ExecutionContext* context, CanceledCallback cb, CancelFunc *cancel) {
    Future<void> self = *this;

    auto cancelFunc = m_interface.listen([self, cb, context](int state) {
        if (state == FutureInterfaceBase::Canceled) {
            Executor* executor = nullptr;
            if (context != nullptr && (executor = context->executor()) != nullptr) {
                auto errcode   = self.m_interface.errorCode();
                auto exception = self.m_interface.exception();
                executor->execute([errcode, exception, cb]() {
                    cb(errcode, exception);
                });
            } else {
                cb(self.m_interface.errorCode(),self.m_interface.exception());
            }
        }
    });

    if (cancel != nullptr) {
        *cancel = std::move(cancelFunc);
    }

    return *this;
}
inline Future<void> FutureInterface<void>::future() {
    return Future<void>(this);
}

CALLER_END

#endif // CALLER_FUTURE_HPP
