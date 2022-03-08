#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

PipelineStage::PipelineStage(const PipelineStage::StageType &t) : _M_StageType(t)
{

}

PipelineStage::StageType PipelineStage::stageType() const
{
    return _M_StageType;
}

PipelineStagePtr PipelineStage::findWritableStage() const
{
    PipelineStagePtr writable = _M_NextStage;
    while (writable != nullptr && !(writable->stageType() & PipelineStage::StageTypeOut)) {
        writable = writable->nextStage();
    }
    return writable;
}

PipelineStagePtr PipelineStage::findReadableStage() const
{
    PipelineStagePtr readable = _M_PrevStage;
    while (readable != nullptr && !(readable->stageType() & PipelineStage::StageTypeIn)) {
        readable = readable->prevStage();
    }
    return readable;
}

PipelineStagePtr PipelineStage::prevStage() const
{
    return _M_PrevStage;
}

void PipelineStage::setPrevStage(const PipelineStagePtr &prevStage)
{
    _M_PrevStage = prevStage;
}

CALLER_END
