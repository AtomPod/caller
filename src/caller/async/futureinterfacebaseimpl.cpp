#include <caller/async/futureinterfacebaseimpl.hpp>

CALLER_BEGIN

static FutureInterfaceBase::CancelFunc emptyCancelFunc = []() {};

FutureInterfaceBaseImpl::FutureInterfaceBaseImpl(size_t storeSize)
    : m_state(FutureInterfaceBase::Pending), m_exceptionPtr(nullptr),
      m_storeData(nullptr)
{
    if (storeSize > 0) {
        m_storeData = RefPtr<StorageElement>(new StorageElement[storeSize], [](StorageElement *p) { delete[] p; });
    }
}

FutureInterfaceBaseImpl::~FutureInterfaceBaseImpl()
{

}

bool FutureInterfaceBaseImpl::switchStateTo(int which) {
    int exp = FutureInterfaceBase::Pending;
    return m_state.compare_exchange_strong(exp, which, std::memory_order_acq_rel);
}

void FutureInterfaceBaseImpl::addListener(RefPtr<FutureEventListener> listener)
{
    if (listener == nullptr)
        return;

    Locker locker(m_mutex);
    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Pending) {
        m_signal.push_back(listener);
        return;
    }

    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Canceled) {
        locker.unlock();

        FutureEvent e(FutureEvent::Canceled);
        e.setErrorCode(m_errorCode);
        e.setExceptionPtr(m_exceptionPtr);
        listener->postEvent(e);
    }

    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Finished) {
        locker.unlock();

        FutureEvent e(FutureEvent::Finished);
        e.setStorage(m_storeData);
        listener->postEvent(e);
    }
}

void FutureInterfaceBaseImpl::removeListener(RefPtr<FutureEventListener> listener)
{
    if (listener == nullptr)
        return;

    Locker locker(m_mutex);
    m_signal.remove(listener);
}

void FutureInterfaceBaseImpl::sendEvent(const FutureEvent &event, Locker *locker)
{
    Signal tempSignal;
    tempSignal.swap(m_signal);

    if (locker != nullptr && locker->owns_lock()) {
        locker->unlock();
    }

    for (auto listener : tempSignal) {
        listener->postEvent(event);
    }
}

CALLER_END
