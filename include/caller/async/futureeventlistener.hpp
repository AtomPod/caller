#ifndef CALLER_FUTUREEVENTLISTENER_HPP
#define CALLER_FUTUREEVENTLISTENER_HPP

#include <caller/async/futureinterface.hpp>
#include <caller/executor/executor.hpp>

CALLER_BEGIN

class FutureEventExecutorListener : public FutureEventListener, public std::enable_shared_from_this<FutureEventExecutorListener>
{
public:
    FutureEventExecutorListener(Executor *executor);

public:
    virtual void postExecutorEvent(const FutureEvent &) = 0;

protected:
    virtual void postEvent(const FutureEvent &e) override {
        FutureEvent copiedEvent = e;
        auto _self = shared_from_this();
        _M_Executor->execute([copiedEvent, _self]() {
            _self->postExecutorEvent(copiedEvent);
        });
    }
protected:
    Executor *executor() { return _M_Executor; }
private:
    Executor *_M_Executor;
};

template<typename Invokable, typename std::enable_if< std::is_function<Invokable>::value
                                        || std::is_member_function_pointer<decltype (&Invokable::operator())>::value, size_t >::type = 0>
class FutureEventListenerFunction : public FutureEventListener
{
public:
    FutureEventListenerFunction(const Invokable &handler) : _M_Handler(handler) {

    }

    FutureEventListenerFunction(Invokable &&handler) : _M_Handler(handler) {

    }
public:
    virtual void postEvent(const FutureEvent &e) override {
        _M_Handler(e);
    }
private:
    Invokable _M_Handler;
};

class FutureEventExecutorListenerFunction : public FutureEventExecutorListener {
public:
    typedef std::function<void(Executor *, const FutureEvent &)> Handler;
public:
    FutureEventExecutorListenerFunction(Executor *executor);
protected:
    virtual void postExecutorEvent(const FutureEvent &e) override {
        _M_Handler(executor(), e);
    }
private:
    Handler _M_Handler;
};

template<typename Invokable>
inline RefPtr< FutureEventListenerFunction<Invokable> > CreateFutureEventListenerFunction(const Invokable &handle) {
    return NewRefPtr< FutureEventListenerFunction<Invokable> >(handle);
}

template<typename Invokable>
inline RefPtr< FutureEventListenerFunction<Invokable> > CreateFutureEventListenerFunction(Invokable &&handle) {
    return NewRefPtr< FutureEventListenerFunction<Invokable> >(std::move(handle));
}


CALLER_END

#endif // FUTUREEVENTLISTENER_HPP
