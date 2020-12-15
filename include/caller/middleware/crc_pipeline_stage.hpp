#ifndef CALLER_CRC_PIPELINE_STAGE_HPP
#define CALLER_CRC_PIPELINE_STAGE_HPP
#include <caller/call/pipelinestagewrapper.hpp>
#include <caller/context/context.hpp>

CALLER_BEGIN

class CRCPipelineStage
{
public:
    static  PipelineStageWrapper::Handler validate16Intercept(size_t start, size_t end);

private:
    static  uint16_t calculateCRC(const char *input, int size);
};

CALLER_END

#endif // CALLER_CRC_PIPELINE_STAGE_HPP
