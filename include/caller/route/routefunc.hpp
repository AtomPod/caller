#ifndef ROUTEFUNC_HPP
#define ROUTEFUNC_HPP
#include <caller/route/route.hpp>
#include <caller/route/declare.hpp>
#include <functional>

CALLER_BEGIN

template<typename Invokable>
class RouteFuncInvoker : public Route
{
public:
    RouteFuncInvoker(const Invokable &invoker) : _M_Handler(invoker) {}
    RouteFuncInvoker(Invokable &&invoker) : _M_Handler(std::move(invoker)) {}
    virtual ~RouteFuncInvoker() override = default;
protected:
    virtual void post(const EventPtr &event) override {
        _M_Handler(event);
    }

private:
    Invokable   _M_Handler;
};

template<typename Invokable>
inline RefPtr< RouteFuncInvoker<Invokable> > CreateRouteFuncInvoker(const Invokable &invoker) {
    return NewRefPtr< RouteFuncInvoker<Invokable> >(invoker);
}

template<typename Invokable>
inline RefPtr< RouteFuncInvoker<Invokable> > CreateRouteFuncInvoker(Invokable &&invoker) {
    return NewRefPtr< RouteFuncInvoker<Invokable> >(std::move(invoker));
}



CALLER_END

#endif // ROUTEFUNC_HPP
