#include <caller/middleware/unpack_pipeline_stage.hpp>
#include <caller/context/context.hpp>
#include <caller/call/iocontext.hpp>
#include <caller/call/response.hpp>

CALLER_BEGIN

UnpackPipelineStage::UnpackPipelineStage(size_t buffersize)
    : _M_Datas(buffersize), _M_HeaderTagValid(false), _M_TailerTagValid(false),
      _M_MinimumSize(0)
{

}

UnpackPipelineStage::~UnpackPipelineStage()
{

}

size_t UnpackPipelineStage::minimumSize() const
{
    return _M_MinimumSize;
}

void UnpackPipelineStage::setMinimumSize(const size_t &minimumSize)
{
    _M_MinimumSize = minimumSize;
}

void UnpackPipelineStage::propagate(IOContextPtr ctx, ByteBuffer data, ServicePtr service)
{
    UNUSED(service);
    ByteBuffer&    buffer   = data;
    unsigned char *datas    = reinterpret_cast<unsigned char*>(buffer.data());
    size_t         length   = buffer.length();
    size_t         offset   = 0;

    while (length > 0) {
        size_t rbytes = _M_Datas.put(datas + offset, length);

        while (_M_Datas.length() > 0) {

            if (_M_Datas.length() < _M_MinimumSize) {
                break;
            }

            if (_M_HeaderTagValid) {
                Tag headerTag = 0;
                if (_M_Datas.copyTo(reinterpret_cast<RingBuffer::Element*>(&headerTag), SizeOfTag, 0) != SizeOfTag) {
                    break;
                }
                headerTag = fromBigEndian(headerTag);

                if (headerTag != _M_HeaderTag) {
                    _M_Datas.moveToNext();
                    continue;
                }
            }

            size_t length = packageSize(_M_Datas);
            Tag    tailerTag = 0;

            if (_M_Datas.length() >= length) {

                if (_M_TailerTagValid) {
                    if (_M_Datas.copyTo(reinterpret_cast<RingBuffer::Element*>(&tailerTag),
                                        SizeOfTag, length - SizeOfTag) != SizeOfTag) {
                        break;
                    }

                    tailerTag = fromBigEndian(tailerTag);
                    if (tailerTag != _M_TailerTag) {
                        _M_Datas.moveToNext();
                        continue;
                    }
                }

                ByteBuffer packageBuffer(length);
                _M_Datas.take(reinterpret_cast<RingBuffer::Element*>(packageBuffer.data()), length);
                packageBuffer.resize(length);

                auto next = nextStage();

                if (next) {
                    IOContextPtr context =  IOContext::make(ctx);
                    ResponsePtr  response = Response::make();

                    response->setDataBuffer(packageBuffer);
                    context->setResponse(response);
                    next->propagate(context, packageBuffer, service);
                }
            }
            break;
        }
        length -= rbytes;
    }
}

UnpackPipelineStage::Tag UnpackPipelineStage::tailerTag() const
{
    return _M_TailerTag;
}

void UnpackPipelineStage::setTailerTag(const Tag &tailerTag)
{
    _M_TailerTag        = tailerTag;
    _M_TailerTagValid   = true;
}

UnpackPipelineStage::Tag UnpackPipelineStage::headerTag() const
{
    return _M_HeaderTag;
}

void UnpackPipelineStage::setHeaderTag(const Tag &headerTag)
{
    _M_HeaderTag        = headerTag;
    _M_HeaderTagValid   = true;
}


CALLER_END
