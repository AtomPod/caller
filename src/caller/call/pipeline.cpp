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

void Pipeline::append(const PipelineStagePtr &stage)
{
    if (_M_Stages.size() > 0) {
        PipelineStagePtr penultimate = _M_Stages.back();
        penultimate->setNextStage(stage);
    }
    stage->setNextStage(_M_LastStage);
    _M_Stages.push_back(stage);
}

void Pipeline::pipelineActive(PipelineContextPtr context)
{
    for (auto stage : _M_Stages) {
        stage->pipelineActive(context);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->pipelineActive(context);
    }
}

void Pipeline::pipelineInactive(PipelineContextPtr context)
{
    for (auto stage : _M_Stages) {
        stage->pipelineInactive(context);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->pipelineInactive(context);
    }
}

void Pipeline::pipelineWriteComplete(PipelineContextPtr context)
{
    for (auto stage : _M_Stages) {
        stage->pipelineWriteComplete(context);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->pipelineWriteComplete(context);
    }
}

void Pipeline::pipelineReadComplete(PipelineContextPtr context)
{
    for (auto stage : _M_Stages) {
        stage->pipelineReadComplete(context);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->pipelineReadComplete(context);
    }
}

void Pipeline::handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object)
{
    try {
        PipelineStagePtr first = nullptr;

        {
            first = (_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage;
        }

        if (first != nullptr) {
            first->handleRead(context, buffer, object);
        }
    } catch (const std::exception &e) {
        causeException(context, e);
    }
}

void Pipeline::handleWrite(PipelineContextPtr context, ByteBuffer buffer, const any &object)
{
    try {
        PipelineStagePtr first = nullptr;

        {
            first = (_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage;
        }

        if (first != nullptr) {
            first->handleWrite(context, buffer, object);
        }
    } catch (const std::exception &e) {
        causeException(context, e);
    }
}

void Pipeline::causeException(PipelineContextPtr context, const std::exception &e)
{
    for (auto stage : _M_Stages) {
        stage->causeException(context, e);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->causeException(context, e);
    }
}

CALLER_END
