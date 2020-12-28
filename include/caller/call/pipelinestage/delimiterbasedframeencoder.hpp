#ifndef CALLER_DELIMITERBASEDFRAMEENCODER_HPP
#define CALLER_DELIMITERBASEDFRAMEENCODER_HPP


#include <caller/call/pipelinewritestage.hpp>

CALLER_BEGIN

template<typename ObjectType>
class CALLER_DLL_EXPORT DelimiterBasedFrameEncoder : public PipelineWriteStage
{
public:
    DelimiterBasedFrameEncoder(){}
    virtual ~DelimiterBasedFrameEncoder() override {}
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        ObjectType encodeObject = CALLER any_cast<ObjectType>(object);
        if (encode(context, encodeObject, buffer)) {
            invokeWriter(context, buffer, object);
        }
    }
protected:
    virtual bool encode(PipelineContextPtr context, ObjectType object , ByteBuffer &pack) = 0;
};

CALLER_END

#endif // CALLER_DELIMITERBASEDFRAMEENCODER_HPP
