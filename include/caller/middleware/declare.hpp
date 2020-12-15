#ifndef CALLER_MIDDLEWARE_DECLARE_HPP
#define CALLER_MIDDLEWARE_DECLARE_HPP
#include <memory>
#include <caller/core.hpp>

CALLER_BEGIN

class RouterPipelineStage;
typedef std::shared_ptr<RouterPipelineStage> RouterPipelineStagePtr;

class MessagePipelineStage;
typedef std::shared_ptr<MessagePipelineStage> MessageInterceptorPtr;

class UnpackPipelineStage;
typedef std::shared_ptr<UnpackPipelineStage>  UnpackPipelineStagePtr;

CALLER_END

#endif // CALLER_MIDDLEWARE_DECLARE_HPP
