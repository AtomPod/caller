#include <caller/common/ringbuffer.hpp>
#include <cstring>
#include <algorithm>

CALLER_BEGIN

RingBuffer::RingBuffer(size_t size) : _M_Datas(size), _M_Header(0), _M_Tailer(0)
{

}

RingBuffer::~RingBuffer()
{

}

size_t RingBuffer::put(const RingBuffer::Element *datas, size_t len)
{
    size_t copysize = copyFrom(datas, len);
    _M_Tailer   += copysize;
    return copysize;
}

size_t RingBuffer::take(RingBuffer::Element *datas, size_t len)
{
    size_t copysize = copyTo(datas, len, 0);
    _M_Header += copysize;
    return copysize;
}

size_t RingBuffer::length() const
{
    return _M_Tailer - _M_Header;
}

size_t RingBuffer::capacity() const
{
    return _M_Datas.size();
}

bool RingBuffer::move(uint32_t off)
{
    if (off > length()) {
        return false;
    }

    _M_Header = _M_Header + off;
    return true;
}

size_t RingBuffer::copyTo(RingBuffer::Element *datas, size_t len, size_t off) const
{
    if (datas == nullptr || len == 0) {
        return 0;
    }

    if (off >= length()) {
        return 0;
    }

    size_t copysize = std::min(len, length() - off);

    size_t start = (_M_Header + off) % _M_Datas.size();
    size_t end   = start + copysize;
    size_t tail  = std::min(end, _M_Datas.size());

    size_t head  = 0;
    size_t headTo= static_cast<size_t>(std::max(int64_t(end - tail), int64_t(0)));

    const Element *rawdata = _M_Datas.data();
    ::std::memmove(datas, &rawdata[start], tail - start);
    ::std::memmove(datas + (tail - start), &rawdata[head], headTo - head);

    return copysize;
}

size_t RingBuffer::copyFrom(const RingBuffer::Element *datas, size_t len)
{
    if (datas == nullptr || len == 0) {
        return 0;
    }

    size_t copysize = std::min(len, capacity() - length());
    if (copysize == 0)
        return 0;

    size_t start = _M_Tailer % _M_Datas.size();
    size_t end   = start + copysize;
    size_t tail  = std::min(end, _M_Datas.size());

    size_t head  = 0;
    size_t headTo= static_cast<size_t>(std::max(int64_t(end - tail), int64_t(0)));

    Element *rawdata = _M_Datas.data();
    ::std::memmove(&rawdata[start], datas, tail - start);
    ::std::memmove(&rawdata[head],  datas + (tail - start), headTo - head);
    return copysize;
}

size_t RingBuffer::readIndex() const
{
    return _M_Header;
}

void RingBuffer::setReadIndex(const size_t &M_Header)
{
    _M_Header = M_Header;
}

size_t RingBuffer::writeIndex() const
{
    return _M_Tailer;
}

void RingBuffer::setWriteIndex(const size_t &M_Tailer)
{
    _M_Tailer = M_Tailer;
}

CALLER_END
