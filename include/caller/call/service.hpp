#ifndef CALLER_CALLSERVICE_HPP
#define CALLER_CALLSERVICE_HPP
#include <caller/async/future.hpp>
#include <caller/call/iocontext.hpp>
#include <caller/route/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <list>
#include <chrono>
#include <atomic>

CALLER_BEGIN

class Service
{
public:
    typedef std::chrono::high_resolution_clock::duration duration;
public:
    typedef std::function<void(std::error_code)> DelayFunc;
    typedef std::function<void()> CancelFunc;
    typedef std::function<void()> PostFunc;
public:
    Service();
    virtual ~Service();

public:
    virtual bool                        start() = 0;
    virtual bool                        stop()  = 0;

    // delay  投递一个延迟任务到服务中进行执行，且保证执行在服务所在的线程中，返回一个取消函数，
    // 调用该函数则取消该次任务
    virtual CancelFunc                  delay(const duration &expiryTime, const DelayFunc &func) = 0;

    // post   投递一个任务到服务中进行执行，且保证执行在服务所在的线程中
    virtual void                        post(const PostFunc &task) = 0;

    // invoke 调用传入IOContextPtr与RequestPtr，返回一个Future，当响应时，通过 Future进行响应
    virtual Future<RequestResponsePtr>  invoke(IOContextPtr ctx, RequestPtr request) = 0;

public:
    int  connectivityState() const;

protected:
    void setConnectivityState(const int &connectivityState);

protected:
    virtual void onConnectivityStateChanged(int newState) = 0;
    virtual void onData(ByteBuffer data) = 0;

private:
    std::atomic_int           _M_ConnectivityState;
};

CALLER_END

#endif // CALLER_CALLSERVICE_HPP
