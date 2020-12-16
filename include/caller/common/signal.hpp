#ifndef CALLER_SIGNAL_HPP
#define CALLER_SIGNAL_HPP
#include <caller/core.hpp>

#ifdef __HAVE_SIG_SLOT_LIBRARY_
#include <sigslot/signal.hpp>
#endif

CALLER_BEGIN

#ifdef __HAVE_SIG_SLOT_LIBRARY_

template<typename... T>
using signal = ::sigslot::signal<T...>;

#elif  defined(__HAVE_BOOST_SIGNAL2_)

#endif

CALLER_END

#endif // SIGNAL_HPP
