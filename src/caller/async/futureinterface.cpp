#include <caller/async/futureinterface.hpp>
#include <caller/async/futureinterfacebaseimpl.hpp>

CALLER_BEGIN

FutureInterfaceBase::FutureInterfaceBase(size_t storeSpace)
    : _M_Ptr(NewRefPtr<FutureInterfaceBaseImpl>(storeSpace)) {

}

FutureInterfaceBase::~FutureInterfaceBase()
{

}

void FutureInterfaceBase::reportErrorCode(const std::error_code &errorcode)
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->m_state.load(std::memory_order_relaxed) & (Finished | Canceled)) {
        return;
    }

    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Canceled)) {
        _M_Ptr->m_errorCode = errorcode;
        _M_Ptr->m_waitCondition.notify_all();
        _M_Ptr->sendCallOut(FutureInterfaceBase::Canceled, &locker);
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
        _M_Ptr->sendCallOut(FutureInterfaceBase::Canceled, &locker);
    }
}

void FutureInterfaceBase::reportFinished()
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Finished)) {
        _M_Ptr->m_waitCondition.notify_all();
        _M_Ptr->sendCallOut(FutureInterfaceBase::Finished, &locker);
    }
}

void FutureInterfaceBase::reportCanceled()
{
    FutureInterfaceBaseImpl::Locker locker(_M_Ptr->m_mutex);
    if (_M_Ptr->switchStateTo(FutureInterfaceBase::Canceled)) {
        _M_Ptr->m_waitCondition.notify_all();
        _M_Ptr->sendCallOut(FutureInterfaceBase::Canceled, &locker);
    }
}

FutureInterfaceBase::CancelFunc FutureInterfaceBase::listen(const FutureInterfaceBase::CallOut &out)
{
    return _M_Ptr->connectCallOut(out);
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
    return _M_Ptr->m_storeData;
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

CALLER_END
