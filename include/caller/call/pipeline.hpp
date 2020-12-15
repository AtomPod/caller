#ifndef CALLER_PIPELINE_HPP
#define CALLER_PIPELINE_HPP
#include <vector>
#include <caller/core.hpp>
#include <caller/call/declare.hpp>
#include <caller/context/declare.hpp>
#include <caller/common/bytebuffer.hpp>

CALLER_BEGIN

class Pipeline
{
    typedef std::vector<PipelineStagePtr> PipelineStageSet;
public:
    static PipelinePtr make(PipelineStagePtr last);
public:
    Pipeline(PipelineStagePtr last);
    Pipeline(const Pipeline &) = delete;
    Pipeline(Pipeline &&) = delete;
    Pipeline& operator=(const Pipeline &) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    ~Pipeline() = default;
public:
    void propagate(IOContextPtr ctx,
                    ByteBuffer data,
                    ServicePtr service);
    void append(const PipelineStagePtr &stage);
private:
    const PipelineStagePtr _M_LastStage;
    PipelineStageSet       _M_Stages;
};

CALLER_END

#endif // CALLER_PIPELINE_HPP
