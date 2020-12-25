#ifndef CALLER_BYTEBUFFER_HPP
#define CALLER_BYTEBUFFER_HPP
#include <caller/core.hpp>
#include <stdint.h>
#include <string>
#include <memory>
#include <caller/common/endian.hpp>
#include <functional>
#include <iostream>
#include <caller/common/stringview.hpp>
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class ByteBufferImpl;
class CALLER_DLL_EXPORT ByteBuffer
{
public:
    typedef std::function<void(char *data, size_t size)> FreeHandler;
    static FreeHandler newFreeHandler;
    static FreeHandler freeFreeHandler;
protected:
    typedef RefPtr<ByteBufferImpl> Ptr;
public:
    ByteBuffer(size_t size = 0);
    ByteBuffer(char *data, size_t size, FreeHandler handler = nullptr);
    ~ByteBuffer();
public:
    template<typename T>
    bool put(T d, Endian endian = Endian::Big,
             typename std::enable_if< std::is_arithmetic<T>::value, void>::type* = nullptr) {
        d = toEndian(d, endian);
        return writeData(&d, sizeof(d));
    }

    template<typename T>
    bool put(const StringView &view, Endian endian = Endian::Big) {
        return put(static_cast<T>(view.size()), endian) && writeData(view.data(), view.capacity());
    }

    bool putNoSize(const StringView &view, Endian endian = Endian::Big) {
        (void)endian;
        return writeData(view.data(), view.capacity());
    }
public:
    template<typename T>
    bool take(T &d,  size_t from ,size_t size = sizeof(T), Endian endian = Endian::Big,
         typename std::enable_if< std::is_arithmetic<T>::value, void>::type* = nullptr) {
        size_t end  = from + size;

        if (end > length()) {
            return false;
        }

        T *raw = reinterpret_cast<T*>(data() + from);
        d = fromEndian(*raw, endian);
        return true;
    }

    bool take(StringView &view, size_t from , size_t size, Endian endian = Endian::Big);
    bool takeUTF8(StringView &view, size_t from , size_t size, Endian endian = Endian::Big);
public:
    char*       data();
    const char *data() const;
    size_t      length() const;
    void        reserve(size_t size);
    void        resize(size_t size);
    size_t      copy(const char *data, size_t size);
    size_t      capacity() const;
    bool        resizable() const;
protected:
    bool        writeData(const void *data, size_t size);
private:
    Ptr         _M_Impl;
};

CALLER_END

#endif // CALLER_BYTEBUFFER_HPP
