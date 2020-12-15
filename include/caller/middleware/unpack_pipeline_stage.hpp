#ifndef CALLER_UNPACKINTERCEPTOR_HPP
#define CALLER_UNPACKINTERCEPTOR_HPP
#include <caller/call/pipelinestage.hpp>
#include <caller/context/context.hpp>
#include <caller/common/ringbuffer.hpp>

CALLER_BEGIN

class UnpackPipelineStage : public PipelineStage
{
public:
    typedef uint16_t Tag;
    static constexpr size_t SizeOfTag = sizeof(Tag);
public:
    UnpackPipelineStage(size_t buffersize = 64 * 1024);
    virtual ~UnpackPipelineStage();


    Tag headerTag() const;
    void setHeaderTag(const Tag &headerTag);

    Tag tailerTag() const;
    void setTailerTag(const Tag &tailerTag);


    size_t minimumSize() const;
    void setMinimumSize(const size_t &minimumSize);

protected:
    virtual size_t  packageSize(const RingBuffer &buffer) = 0;

public:
    virtual void propagate(IOContextPtr ctx,
                            ByteBuffer data,
                            ServicePtr service);

private:
    RingBuffer  _M_Datas;
    bool        _M_HeaderTagValid;
    Tag         _M_HeaderTag;
    bool        _M_TailerTagValid;
    Tag         _M_TailerTag;
    size_t      _M_MinimumSize;
};

CALLER_END

#endif // UNPACKINTERCEPTOR_HPP
