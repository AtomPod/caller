#ifndef CALLER_THREADEXECUTIONCONTEXT_HPP
#define CALLER_THREADEXECUTIONCONTEXT_HPP
#include <caller/executor/executor.hpp>

CALLER_BEGIN

class ThreadExecutionContext
{
public:
    ThreadExecutionContext(ExecutionContext &context);
    ~ThreadExecutionContext();

public:
    static inline ExecutionContext *executionContext() { return _M_Current; }

protected:
    static void setExecutionContext(ExecutionContext *context);

private:
    ExecutionContext *_M_Prev;
    static thread_local ExecutionContext *_M_Current;
};

CALLER_END

#endif // THREADEXECUTIONCONTEXT_HPP
