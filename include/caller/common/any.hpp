#pragma once
#ifndef CALLER_ANY_HPP
#define CALLER_ANY_HPP
#include <caller/core.hpp>
#include <algorithm>

#if __cplusplus >= 201703L
    #include <any>
    #define ANY_NAMESPACE ::std::
#else
    #include <boost/any.hpp>
    #define ANY_NAMESPACE ::boost::
#endif

CALLER_BEGIN

#if __cplusplus >= 201703L
    typedef ::std::any Any;
#else
    typedef boost::any Any;
#endif

template<typename ValueType>
ValueType * AnyCast(Any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline const ValueType * AnyCast(const Any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
ValueType AnyCast(Any & operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline ValueType AnyCast(const Any & operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline ValueType AnyCast(Any&& operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(std::move(operand));
}

template<typename ValueType>
inline ValueType * UnsafeAnyCast(Any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline const ValueType * UnsafeAnyCast(const Any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

class Variant : public Any
{
public:
    using Any::Any;
public:
    template<typename T>
    T convertTo(bool *success = nullptr) const {
        try {
            if (success != nullptr) {
                *success = true;
            }
            return CALLER AnyCast<T>(*this);
        } catch(...) {
            if (success != nullptr) {
                *success = false;
            }
            return T();
        }
    }

    template<typename T>
    T convertTo(bool *success = nullptr) {
        try {
            if (success != nullptr) {
                *success = true;
            }
            return CALLER AnyCast<T>(*this);
        } catch(...) {
            if (success != nullptr) {
                *success = false;
            }
            return T();
        }
    }
};

CALLER_END

#endif // CALLER_ANY_HPP
