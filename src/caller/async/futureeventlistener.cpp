#include <caller/async/futureeventlistener.hpp>
#include <assert.h>

CALLER_BEGIN

FutureEventExecutorListener::FutureEventExecutorListener(Executor *executor) : _M_Executor(executor) {
    assert(executor != nullptr);
}

FutureEventExecutorListenerFunction::FutureEventExecutorListenerFunction(Executor *executor) : FutureEventExecutorListener(executor)
{
    assert(executor != nullptr);
}

CALLER_END
