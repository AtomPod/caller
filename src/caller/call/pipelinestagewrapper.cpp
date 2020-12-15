#include <caller/call/pipelinestagewrapper.hpp>
#include <cassert>

CALLER_BEGIN

PipelineStagePtr PipelineStageWrapper::make(const PipelineStageWrapper::Handler &handler)
{
    return std::make_shared<PipelineStageWrapper>(handler);
}

PipelineStageWrapper::PipelineStageWrapper(const Handler &handler) : _M_Handler(handler)
{
    assert(handler != nullptr);
}

void PipelineStageWrapper::propagate(IOContextPtr ctx, ByteBuffer data, ServicePtr service)
{
    _M_Handler(ctx, data, service, nextStage());
}


CALLER_END
