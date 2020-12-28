#include <caller/call/pipeline.hpp>
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

PipelinePtr Pipeline::make(PipelineStagePtr first, PipelineStagePtr last)
{
    return NewRefPtr<Pipeline>(first, last);
}

Pipeline::Pipeline(PipelineStagePtr first, PipelineStagePtr last) : _M_FirstStage(first), _M_LastStage(last)
{
    if (_M_FirstStage != nullptr) {
        _M_FirstStage->setPrevStage(nullptr);
        _M_FirstStage->setNextStage(_M_LastStage);
    }

    if (_M_LastStage != nullptr) {
        _M_LastStage->setPrevStage(_M_FirstStage);
        _M_LastStage->setNextStage(nullptr);
    }
}

void Pipeline::append(const PipelineStagePtr &stage)
{
    if (_M_Stages.size() > 0) {
        PipelineStagePtr penultimate = _M_Stages.back();
        penultimate->setNextStage(stage);
        stage->setPrevStage(penultimate);
    } else {
        stage->setPrevStage(_M_FirstStage);
    }

    if (_M_FirstStage != nullptr && _M_Stages.size() == 0) {
        _M_FirstStage->setNextStage(stage);
    }

    stage->setNextStage(_M_LastStage);
    if (_M_LastStage != nullptr) {
        _M_LastStage->setPrevStage(stage);
    }
    _M_Stages.push_back(stage);
}

void Pipeline::pipelineActive(PipelineContextPtr context)
{
    PipelineStagePtr first = nullptr;

    {
        first = _M_FirstStage ? _M_FirstStage : ((_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage);
    }

    if (first != nullptr) {
        first->pipelineActive(context);
    }
}

void Pipeline::pipelineInactive(PipelineContextPtr context)
{
    PipelineStagePtr first = nullptr;

    {
        first = _M_FirstStage ? _M_FirstStage : ((_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage);
    }

    if (first != nullptr) {
        first->pipelineInactive(context);
    }
}

void Pipeline::pipelineWriteComplete(PipelineContextPtr context)
{
    PipelineStagePtr first = nullptr;

    {
        first = _M_FirstStage ? _M_FirstStage : ((_M_Stages.size() > 0) ? _M_Stages[0] : _M_LastStage);
    }

    if (first != nullptr) {
        first->pipelineWriteComplete(context);
    }
}

void Pipeline::pipelineReadComplete(PipelineContextPtr context)
{
    PipelineStagePtr first = nullptr;

    {
        first = _M_LastStage ? _M_LastStage : ((_M_Stages.size() > 0) ? _M_Stages.back() : _M_FirstStage);
    }

    if (first != nullptr) {
        first->pipelineReadComplete(context);
    }
}

void Pipeline::handleRead(PipelineContextPtr context, ByteBuffer buffer, const any &object)
{
    try {
        PipelineStagePtr first = nullptr;

        {
            first = _M_LastStage ? _M_LastStage : ((_M_Stages.size() > 0) ? _M_Stages.back() : _M_FirstStage);
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
