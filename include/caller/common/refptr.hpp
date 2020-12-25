#ifndef CALLER_REFPTR_HPP
#define CALLER_REFPTR_HPP

#include <caller/core.hpp>
#include <memory>

CALLER_BEGIN

template<typename T>
using RefPtr = ::std::shared_ptr<T>;

template<typename T>
using WeakRefPtr = ::std::weak_ptr<T>;

template<typename T, typename... Args>
inline RefPtr<T> NewRefPtr(Args&&... args) {
    return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename Ty1, typename Ty2>
inline RefPtr<Ty1> StaticCastRefPtr(const RefPtr<Ty2> &ptr) {
    return std::static_pointer_cast<Ty1, Ty2>(ptr);
}

template<typename Ty1, typename Ty2>
inline RefPtr<Ty1> ConstCastRefPtr(const RefPtr<Ty2> &ptr) {
    return std::const_pointer_cast<Ty1, Ty2>(ptr);
}

template<typename Ty1, typename Ty2>
inline RefPtr<Ty1> ReinterpretCastRefPtr(const RefPtr<Ty2> &ptr) {
    return std::reinterpret_pointer_cast<Ty1, Ty2>(ptr);
}

template<typename Ty1, typename Ty2>
inline RefPtr<Ty1> DynamicCastRefPtr(const RefPtr<Ty2> &ptr) {
    return std::dynamic_pointer_cast<Ty1, Ty2>(ptr);
}

CALLER_END

#endif // CALLER_REFPTR_HPP
