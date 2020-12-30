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
    virtual void pipelineActive(const PipelineContextPtr &ctx)         {
        invokePipelineActive(ctx);
    }

    virtual void pipelineInactive(const PipelineContextPtr & ctx)       {
        invokePipelineInactive(ctx);
    }

    virtual void pipelineWriteComplete(const PipelineContextPtr & ctx)  {
        invokePipelineWriteComplete(ctx);
    }

    virtual void pipelineReadComplete(const PipelineContextPtr & ctx)   {
        invokePipelineReadComplete(ctx);
    }

    virtual void handleRead(const PipelineContextPtr &context, const ByteBuffer &buffer, const any &object) {
        invokeReader(context, buffer, object);
    }

    virtual void handleWrite(const PipelineContextPtr &context, ByteBuffer & buffer, const any &object) {
        invokeWriter(context, buffer, object);
    }

    virtual void causeException(const PipelineContextPtr & ctx, const std::exception &e) {
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
    void invokePipelineActive(const PipelineContextPtr & ctx)         {
        if (auto next = nextStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->pipelineActive(ctx);
            } else {
                executor->execute([next, ctx]() {
                    next->pipelineActive(ctx);
                });
            }
        }
    }

    void invokePipelineInactive(const PipelineContextPtr & ctx)       {
        if (auto next = nextStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->pipelineInactive(ctx);
            } else {
                executor->execute([next, ctx]() {
                     next->pipelineInactive(ctx);
                });
            }
        }
    }
    void invokePipelineWriteComplete(const PipelineContextPtr & ctx)  {
        if (auto next = findWritableStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->pipelineWriteComplete(ctx);
            } else {
                executor->execute([next, ctx]() {
                     next->pipelineWriteComplete(ctx);
                });
            }
        }
    }

    void invokePipelineReadComplete(const PipelineContextPtr & ctx)   {
        if (auto next = findReadableStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->pipelineReadComplete(ctx);
            } else {
                executor->execute([next, ctx]() {
                     next->pipelineReadComplete(ctx);
                });
            }
        }
    }

    void invokeCauseException(const PipelineContextPtr & ctx, const std::exception &e) {
        if (auto next = nextStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->causeException(ctx, e);
            } else {
                std::exception_ptr eptr = std::make_exception_ptr(e);
                executor->execute([next, ctx, eptr]() {
                    try {
                       std::rethrow_exception(eptr);
                    } catch (const std::exception &e) {
                       next->causeException(ctx, e);
                    }
                });
            }
        }
    }

    void invokeWriter(const PipelineContextPtr & ctx, ByteBuffer & buffer, const any &object) {
        if (auto next = findWritableStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->handleWrite(ctx, buffer, object);
            } else {
                executor->execute([next, ctx]() {
                    next->pipelineInactive(ctx);
                });
            }
        }
    }

    void invokeReader(const PipelineContextPtr & ctx, const ByteBuffer & buffer, const any &object) {
        if (auto next = findReadableStage(); next != nullptr) {
            auto executor = next->executor();

            if (executor == nullptr) {
                next->handleRead(ctx, buffer, object);
            } else {
                executor->execute([next, ctx]() {
                     next->pipelineInactive(ctx);
                });
            }
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
