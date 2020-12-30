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

class ByteStorage;
class CALLER_DLL_EXPORT ByteBuffer
{
public:
    class Index {
        friend class ByteBuffer;
    public:
        Index() : _M_Index(0) {}
        Index(size_t index) : _M_Index(index) {}
    public:
        operator size_t() const { return _M_Index; }
    public:
        size_t index()   const  { return _M_Index; }
    protected:
        void setIndex(const size_t &index) {
            _M_Index = index;
        }
    private:
        size_t _M_Index;
    };
public:
    typedef std::function<void(char *data, size_t size)> FreeHandler;
    static FreeHandler newFreeHandler;
    static FreeHandler freeFreeHandler;
protected:
    typedef RefPtr<ByteStorage> Ptr;
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

    bool put(const StringView &view, Endian endian = Endian::Big) {
        UNUSED(endian);
        return put(static_cast<uint16_t>(view.size()), endian) && writeData(view.data(), view.capacity());
    }

public:
    template<typename T>
    bool take(T &d,  size_t size = sizeof(T), Endian endian = Endian::Big,
         typename std::enable_if< std::is_arithmetic<T>::value, void>::type* = nullptr) const {
        if (size > readableLength()) {
            return false;
        }

        const T *raw = reinterpret_cast<const T*>(data() + readIndex());
        d = fromEndian(*raw, endian);
        constSetReadIndex(readIndex() + size);
        return true;
    }

    bool take(StringView &view, size_t size = 0, Endian endian = Endian::Big) const;
    bool takeUTF8(StringView &view, size_t size = 0, Endian endian = Endian::Big) const;
public:
    // 不安全函数，使用时请保证父级别的ByteBuffer不会被释放，否则会造成崩溃
    ByteBuffer  slice(size_t offset, size_t readableLength, bool resize = true);
public:
    char*       data();
    const char *data() const;
    size_t      readableLength() const;
    size_t      writableLength() const;
    void        reset();

    bool        setReadableLength(const size_t &len);

    size_t      copy(const char *data, size_t size);
    size_t      capacity() const;
    bool        resizable() const;

    size_t      writeIndex() const;
    bool        setWriteIndex(const size_t &writeIndex);

    size_t      readIndex() const;
    bool        setReadIndex(const size_t &readIndex);

protected:
    bool        constSetReadIndex(const size_t &readIndex) const;

    bool        writeData(const void *data, size_t size);

    size_t      absoluteWriteIndex() const;
    size_t      absoluteReadIndex()  const;

    size_t      mapToAbsoluteIndex(size_t index) const;
protected:
    char        *takeSpace(size_t size);

private:
    Ptr         _M_Impl;
    size_t      _M_Offset;
    size_t      _M_WriteIndex;
    mutable     size_t      _M_ReadIndex;
    size_t      _M_Capacity;
};

CALLER_END

#endif // CALLER_BYTEBUFFER_HPP
