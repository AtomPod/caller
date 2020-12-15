#include <caller/executor/threadexecutioncontext.hpp>

CALLER_BEGIN


//class ExecutionContext;
//class Executor : public std::enable_shared_from_this<Executor>
//{
//public:
//    typedef std::function<void()> Task;
//public:
//    Executor();
//    virtual ~Executor();
//public:
//    virtual void execute(Task task)     = 0;
//    virtual ExecutionContext* context() = 0;
//};

//class ExecutionContext
//{
//public:
//    ExecutionContext();
//    virtual ~ExecutionContext();
//public:
//    virtual Executor* executor() = 0;
//};

thread_local         ExecutionContext *ThreadExecutionContext::_M_Current = nullptr;

ThreadExecutionContext::ThreadExecutionContext(caller::ExecutionContext &context)
{
    _M_Prev = executionContext();
    setExecutionContext(&context);
}

ThreadExecutionContext::~ThreadExecutionContext()
{
    setExecutionContext(_M_Prev);
}

ExecutionContext *ThreadExecutionContext::executionContext()
{
    return _M_Current;
}

void ThreadExecutionContext::setExecutionContext(ExecutionContext *context)
{
    _M_Current = context;
}


CALLER_END
