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
    typedef ::std::any any;
#else
    typedef boost::any any;
#endif

template<typename ValueType>
ValueType * any_cast(any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline const ValueType * any_cast(const any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
ValueType any_cast(any & operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline ValueType any_cast(const any & operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline ValueType any_cast(any&& operand)
{
    return ANY_NAMESPACE any_cast<ValueType>(std::move(operand));
}

template<typename ValueType>
inline ValueType * unsafe_any_cast(any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

template<typename ValueType>
inline const ValueType * unsafe_any_cast(const any * operand) CALLER_NOEXCEPT
{
    return ANY_NAMESPACE any_cast<ValueType>(operand);
}

class Variant : public any
{
public:
    using any::any;
public:
    template<typename T>
    T convertTo(bool *success = nullptr) const {
        try {
            if (success != nullptr) {
                *success = true;
            }
            return CALLER any_cast<T>(*this);
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
            return CALLER any_cast<T>(*this);
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
