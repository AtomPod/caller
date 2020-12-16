#include <caller/async/futureinterfacebaseimpl.hpp>

CALLER_BEGIN

static FutureInterfaceBase::CancelFunc emptyCancelFunc = []() {};

FutureInterfaceBaseImpl::FutureInterfaceBaseImpl(size_t storeSize)
    : m_state(FutureInterfaceBase::Pending), m_exceptionPtr(nullptr),
      m_storeData(nullptr)
{
    if (storeSize > 0) {
        m_storeData = new StorageElement[storeSize];
    }
}

FutureInterfaceBaseImpl::~FutureInterfaceBaseImpl()
{
    delete[] m_storeData;
}

bool FutureInterfaceBaseImpl::switchStateTo(int which) {
    int exp = FutureInterfaceBase::Pending;
    return m_state.compare_exchange_strong(exp, which, std::memory_order_acq_rel);
}

FutureInterfaceBase::CancelFunc FutureInterfaceBaseImpl::connectCallOut(const FutureInterfaceBase::CallOut &o)
{
    if (o == nullptr)
        return nullptr;

    Locker locker(m_mutex);
    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Pending) {
        m_signal.push_back(o);
        auto result  = m_signal.back();
        auto pointer = result.template target<void(*)(int)>();
        auto _self   = shared_from_this();
        return [pointer, _self]() {
            Locker locker(_self->m_mutex);
            for (auto beg = _self->m_signal.begin(); beg != _self->m_signal.end(); ++beg) {
                auto& fn = *beg;
                if (fn.template target<void(*)(int)>() == pointer) {
                    _self->m_signal.erase(beg);
                    return;
                }
            }
        };
    }

    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Canceled) {
        locker.unlock();

        o(FutureInterfaceBase::Canceled);
    }

    if (m_state.load(std::memory_order_relaxed) & FutureInterfaceBase::Finished) {
        locker.unlock();

        o(FutureInterfaceBase::Finished);
    }
    return emptyCancelFunc;
}

void FutureInterfaceBaseImpl::sendCallOut(int state, Locker *locker)
{
    Signal tempSignal;
    tempSignal.swap(m_signal);

    if (locker != nullptr && locker->owns_lock()) {
        locker->unlock();
    }

    for (auto fn : tempSignal) {
        fn(state);
    }
}

CALLER_END
