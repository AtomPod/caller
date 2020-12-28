#include <caller/context/context.hpp>
#include <caller/context/cancelablecontext.hpp>
#include <caller/context/valuecontext.hpp>
#include <caller/async/futureeventlistener.hpp>
#include <cassert>

CALLER_BEGIN

ContextBase::ContextBase()
{

}

ContextBase::~ContextBase()
{

}

Context::Error ContextBase::Err()
{
    std::unique_lock<std::mutex> locker(_M_Mutex);
    return _M_Err;
}

Context::ValueType ContextBase::Value(const Context::KeyType &key)
{
    auto p = parent();
    if (p != nullptr) {
        return p->Value(key);
    }
    return nullptr;
}

Context::OptionalTime ContextBase::Deadline()
{
    return ::std::make_optional<Time>();
}

Context::Channel ContextBase::Done()
{
    return _M_DoneSender.future();
}

void ContextBase::setError(const Error &err)
{
    {
        std::unique_lock<std::mutex> locker(_M_Mutex);
        _M_Err = err;
    }
    _M_DoneSender.reportResult(err);
}

ContextPtr ContextBase::parent()
{
    return _M_Parent.lock();
}

void ContextBase::addChild(ContextPtr child)
{
    {
        std::unique_lock<std::mutex> locker(_M_Mutex);
        _M_Childrens.insert(child);
    }
}

void ContextBase::removeChild(ContextPtr child)
{
    std::unique_lock<std::mutex> locker(_M_Mutex);
    _M_Childrens.erase(child);
}

void ContextBase::setParent(const ContextPtr &parent)
{
    assert(parent != nullptr);

    auto    self = std::static_pointer_cast<ContextBase>(shared_from_this());

    _M_Parent = parent;
    parent->addChild(self);

    Channel pdone = parent->Done();

    _M_ParentChannelListener = CreateFutureEventListenerFunction([self](const FutureEvent &e) {
        if (e.isFinished()) {
            self->setError(*e.resultPtr<Error>());
        }
    });

    pdone.addListener(_M_ParentChannelListener);

    Channel done = Done();
    done.addListener(CreateFutureEventListenerFunction([self, parent, pdone](const FutureEvent &e) mutable {
        UNUSED(e);
        parent->removeChild(self);

        auto listener = self->_M_ParentChannelListener;
        pdone.removeListener(listener);
    }));
}

ContextPtr Context::backgroundContext()
{
    static ContextBase::ContextBasePtr  background =
            NewRefPtr<ContextBase>();
    return background;
}

ContextPtr Context::withValue(ContextPtr parent, const Context::KeyType &key, const Context::ValueType &value)
{
    auto valueContext = NewRefPtr<ValueContext>(key, value);
    valueContext->setParent(parent);
    return std::move(valueContext);
}

Context::ContextCancelPair Context::withCancel(ContextPtr parent)
{
    assert(parent != nullptr);
    auto cancelableContext = NewRefPtr<CancelableContext>();
    cancelableContext->setParent(parent);
    return ContextCancelPair({
        cancelableContext,
        [cancelableContext]() {
             cancelableContext->cancel();
        }
    });
}

CALLER_END
