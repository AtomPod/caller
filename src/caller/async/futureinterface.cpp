#include <caller/async/futureinterface.hpp>
#include <caller/async/futureinterfacebaseimpl.hpp>

CALLER_BEGIN

FutureInterfaceBase::FutureInterfaceBase(size_t storeSpace)
    : _M_Ptr(NewRefPtr<FutureInterfaceBaseImpl>(storeSpace)) {

}

FutureInterfaceBase::~FutureInterfaceBase()
{

}

void FutureInterfaceBase::reportErrorCode(const Error &errorcode)
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->m_state.load(std::memory_order_relaxed) & (Finished | Canceled)) {
        return;
    }

    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Canceled)) {
        _M_Ptr->m_errorCode = errorcode;
        _M_Ptr->m_waitCondition.notify_all();

        FutureEvent event(FutureEvent::Canceled);
        event.setErrorCode(errorcode);
        _M_Ptr->sendEvent(event, &locker);
    }
}

void FutureInterfaceBase::reportException(const std::exception_ptr &e)
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->m_state.load(std::memory_order_relaxed) & (Finished | Canceled)) {
        return;
    }

    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Canceled)) {
        _M_Ptr->m_exceptionPtr = e;
        _M_Ptr->m_waitCondition.notify_all();

        FutureEvent event(FutureEvent::Canceled);
        event.setExceptionPtr(e);
        _M_Ptr->sendEvent(event, &locker);
    }
}

void FutureInterfaceBase::reportFinished()
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Finished)) {
        _M_Ptr->m_waitCondition.notify_all();

        FutureEvent event(FutureEvent::Finished);
        event.setStorage(_M_Ptr->m_storeData);
        _M_Ptr->sendEvent(event, &locker);
    }
}

void FutureInterfaceBase::reportCanceled()
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Canceled)) {
        _M_Ptr->m_waitCondition.notify_all();

        FutureEvent event(FutureEvent::Canceled);
        _M_Ptr->sendEvent(event, &locker);
    }
}

void FutureInterfaceBase::addListener(RefPtr<FutureEventListener> listener)
{
    _M_Ptr->addListener(listener);
}

void FutureInterfaceBase::removeListener(RefPtr<FutureEventListener> listener)
{
    _M_Ptr->removeListener(listener);
}

void FutureInterfaceBase::wait()
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (!isPending()) {
        return;
    }
    _M_Ptr->m_waitCondition.wait(locker);
}

bool FutureInterfaceBase::isPending() const
{
    return queryState(Pending);
}

bool FutureInterfaceBase::isFinished() const
{
    return queryState(Finished);
}

bool FutureInterfaceBase::isCanceled() const
{
    return queryState(Canceled);
}

std::error_code FutureInterfaceBase::errorCode() const
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    return _M_Ptr->m_errorCode;
}

void *FutureInterfaceBase::store() const
{
    return _M_Ptr->m_storeData == nullptr ? nullptr : &(*_M_Ptr->m_storeData);
}

std::exception_ptr FutureInterfaceBase::exception() const
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    return _M_Ptr->m_exceptionPtr;
}

std::mutex &FutureInterfaceBase::mutex() const
{
    return _M_Ptr->m_mutex;
}

bool FutureInterfaceBase::queryState(int state) const
{
    return _M_Ptr->m_state.load(std::memory_order_relaxed) & state;
}

FutureEvent::EventType FutureEvent::event() const
{
    return _M_Event;
}

void FutureEvent::setEvent(const EventType &M_Event)
{
    _M_Event = M_Event;
}

FutureEvent::Storage FutureEvent::storage() const
{
    return _M_Storage;
}

void FutureEvent::setStorage(const caller::FutureEvent::Storage &storage)
{
    _M_Storage = storage;
}

Error FutureEvent::errorCode() const
{
    return _M_ErrorCode;
}

void FutureEvent::setErrorCode(const Error &M_ErrorCode)
{
    _M_ErrorCode = M_ErrorCode;
}

std::exception_ptr FutureEvent::exceptionPtr() const
{
    return _M_ExceptionPtr;
}

void FutureEvent::setExceptionPtr(const std::exception_ptr &M_ExceptionPtr)
{
    _M_ExceptionPtr = M_ExceptionPtr;
}

CALLER_END
