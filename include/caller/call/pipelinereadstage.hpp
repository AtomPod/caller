#ifndef CALLER_PIPELINEREADSTAGE_HPP
#define CALLER_PIPELINEREADSTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineReadStage : public PipelineStage
{
public:
    PipelineReadStage();
    virtual ~PipelineReadStage() override = default;
};

template<typename T>
class PipelineTypedReadStage : public PipelineReadStage {
public:
    PipelineTypedReadStage() = default;
    virtual ~PipelineTypedReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        T typedObject = CALLER any_cast<T>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, const T &object) = 0;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypedReadStage<T*> : public PipelineReadStage {
public:
    PipelineTypedReadStage() = default;
    virtual ~PipelineTypedReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        T* typedObject = CALLER any_cast<T*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, T *object) = 0;
};


CALLER_END

#endif // CALLER_PIPELINEREADSTAGE_HPP
