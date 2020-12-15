#ifndef CALLER_BASE_EXECUTOR_HPP
#define CALLER_BASE_EXECUTOR_HPP
#include <caller/core.hpp>
#include <functional>
#include <memory>

CALLER_BEGIN

class ExecutionContext;
class Executor : public std::enable_shared_from_this<Executor>
{
public:
    typedef std::function<void()> Task;
public:
    Executor();
    virtual ~Executor();
public:
    virtual void execute(Task task)     = 0;
    virtual ExecutionContext* context() = 0;
};

class ExecutionContext
{
public:
    ExecutionContext();
    virtual ~ExecutionContext();
public:
    virtual Executor* executor() = 0;
};

CALLER_END

#endif // CALLER_BASE_EXECUTOR_HPP
