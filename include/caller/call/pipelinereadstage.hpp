#ifndef CALLER_PIPELINEREADSTAGE_HPP
#define CALLER_PIPELINEREADSTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class PipelineReadStage : public PipelineStage
{
public:
    PipelineReadStage();
    virtual ~PipelineReadStage() override = default;
protected:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        auto next = nextStage();
        if (next != nullptr) {
            next->handleWrite(context, buffer, object);
        }
    }
    virtual void pipelineWriteComplete(PipelineContextPtr) override  {}
};

template<typename T>
class PipelineTypedReadStage : public PipelineReadStage {
public:
    PipelineTypedReadStage() = default;
    virtual ~PipelineTypedReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        T typedObject = CALLER any_cast<T>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, const T &object) = 0;
};

template<typename T>
class PipelineTypedReadStage<T*> : public PipelineReadStage {
public:
    PipelineTypedReadStage() = default;
    virtual ~PipelineTypedReadStage() override = default;
public:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        T* typedObject = CALLER any_cast<T*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, T *object) = 0;
};


CALLER_END

#endif // CALLER_PIPELINEREADSTAGE_HPP
