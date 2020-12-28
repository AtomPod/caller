#ifndef CALLER_PIPELINEWRITEREADSTAGE_HPP
#define CALLER_PIPELINEWRITEREADSTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineWriteReadStage : public PipelineStage
{
public:
    PipelineWriteReadStage();
    virtual ~PipelineWriteReadStage() override = default;
};

template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        WriteType typedObject = CALLER any_cast<WriteType>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        ReadType typedObject = CALLER any_cast<ReadType>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, const WriteType &object) = 0;
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, const ReadType &object) = 0;
};

template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType*, ReadType> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        WriteType *typedObject = CALLER any_cast<WriteType*>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        ReadType typedObject = CALLER any_cast<ReadType>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, WriteType *object) = 0;
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, const ReadType &object) = 0;
};


template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType, ReadType*> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        WriteType typedObject = CALLER any_cast<WriteType>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        ReadType *typedObject = CALLER any_cast<ReadType*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, const WriteType &object) = 0;
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer,  ReadType *object) = 0;
};


template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType*, ReadType*> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        WriteType *typedObject = CALLER any_cast<WriteType*>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) override final {
        ReadType *typedObject = CALLER any_cast<ReadType*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, WriteType *object) = 0;
    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer,  ReadType *object) = 0;
};


CALLER_END

#endif // PIPELINEWRITEREADSTAGE_HPP
