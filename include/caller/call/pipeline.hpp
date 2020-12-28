#ifndef CALLER_PIPELINE_HPP
#define CALLER_PIPELINE_HPP
#include <vector>
#include <caller/core.hpp>
#include <caller/call/declare.hpp>
#include <caller/context/declare.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/common/any.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT Pipeline
{
    typedef std::vector<PipelineStagePtr> PipelineStageSet;
public:
    static PipelinePtr make(PipelineStagePtr first,PipelineStagePtr last);
public:
    Pipeline(PipelineStagePtr first, PipelineStagePtr last);
    Pipeline(const Pipeline &) = delete;
    Pipeline(Pipeline &&) = delete;
    Pipeline& operator=(const Pipeline &) = delete;
    Pipeline& operator=(Pipeline&&) = delete;
    ~Pipeline() = default;
public:
    void append(const PipelineStagePtr &stage);

    void pipelineActive(PipelineContextPtr context)         ;
    void pipelineInactive(PipelineContextPtr context)       ;
    void pipelineWriteComplete(PipelineContextPtr context)  ;
    void pipelineReadComplete(PipelineContextPtr context)   ;
    void handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object) ;
    void handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object) ;
    void causeException(PipelineContextPtr context, const std::exception &e) ;
private:
    const PipelineStagePtr _M_FirstStage;
    const PipelineStagePtr _M_LastStage;
    PipelineStageSet       _M_Stages;
};

CALLER_END

#endif // CALLER_PIPELINE_HPP
