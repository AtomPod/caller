#ifndef CALLER_PIPELINESTAGE_HPP
#define CALLER_PIPELINESTAGE_HPP
#include <caller/core.hpp>
#include <memory>
#include <caller/call/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/context/context.hpp>

CALLER_BEGIN

class PipelineStage : public std::enable_shared_from_this<PipelineStage>
{
    friend class Pipeline;
public:
    PipelineStage() = default;
    PipelineStage(const PipelineStage &) = delete;
    PipelineStage& operator=(const PipelineStage &) = delete;
    virtual ~PipelineStage() = default;
public:
    virtual void propagate(IOContextPtr ctx,
                            ByteBuffer data,
                            ServicePtr service) = 0;
public:
    PipelineStagePtr nextStage() const { return _M_NextStage; }
    void setNextStage(const PipelineStagePtr &nextStage) { _M_NextStage = nextStage; }

    PipelineStagePtr currentStage() { return this->shared_from_this(); }
private:
    PipelineStagePtr _M_NextStage;
};

CALLER_END

#endif // PIPELINESTAGE_HPP
