#include <caller/call/pipeline.hpp>
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

PipelinePtr Pipeline::make(PipelineStagePtr last)
{
    return std::make_shared<Pipeline>(last);
}

Pipeline::Pipeline(PipelineStagePtr last) : _M_LastStage(last)
{

}

void Pipeline::propagate(IOContextPtr ctx, ByteBuffer data, ServicePtr service)
{
    PipelineStagePtr first = nullptr;

    {
        first = (_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage;
    }

    if (first != nullptr) {
        first->propagate(ctx, data, service);
    }
}

void Pipeline::append(const PipelineStagePtr &stage)
{
    if (_M_Stages.size() > 0) {
        PipelineStagePtr penultimate = _M_Stages.back();
        penultimate->setNextStage(stage);
    }
    stage->setNextStage(_M_LastStage);
    _M_Stages.push_back(stage);
}

CALLER_END
