#include <caller/common/bytebuffer.hpp>
#include <stdlib.h>
#include <assert.h>
#include <cstring>
#include <iostream>

CALLER_BEGIN

ByteBuffer::FreeHandler ByteBuffer::newFreeHandler = [](char *data, size_t size) {
    (void)size;
    delete[] data;
};

ByteBuffer::FreeHandler ByteBuffer::freeFreeHandler = [](char *data, size_t size) {
    (void)size;
    free(data);
};

class ByteBufferImpl
{
    friend class ByteBuffer;
public:
    ByteBufferImpl(size_t size = 0);
    ByteBufferImpl(char *data, size_t size, ByteBuffer::FreeHandler handler = nullptr);
    ~ByteBufferImpl();

public:
    size_t      copy(const char *data, size_t size);
    char*       data() { return _M_Data; }
    const char *data() const { return _M_Data; }
    size_t      length() const { return _M_Len; }
    void        reserve(size_t size);
    void        resize(size_t size);
    size_t      capacity() const;
protected:
    char        *takeSpace(size_t size);
    bool        allocNew(size_t size);
    bool        writeData(const void *data, size_t size);
private:
    char                       *_M_Data;
    size_t                      _M_Len;
    size_t                      _M_Capacity;
    ByteBuffer::FreeHandler     _M_FreeHandler;
    bool                        _M_External;
};


ByteBufferImpl::ByteBufferImpl(size_t size)
    : _M_Data(nullptr), _M_Len(0), _M_Capacity(0), _M_FreeHandler(nullptr), _M_External(false)
{
    if (size > 0) {
        _M_Capacity  = size;
        _M_Data      = new char[size]();
    }
}

ByteBufferImpl::ByteBufferImpl(char *data, size_t size, ByteBuffer::FreeHandler handler)
    : _M_Data(data), _M_Len(0), _M_Capacity(size), _M_FreeHandler(handler), _M_External(true)
{

}

ByteBufferImpl::~ByteBufferImpl()
{
    if (_M_External) {
        if (_M_FreeHandler) {
            _M_FreeHandler(_M_Data, _M_Capacity);
        }
    } else {
        delete[] _M_Data;
    }
}

size_t ByteBufferImpl::copy(const char *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    size_t  copyBytes   = size;
    char    *space      = takeSpace(size);

    if (space == nullptr) {
        copyBytes   = _M_Capacity - _M_Len;
        space       = takeSpace(_M_Capacity);
    }

    if (space == nullptr) {
        return 0;
    }

    ::std::memmove(space, data, copyBytes);
    return copyBytes;
}

void ByteBufferImpl::reserve(size_t size)
{
    allocNew(size);
}

void ByteBufferImpl::resize(size_t size)
{
    if (allocNew(size)) {
        _M_Len = size;
    }
}

size_t ByteBufferImpl::capacity() const
{
    return _M_Capacity;
}

char *ByteBufferImpl::takeSpace(size_t size)
{
    size_t len = _M_Len + size;
    if (len > _M_Capacity) {
        size_t nextDoubleSpace = _M_Capacity << 1;
        if (nextDoubleSpace > len) {
            len = nextDoubleSpace;
        }
        if (!allocNew(len)) {
            return nullptr;
        }
    }

    char *data = _M_Data + _M_Len;
    _M_Len += size;
    return data;
}

bool ByteBufferImpl::allocNew(size_t size)
{
    if (_M_Capacity >= size) {
        return true;
    }

    if (_M_External) {
        return false;
    }

    char* newData = reinterpret_cast<char*>(realloc(_M_Data, size));
    if (newData != nullptr) {
        _M_Data     = newData;
        _M_Capacity = size;
        return true;
    }
    return false;
}

bool ByteBufferImpl::writeData(const void *data, size_t size)
{
    char *space = takeSpace(size);
    if (space == nullptr) {
        return false;
    }

    ::std::memmove(space, data, size);
    return true;
}

ByteBuffer::ByteBuffer(size_t size) : _M_Impl(NewRefPtr<ByteBufferImpl>(size))
{

}

ByteBuffer::ByteBuffer(char *data, size_t size, ByteBuffer::FreeHandler handler) :
    _M_Impl(NewRefPtr<ByteBufferImpl>(data, size, handler))
{

}

ByteBuffer::~ByteBuffer()
{

}

bool ByteBuffer::take(StringView &view, size_t from, size_t size, Endian endian)
{
    (void)endian;

    size_t end = from + size;
    if (end > length()) {
        return false;
    }

    const char *raw = reinterpret_cast<const char*>(data() + from);
    view = StringView(raw, size);
    return true;
}

bool ByteBuffer::takeUTF8(StringView &view, size_t from, size_t size, Endian endian)
{
    if (!take(view, from, size, endian)) {
        return false;
    }
    view.setCode(StringView::Code::UTF8);
    return true;
}

ByteBuffer ByteBuffer::slice(size_t offset, size_t length, bool resize)
{
    size_t newSize = offset + length;
    if (newSize > capacity()) {
        return ByteBuffer();
    }

    ByteBuffer sub(this->data() + offset, length);
    if (resize && newSize > this->length()) {
        this->resize(newSize);
    }
    return sub;
}

char *ByteBuffer::data() {
    assert(_M_Impl != nullptr);
    return _M_Impl->data();
}

const char *ByteBuffer::data() const {
    assert(_M_Impl != nullptr);
    return _M_Impl->data();
}

size_t ByteBuffer::length() const {
    assert(_M_Impl != nullptr);
    return _M_Impl->length();
}

void ByteBuffer::reserve(size_t size)
{
    assert(_M_Impl != nullptr);
    _M_Impl->reserve(size);
}

void ByteBuffer::resize(size_t size)
{
    assert(_M_Impl != nullptr);
    _M_Impl->resize(size);
}

size_t ByteBuffer::copy(const char *data, size_t size)
{
    assert(_M_Impl != nullptr);
    return _M_Impl->copy(data, size);
}

size_t ByteBuffer::capacity() const
{
    assert(_M_Impl != nullptr);
    return _M_Impl->capacity();
}

bool ByteBuffer::resizable() const
{
    assert(_M_Impl != nullptr);
    return !_M_Impl->_M_External;
}

bool ByteBuffer::writeData(const void *data, size_t size)
{
    assert(_M_Impl != nullptr);
    return _M_Impl->writeData(data, size);
}

CALLER_END
