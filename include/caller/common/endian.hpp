#ifndef ENDIAN_HPP
#define ENDIAN_HPP
#include <caller/core.hpp>
#include <stdint.h>
#include <type_traits>

CALLER_BEGIN

enum class Endian {
    Little,
    Big
};

Endian nativeEndian();

template<typename T>
T cbswap(T d);

template<>
inline uint8_t cbswap(uint8_t d) {
    return d;
}

template<>
inline uint16_t cbswap(uint16_t d) {
    return uint16_t(0
            |((d & 0xff00) >> 8)
            |((d & 0x00ff) << 8));
}

template<>
inline uint32_t cbswap(uint32_t d) {
    return 0
            |((d & 0xff000000) >> 24)
            |((d & 0x00ff0000) >> 8)
            |((d & 0x0000ff00) << 8)
            |((d & 0x000000ff) << 24);

}

template<>
inline uint64_t cbswap(uint64_t d) {
    return 0
            |((d & uint64_t(0xff00000000000000)) >> 56)
            |((d & uint64_t(0x00ff000000000000)) >> 40)
            |((d & uint64_t(0x0000ff0000000000)) >> 24)
            |((d & uint64_t(0x000000ff00000000)) >> 8)
            |((d & uint64_t(0x00000000000000ff)) << 56)
            |((d & uint64_t(0x000000000000ff00)) << 40)
            |((d & uint64_t(0x0000000000ff0000)) << 24)
            |((d & uint64_t(0x00000000ff000000)) << 8);
}

template<>
inline int8_t cbswap(int8_t d) {
    return d;
}

template<>
inline int16_t cbswap(int16_t d) {
    return static_cast<int16_t>(cbswap(uint16_t(d)));
}

template<>
inline int32_t cbswap(int32_t d) {
    return static_cast<int32_t>(cbswap(uint32_t(d)));
}

template<>
inline int64_t cbswap(int64_t d) {
    return static_cast<int64_t>(cbswap(uint64_t(d)));
}

template<typename T>
T toBigEndian(T d) {
    if (nativeEndian() != Endian::Big) {
        return cbswap<T>(d);
    }
    return d;
}

template<typename T>
T toLittleEndian(T d) {
    if (nativeEndian() != Endian::Little) {
        return cbswap<T>(d);
    }
    return d;
}

template<typename T>
T toEndian(T d, Endian endian) {
    if (endian == Endian::Big) {
        return toBigEndian(d);
    }
    return toLittleEndian(d);
}

template<typename T>
T fromBigEndian(T d) {
    if (nativeEndian() != Endian::Big) {
        return cbswap<T>(d);
    }
    return d;
}

template<typename T>
T fromLittleEndian(T d) {
    if (nativeEndian() != Endian::Little) {
        return cbswap<T>(d);
    }
    return d;
}

template<typename T>
T fromEndian(T d, Endian endian) {
    if (endian == Endian::Big) {
        return fromBigEndian(d);
    }
    return fromLittleEndian(d);
}

CALLER_END

#endif // ENDIAN_HPP
