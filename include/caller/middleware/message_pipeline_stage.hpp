#ifndef CALLER_MESSAGE_INTERCEPTOR_HPP
#define CALLER_MESSAGE_INTERCEPTOR_HPP
#include <caller/core.hpp>
#include <caller/route/router.hpp>
#include <caller/context/context.hpp>
#include <caller/message/declare.hpp>
#include <caller/call/pipelinestagewrapper.hpp>

CALLER_BEGIN

class MessagePipelineStage
{
public:
    typedef std::function<MessageHeaderPtr(ByteBuffer data)> MessageHeaderGenerator;
public:
    MessagePipelineStage();

public:
    static PipelineStageWrapper::Handler messageHeaderParser(MessageHeaderGenerator g);
    static PipelineStageWrapper::Handler messageFromHeader(MessageFactoryPtr factory);
};

CALLER_END

#endif // CALLER_MESSAGE_INTERCEPTOR_HPP
