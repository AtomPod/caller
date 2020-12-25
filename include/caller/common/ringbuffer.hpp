#ifndef CALLER_RINGBUFFER_HPP
#define CALLER_RINGBUFFER_HPP
#include <caller/core.hpp>
#include <vector>
#include <stack>
#include <cstdint>

CALLER_BEGIN

class RingBuffer
{
public:
    typedef unsigned char Element;
protected:
    typedef ::std::vector<Element> Container;
public:
    RingBuffer(size_t size);
    ~RingBuffer();
public:
    size_t  put(const Element *datas, size_t len);
    size_t  take(Element *datas, size_t len);
    size_t  takeSkip(Element *datas, size_t len, size_t skip) {
        if (!move(skip))
            return 0;
        return take(datas, len);
    }

    size_t  length() const;
    size_t  capacity() const;
    size_t  copyTo(Element *datas, size_t len, size_t off = 0) const;

    const Element &current() const  { return _M_Datas[_M_Header % _M_Datas.size()]; }
    bool           moveToNext()            { return move(1); }
    bool           move(uint32_t off);

    size_t         writeIndex() const;
    void           setWriteIndex(const size_t &writeIndex);

    size_t         readIndex() const;
    void           setReadIndex(const size_t &readIndex);
protected:
    size_t  copyFrom(const Element *datas, size_t len);
private:
    Container   _M_Datas;
    size_t      _M_Header;
    size_t      _M_Tailer;
};

CALLER_END

#endif // CALLER_RINGBUFFER_HPP
