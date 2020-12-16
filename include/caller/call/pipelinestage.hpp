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

class PipelineStage : public std::enable_shared_from_this<PipelineStage>
{
    friend class Pipeline;
public:
    template<typename T, typename... Args>
    static PipelineStagePtr create(Args&& ...args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
public:
    PipelineStage() = default;
    PipelineStage(const PipelineStage &) = delete;
    PipelineStage& operator=(const PipelineStage &) = delete;
    virtual ~PipelineStage() = default;
public:
    virtual void pipelineActive(PipelineContextPtr)         {}
    virtual void pipelineInactive(PipelineContextPtr)       {}
    virtual void pipelineWriteComplete(PipelineContextPtr)  {}
    virtual void pipelineReadComplete(PipelineContextPtr)   {}
    virtual void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) = 0;
    virtual void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) = 0;
    virtual void causeException(PipelineContextPtr, const std::exception &) {}

public:
    PipelineStagePtr nextStage() const { return _M_NextStage; }
    void setNextStage(const PipelineStagePtr &nextStage) { _M_NextStage = nextStage; }

    PipelineStagePtr currentStage() { return this->shared_from_this(); }
private:
    PipelineStagePtr _M_NextStage;
};

CALLER_END

#endif // PIPELINESTAGE_HPP
