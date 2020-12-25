#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/context/context.hpp>
#include <asio.hpp>
#include <iostream>
#include <thread>

CALLER_BEGIN

AsioExecutor::AsioExecutor(AsioExecutionContext &context) : _M_Context(context)
{

}

void AsioExecutor::execute(Task task) {
    if (task == nullptr)
        return;

    ::asio::dispatch(_M_Context.rawContext(), std::move(task));
}

AsioExecutor::CancelFunc AsioExecutor::execute(Executor::Task task, const std::chrono::steady_clock::duration &expiry)
{
    AsioExecutionContext::AsioIOContext& context = _M_Context.rawContext();

    std::shared_ptr<::asio::steady_timer> timer  = std::make_shared<::asio::steady_timer>(context);
    timer->expires_from_now(expiry);
    timer->async_wait([task, timer](const std::error_code &e) {
        if (!e) {
            task();
        }
    });

    return [timer]() mutable {
        std::error_code e;
        timer->cancel(e);
    };
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
