#ifndef CALLER_DECLARE_HPP
#define CALLER_DECLARE_HPP

#include <caller/core.hpp>
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class Request;
typedef RefPtr<Request> RequestPtr;

class Response;
typedef RefPtr<Response> ResponsePtr;

class PipelineStage;
typedef RefPtr<PipelineStage> PipelineStagePtr;

class Pipeline;
typedef RefPtr<Pipeline> PipelinePtr;

class PipelineContext;
typedef RefPtr<PipelineContext> PipelineContextPtr;

CALLER_END

#endif // CALLER_DECLARE_HPP
