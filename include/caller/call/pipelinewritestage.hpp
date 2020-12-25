#ifndef CALLER_PIPELINEWRITESTAGE_HPP
#define CALLER_PIPELINEWRITESTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineWriteStage : public PipelineStage
{
public:
    PipelineWriteStage();
    virtual ~PipelineWriteStage() override = default;
protected:
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        auto next = nextStage();
        if (next != nullptr) {
            next->handleRead(context, buffer, object);
        }
    }
    virtual void pipelineReadComplete(PipelineContextPtr) override  {}
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypedWriteStage : public PipelineWriteStage
{
public:
    PipelineTypedWriteStage() = default;
    virtual ~PipelineTypedWriteStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        T typedObject = any_cast<T>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, const T &object) = 0;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypedWriteStage<T*> : public PipelineWriteStage {
public:
    PipelineTypedWriteStage() = default;
    virtual ~PipelineTypedWriteStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override {
        T* typedObject = any_cast<T*>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, T *object) = 0;
};


CALLER_END

#endif // CALLER_PIPELINEWRITESTAGE_HPP
