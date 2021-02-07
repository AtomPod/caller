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
    virtual void handleWrite(const PipelineContextPtr& context, ByteBuffer& buffer, const Any &object) override final {
        WriteType typedObject = CALLER AnyCast<WriteType>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        ReadType typedObject = CALLER AnyCast<ReadType>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(const PipelineContextPtr& context, const ByteBuffer& buffer, const WriteType &object) = 0;
    virtual void handleTypedRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const ReadType &object) = 0;
};

template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType*, ReadType> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(const PipelineContextPtr& context, ByteBuffer& buffer, const Any &object) override final {
        WriteType *typedObject = CALLER AnyCast<WriteType*>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        ReadType typedObject = CALLER AnyCast<ReadType>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(const PipelineContextPtr& context, const ByteBuffer& buffer, WriteType *object) = 0;
    virtual void handleTypedRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const ReadType &object) = 0;
};


template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType, ReadType*> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(const PipelineContextPtr& context, ByteBuffer& buffer, const Any &object) override final {
        WriteType typedObject = CALLER AnyCast<WriteType>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        ReadType *typedObject = CALLER AnyCast<ReadType*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(const PipelineContextPtr& context, const ByteBuffer& buffer, const WriteType &object) = 0;
    virtual void handleTypedRead(const PipelineContextPtr& context, const ByteBuffer& buffer,  ReadType *object) = 0;
};


template<typename WriteType, typename ReadType>
class CALLER_DLL_EXPORT PipelineTypedWriteReadStage<WriteType*, ReadType*> : public PipelineStage
{
public:
    PipelineTypedWriteReadStage() : PipelineStage(StageTypeBoth) {}
    virtual ~PipelineTypedWriteReadStage() override = default;
public:
    virtual void handleWrite(const PipelineContextPtr& context, ByteBuffer& buffer, const Any &object) override final {
        WriteType *typedObject = CALLER AnyCast<WriteType*>(object);
        handleTypedWrite(context, buffer, typedObject);
    }
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        ReadType *typedObject = CALLER AnyCast<ReadType*>(object);
        handleTypedRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypedWrite(const PipelineContextPtr& context, const ByteBuffer& buffer, WriteType *object) = 0;
    virtual void handleTypedRead(const PipelineContextPtr& context, const ByteBuffer& buffer,  ReadType *object) = 0;
};


CALLER_END

#endif // PIPELINEWRITEREADSTAGE_HPP
