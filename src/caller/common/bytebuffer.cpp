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

class ByteStorage
{
    friend class ByteBuffer;
public:
    ByteStorage(size_t size = 0);
    ByteStorage(char *data, size_t size, ByteBuffer::FreeHandler handler = nullptr);
    ~ByteStorage();

public:
    char*       data() { return _M_Data; }
    const char *data() const { return _M_Data; }
    size_t      capacity() const;
private:
    char                       *_M_Data;
    size_t                      _M_Capacity;
    ByteBuffer::FreeHandler     _M_FreeHandler;
    bool                        _M_External;
};


ByteStorage::ByteStorage(size_t size)
    : _M_Data(nullptr),/* _M_WriteIndex(0),*/ _M_Capacity(0), _M_FreeHandler(nullptr), _M_External(false)
{
    if (size > 0) {
        _M_Capacity  = size;
        _M_Data      = new char[size]();
    }
}

ByteStorage::ByteStorage(char *data, size_t size, ByteBuffer::FreeHandler handler)
    : _M_Data(data), _M_Capacity(size), _M_FreeHandler(handler), _M_External(true)
{

}

ByteStorage::~ByteStorage()
{
    if (_M_External) {
        if (_M_FreeHandler) {
            _M_FreeHandler(_M_Data, _M_Capacity);
        }
    } else {
        delete[] _M_Data;
    }
}


size_t ByteStorage::capacity() const
{
    return _M_Capacity;
}

ByteBuffer::ByteBuffer(size_t size) : _M_Impl(NewRefPtr<ByteStorage>(size)),
    _M_Offset(0), _M_WriteIndex(0), _M_ReadIndex(0), _M_Capacity(size)
{

}

ByteBuffer::ByteBuffer(char *data, size_t size, ByteBuffer::FreeHandler handler) :
    _M_Impl(NewRefPtr<ByteStorage>(data, size, handler)),
    _M_Offset(0), _M_WriteIndex(0), _M_ReadIndex(0), _M_Capacity(size)
{

}

ByteBuffer::~ByteBuffer()
{

}

bool ByteBuffer::take(StringView &view, size_t size, Endian endian) const
{
    (void)endian;
    size_t markReadIndex = readIndex();
    size_t fromIndex     = markReadIndex;
    size_t need          = (size == 0 ? sizeof(uint16_t) : size);
    if (need > readableLength()) {
        return false;
    }

    if (size == 0) {
        uint16_t rsize = 0;
        if (!take<uint16_t>(rsize, sizeof(uint16_t), endian)) {
            return false;
        }
        fromIndex += sizeof(uint16_t);
        size       = rsize;
    }

    if (size > readableLength()) {
        return false;
    }

    const char *raw = reinterpret_cast<const char*>(data() + fromIndex);
    ::std::memmove(const_cast<char*>(view.data()), raw, size);
//    view = StringView(raw, size);
    constSetReadIndex(fromIndex + size);
    return true;
}

bool ByteBuffer::takeUTF8(StringView &view, size_t size, Endian endian) const
{
    if (!take(view, size, endian)) {
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

    ByteBuffer sub = *this;
    sub.setReadIndex(0);
    sub.setWriteIndex(0);
    sub._M_Offset   = offset + _M_Offset;
    sub._M_Capacity = length;

    if (resize && offset + length > writeIndex()) {
        setWriteIndex(offset + length);
    }
    return sub;
}

char *ByteBuffer::data() {
    assert(_M_Impl != nullptr);
    return _M_Impl->data() + _M_Offset;
}

const char *ByteBuffer::data() const {
    assert(_M_Impl != nullptr);
    return _M_Impl->data() + _M_Offset;
}

size_t ByteBuffer::readableLength() const {
    return _M_WriteIndex - _M_ReadIndex;
}

size_t ByteBuffer::capacity() const
{
    assert(_M_Impl != nullptr);
    return _M_Capacity;
}

bool ByteBuffer::resizable() const
{
    assert(_M_Impl != nullptr);
    return false;
}

size_t ByteBuffer::writeIndex() const
{
    return _M_WriteIndex;
}

bool ByteBuffer::setWriteIndex(const size_t &writeIndex)
{
    if (writeIndex < readIndex()) {
        return false;
    }
    _M_WriteIndex = writeIndex;
    return true;
}

size_t ByteBuffer::readIndex() const
{
    return _M_ReadIndex;
}

bool ByteBuffer::setReadIndex(const size_t &readIndex)
{
    assert(_M_Impl != nullptr);
    if (readIndex > this->writeIndex()) {
        return false;
    }

    _M_ReadIndex = readIndex;
    return true;
}

bool ByteBuffer::constSetReadIndex(const size_t &readIndex) const
{
    assert(_M_Impl != nullptr);
    if (readIndex > this->writeIndex()) {
        return false;
    }

    _M_ReadIndex = readIndex;
    return true;
}

size_t ByteBuffer::writableLength() const
{
    return _M_Capacity - _M_WriteIndex;
}

void ByteBuffer::reset()
{
    _M_WriteIndex = 0;
    _M_ReadIndex  = 0;
}

bool ByteBuffer::setReadableLength(const size_t &len)
{
    size_t r = readableLength();
    if (r >= len) {
        return true;
    }

    size_t d  = len - r;
    if (writableLength() >= d) {
        setWriteIndex(writeIndex() + d);
        return true;
    }
    return false;
}

size_t ByteBuffer::copy(const char *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return 0;
    }

    size_t  copyBytes   = size;
    char    *space      = takeSpace(size);

    if (space == nullptr) {
        return 0;
    }

    ::std::memmove(space, data, copyBytes);
    setWriteIndex(writeIndex() + size);
    return copyBytes;
}

char *ByteBuffer::takeSpace(size_t size)
{
    if (writableLength() < size) {
        return nullptr;
    }
    return this->data() + writeIndex();
}

bool ByteBuffer::writeData(const void *data, size_t size)
{
    char *space = takeSpace(size);
    if (space == nullptr) {
        return false;
    }

    ::std::memmove(space, data, size);
    setWriteIndex(writeIndex() + size);
    return true;
}

size_t ByteBuffer::mapToAbsoluteIndex(size_t index) const
{
    return index + _M_Offset;
}

size_t ByteBuffer::absoluteReadIndex() const
{
    return mapToAbsoluteIndex(_M_ReadIndex);
}

size_t ByteBuffer::absoluteWriteIndex() const
{
    return mapToAbsoluteIndex(_M_WriteIndex);
}

CALLER_END
