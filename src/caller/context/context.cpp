#include <caller/context/context.hpp>
#include <caller/context/cancelablecontext.hpp>
#include <caller/context/valuecontext.hpp>
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
    pdone.whenFinished([self](Error err) {
        self->setError(err);
    }, &_M_ParentChannelCancel);

    Channel done = Done();
    done.whenFinished([self, parent](Error err) {
        UNUSED(err);
        parent->removeChild(self);

        auto cancel = self->_M_ParentChannelCancel;
        if (cancel != nullptr) {
            cancel();
        }
    });
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

//Context::Time Context::now()
//{
//    return Clock::now();
//}

//Context::Time Context::fromNow(const Context::Duration &d)
//{
//    return Clock::now() + d;
//}

//Context::Context(ContextPtr ptr) : _M_Parent(ptr) {

//}

//Context::~Context()
//{

//}

//void Context::setMetadata(const Context::Key &key, const Context::Value &value)
//{
//    _M_Metadata.insert(std::make_pair(key, value));
//}

//Context::Value Context::metadata(const Context::Key &key, bool *exsist) const
//{
//    auto dataIt = _M_Metadata.find(key);
//    ContextPtr _parent = parent();

//    if (dataIt != _M_Metadata.end()) {
//        if (exsist != nullptr) {
//            *exsist = true;
//        }
//        return  dataIt->second;
//    } else if (_parent != nullptr){
//        bool _exists = false;
//        Context::Value value = _parent->metadata(key, &_exists);
//        if (_exists) {
//            return value;
//        }
//    }

//    if (exsist != nullptr) {
//        *exsist = false;
//    }

//    return Context::Value();
//}

//bool Context::hasMetadata(const Context::Key &key)
//{
//    auto dataIt = _M_Metadata.find(key);
//    if (dataIt != _M_Metadata.end()) {
//        return true;
//    }

//    ContextPtr _parent = parent();
//    if (_parent != nullptr) {
//        return _parent->hasMetadata(key);
//    }
//    return false;
//}

//bool Context::removeMetadata(const Context::Key &key)
//{
//    return _M_Metadata.erase(key) != 0;
//}

//void Context::setExpireFromNow(const Context::Duration &d)
//{
//    setExpireAt(fromNow(d));
//}

//void Context::setExpireAt(const Context::Time &expireat)
//{
//    _M_ExpireAt = expireat;
//}

//Context::Time Context::expireAt() const
//{
//    return _M_ExpireAt;
//}

//bool Context::isExpired() const
//{
//    ContextPtr _parent = parent();
//    if (_parent != nullptr) {
//       return _parent->isExpired();
//    }

//    if (_M_ExpireAt == Time()) {
//        return false;
//    }

//    Time _now = now();
//    return _now >= _M_ExpireAt;
//}

//void Context::cancel()
//{
//    _M_Canceled.store(true, std::memory_order_release);
//}

//bool Context::isCanceled() const
//{
//    ContextPtr _parent = parent();
//    if (_parent != nullptr) {
//        return _parent->isCanceled();
//    }
//    return _M_Canceled.load(std::memory_order_acquire);
//}

//ContextPtr Context::parent() const
//{
//    return _M_Parent;
//}

//void Context::setParent(const ContextPtr &parent)
//{
//    _M_Parent = parent;
//}

CALLER_END
