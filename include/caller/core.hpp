#pragma once
#ifndef CALLER_CORE_HPP
#define CALLER_CORE_HPP

#define CALLER_BEGIN namespace caller {
#define CALLER_END   }
#define CALLER ::caller::

#define CALLER_NOEXCEPT noexcept

#ifdef CALLER_BUILD_DLL
#define CALLER_DLL_EXPORT __declspec(dllexport)
#else
#define CALLER_DLL_EXPORT
#endif

#if __cplusplus >= 201703L
#define UNUSED_ATTRIBUTE [[maybe_unused]]
#elif defined(__GNUC__)
#define UNUSED_ATTRIBUTE __attribute__((unused))
#elif defined(_MSC_VER)
#define UNUSED_ATTRIBUTE
#endif

static union {
    char c[4];
    int  i;
} ___endian UNUSED_ATTRIBUTE = {{'l', ' ', ' ', 'b'}};

#define BIG_ENDIAN ('b')
#define LITTLE_ENDIAN ('l')

#define ENDIAN() ((reinterpret_cast<char*>(&___endian.i))[0])

#ifndef UNUSED
#define UNUSED(c) (void)(c)
#endif

#endif // CALLER_CORE_HPP
