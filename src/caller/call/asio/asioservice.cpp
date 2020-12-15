#include <caller/call/asio/asioservice.hpp>
#include <caller/message/messagefactory.hpp>
#include <caller/route/router.hpp>
#include <memory>

CALLER_BEGIN

AsioService::AsioService(AsioExecutionContext &context, const Config *config)
    : UnderlyingService(config), _M_IOContext(context), _M_Config(config)
{

}

AsioService::~AsioService()
{

}

bool caller::AsioService::start()
{
    return true;
}

bool caller::AsioService::stop()
{
    return true;
}

Service::CancelFunc AsioService::delay(const Service::duration &expiryTime, const Service::DelayFunc &func)
{
    AsioExecutionContext::AsioIOContext& asioContext = _M_IOContext.rawContext();
    if (func == nullptr || asioContext.stopped())
        return nullptr;

    std::shared_ptr<asio::steady_timer> timer = std::make_shared<asio::steady_timer>(asioContext);
    timer->expires_from_now(expiryTime);

    this->post([timer, func]() {
        timer->async_wait([timer, func](std::error_code e) {
            func(e);
        });
    });

    return [timer, this]() {
        this->post([timer]() {
            timer->cancel();
        });
    };
}

void AsioService::post(const Service::PostFunc &task)
{
    AsioExecutionContext::AsioIOContext& asioContext = _M_IOContext.rawContext();
    if (task == nullptr || asioContext.stopped()) {
        return;
    }

    asioContext.post(task);
}

const AsioService::Config *AsioService::config() const
{
    return _M_Config;
}

CALLER_END
