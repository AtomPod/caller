#ifndef CALLER_ASIO_EXECUTOR_HPP
#define CALLER_ASIO_EXECUTOR_HPP
#include <caller/executor/executor.hpp>
#include <asio.hpp>

CALLER_BEGIN

class AsioExecutionContext;
class AsioExecutor : public Executor
{
    friend class AsioExecutionContext;
protected:
    AsioExecutor(AsioExecutionContext &context);
public:
    virtual ~AsioExecutor()             override;
public:
    virtual void       execute(Task task)     override;
    virtual CancelFunc execute(Task task, const std::chrono::high_resolution_clock::duration &expiry) override;
    virtual ExecutionContext* context() override;
private:
    AsioExecutionContext &_M_Context;
};

class AsioExecutionContext : public ExecutionContext
{
    typedef ::asio::io_context::work    AsioIOWork;
public:
    typedef ::asio::io_context          AsioIOContext;
public:
    AsioExecutionContext();
    virtual ~AsioExecutionContext();
public:
    virtual Executor*   executor();
public:
    AsioExecutor*       asioExecutor();
    AsioIOContext&      rawContext();
    operator            AsioIOContext&() { return this->_M_Context; }
public:
    void                start();
    void                stop();
private:
    AsioIOContext _M_Context;
    AsioExecutor  _M_Executor;
    AsioIOWork    _M_Worker;
};

CALLER_END

#endif // CALLER_BASE_EXECUTOR_HPP
