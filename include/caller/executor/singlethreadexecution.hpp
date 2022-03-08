#ifndef CALLER_SINGLETHREADEXECUTION_HPP
#define CALLER_SINGLETHREADEXECUTION_HPP
#include <caller/executor/executor.hpp>
#include <type_traits>
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <iostream>
#include <caller/executor/threadexecutioncontext.hpp>
#include <caller/executor/asio/asioexecutor.hpp>

CALLER_BEGIN

template<typename ExecutionContextT,
        typename std::enable_if< std::is_base_of<ExecutionContext, ExecutionContextT>::value &&
                                 std::is_member_pointer<decltype (&ExecutionContextT::start) >::value &&
                                 std::is_member_pointer<decltype (&ExecutionContextT::stop)  >::value, int>::type = 0>
class SingleThreadExecutor : public Executor
{
public:
    typedef std::thread::id ID;
public:
    SingleThreadExecutor() : _M_ExecutionContext(new ExecutionContextT()) {
        _M_Thread = std::thread([this]() {
            this->run();
        });
    }

    ~SingleThreadExecutor() {
        close();
        delete _M_ExecutionContext;
    }
public:
    void wait() {
        if (isClosed()) {
            return;
        }
        std::unique_lock<std::mutex> Locker(_M_Mutex);
        _M_Condition.wait(Locker);
    }

    void close() {
        _M_Closed.store(true, std::memory_order_relaxed);
        _M_ExecutionContext->stop();

        if (_M_Thread.joinable()) {
            _M_Thread.join();
        }
    }

    bool isClosed() {
        return _M_Closed.load(std::memory_order_acquire);
    }

    ID id() {
      return _M_Thread.get_id();
    }
public:
    virtual void       execute(Task task)    override {
        if (!isClosed()) {
            _M_ExecutionContext->executor()->execute(task);
        }
    }

    virtual CancelFunc execute(Task task, const std::chrono::high_resolution_clock::duration &expiry) {
        if (!isClosed()) {
            return _M_ExecutionContext->executor()->execute(task, expiry);
        }
        return nullptr;
    }

    virtual ExecutionContext* context() {
        return _M_ExecutionContext;
    }

    ExecutionContextT *rawContext() {
        return _M_ExecutionContext;
    }
protected:
    void run() {
        ThreadExecutionContext _selfExecutionContext(*context());

        _M_Closed.store(false, std::memory_order_release);

        for (; !isClosed() ;) {
            try {
                _M_ExecutionContext->start();
            } catch (const std::exception &) {

            } catch (...) {

            }
        }
        _M_Condition.notify_all();
    }

private:
    ExecutionContextT           *_M_ExecutionContext;
    std::thread                 _M_Thread;
    std::atomic_bool            _M_Closed;
    std::mutex                  _M_Mutex;
    std::condition_variable     _M_Condition;
};

using CoreSingleThreadExecutor = SingleThreadExecutor<AsioExecutionContext>;

CALLER_END

#endif // SINGLETHREADEXECUTION_HPP
