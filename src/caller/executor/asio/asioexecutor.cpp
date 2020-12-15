#include <caller/executor/asio/asioexecutor.hpp>
#include <iostream>
#include <thread>

CALLER_BEGIN

AsioExecutor::AsioExecutor(AsioExecutionContext &context) : _M_Context(context)
{

}

void AsioExecutor::execute(Task task) {
    _M_Context.rawContext().post(task);
}

AsioExecutor::~AsioExecutor()
{

}

ExecutionContext *AsioExecutor::context()
{
    return &_M_Context;
}

AsioExecutionContext::AsioExecutionContext() : _M_Executor(*this), _M_Worker(_M_Context)
{

}

AsioExecutionContext::~AsioExecutionContext()
{

}

Executor *AsioExecutionContext::executor()
{
    return &_M_Executor;
}

AsioExecutor *AsioExecutionContext::asioExecutor()
{
    return &_M_Executor;
}

AsioExecutionContext::AsioIOContext &AsioExecutionContext::rawContext()
{
    return _M_Context;
}

void AsioExecutionContext::start()
{
    _M_Context.run();
}

void AsioExecutionContext::stop()
{
    _M_Context.stop();
}

CALLER_END
