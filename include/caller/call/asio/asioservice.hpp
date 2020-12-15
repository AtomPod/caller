#ifndef CALLER_ASIOSERVICE_HPP
#define CALLER_ASIOSERVICE_HPP
#include <caller/core.hpp>
#include <asio.hpp>
#include <caller/call/underlyingservice.hpp>
#include <caller/message/declare.hpp>
#include <caller/executor/asio/asioexecutor.hpp>

CALLER_BEGIN

class AsioService : public UnderlyingService
{
public:
    class Config : public UnderlyingService::Config {

    };
public:
    AsioService(AsioExecutionContext &context, const Config *config = nullptr);
    virtual ~AsioService() override;
public:
    virtual bool    start() override;
    virtual bool    stop() override;
    // delay  投递一个延迟任务到服务中进行执行，且保证执行在服务所在的线程中，返回一个取消函数，
    // 调用该函数则取消该次任务
    virtual CancelFunc                  delay(const duration &expiryTime, const DelayFunc &func) override;

    // post   投递一个任务到服务中进行执行，且保证执行在服务所在的线程中
    virtual void                        post(const PostFunc &task) override;

public:
    const Config *config() const;

public:
    AsioExecutionContext& context() { return _M_IOContext; }

private:
    AsioExecutionContext        &_M_IOContext;
    const Config                *_M_Config;
};

CALLER_END

#endif // ASIOSERVICE_HPP
