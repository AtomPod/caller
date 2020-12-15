#ifndef CALLER_PIPELINESTAGEWRAPPER_HPP
#define CALLER_PIPELINESTAGEWRAPPER_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class PipelineStageWrapper : public PipelineStage
{
public:
    typedef std::function<void(IOContextPtr ctx,
                               ByteBuffer data,
                               ServicePtr service,
                               PipelineStagePtr next)> Handler;
public:
    static PipelineStagePtr make(const Handler &handler);
public:
    PipelineStageWrapper(const Handler &handler);

public:
    virtual void propagate(IOContextPtr ctx,
                            ByteBuffer data,
                            ServicePtr service) override;

private:
    Handler     _M_Handler;
};

CALLER_END

#endif // PIPELINESTAGEWRAPPER_HPP
