#ifndef CALLER_PIPELINESTAGE_HPP
#define CALLER_PIPELINESTAGE_HPP
#include <caller/core.hpp>
#include <caller/call/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/context/context.hpp>
#include <caller/call/pipelinecontext.hpp>
#include <caller/common/any.hpp>
#include <memory>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineStage : public std::enable_shared_from_this<PipelineStage>
{
    friend class Pipeline;
public:
    enum StageType {
        StageTypeIn  = 0x01,
        StageTypeOut = 0x02,
        StageTypeBoth = StageTypeIn | StageTypeOut
    };
public:
    template<typename T, typename... Args>
    static PipelineStagePtr create(Args&& ...args) {
        return NewRefPtr<T>(std::forward<Args>(args)...);
    }
public:
    PipelineStage(const StageType &t);
    PipelineStage(const PipelineStage &) = delete;
    PipelineStage& operator=(const PipelineStage &) = delete;
    virtual ~PipelineStage() = default;
public:
    virtual void pipelineActive(PipelineContextPtr ctx)         {
        invokePipelineActive(ctx);
    }

    virtual void pipelineInactive(PipelineContextPtr ctx)       {
        invokePipelineInactive(ctx);
    }

    virtual void pipelineWriteComplete(PipelineContextPtr ctx)  {
        invokePipelineWriteComplete(ctx);
    }

    virtual void pipelineReadComplete(PipelineContextPtr ctx)   {
        invokePipelineReadComplete(ctx);
    }

    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) {
        invokeReader(context, buffer, object);
    }

    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) {
        invokeWriter(context, buffer, object);
    }

    virtual void causeException(PipelineContextPtr ctx, const std::exception &e) {
        invokeCauseException(ctx, e);
    }
public:
    Executor *executor() { return _M_Executor; }
    void      setExecutor(Executor *executor) { _M_Executor = executor; }

public:
    PipelineStagePtr nextStage() const { return _M_NextStage; }
    void setNextStage(const PipelineStagePtr &nextStage) { _M_NextStage = nextStage; }

    PipelineStagePtr prevStage() const;
    void setPrevStage(const PipelineStagePtr &prevStage);

    PipelineStagePtr currentStage() { return this->shared_from_this(); }

    StageType        stageType() const;


protected:
    void invokePipelineActive(PipelineContextPtr ctx)         {
        if (auto next = nextStage(); next != nullptr) {
            next->pipelineActive(ctx);
        }
    }

    void invokePipelineInactive(PipelineContextPtr ctx)       {
        if (auto next = nextStage(); next != nullptr) {
            next->pipelineInactive(ctx);
        }
    }
    void invokePipelineWriteComplete(PipelineContextPtr ctx)  {
        if (auto next = findWritableStage(); next != nullptr) {
            next->pipelineWriteComplete(ctx);
        }
    }

    void invokePipelineReadComplete(PipelineContextPtr ctx)   {
        if (auto next = findReadableStage(); next != nullptr) {
            next->pipelineReadComplete(ctx);
        }
    }

    void invokeCauseException(PipelineContextPtr ctx, const std::exception &e) {
        if (auto next = nextStage(); next != nullptr) {
            next->causeException(ctx, e);
        }
    }

    void invokeWriter(PipelineContextPtr context, ByteBuffer buffer, const any &object) {
        if (auto next = findWritableStage(); next != nullptr) {
            next->handleWrite(context, buffer, object);
        }
    }

    void invokeReader(PipelineContextPtr context, ByteBuffer buffer, const any &object) {
        if (auto next = findReadableStage(); next != nullptr) {
            next->handleRead(context, buffer, object);
        }
    }
protected:
    PipelineStagePtr findWritableStage() const;
    PipelineStagePtr findReadableStage() const;

private:
    PipelineStagePtr _M_NextStage;
    PipelineStagePtr _M_PrevStage;
    StageType        _M_StageType;
    Executor        *_M_Executor = nullptr;
};

CALLER_END

#endif // PIPELINESTAGE_HPP
