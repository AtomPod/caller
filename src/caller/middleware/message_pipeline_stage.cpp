#include <caller/middleware/message_pipeline_stage.hpp>
#include <caller/call/iocontext.hpp>
#include <caller/call/response.hpp>
#include <caller/message/message.hpp>
#include <caller/message/messageheader.hpp>
#include <caller/message/messagebody.hpp>
#include <caller/message/messagefactory.hpp>

CALLER_BEGIN

MessagePipelineStage::MessagePipelineStage()
{

}

PipelineStageWrapper::Handler
MessagePipelineStage::messageHeaderParser(MessageHeaderGenerator g)
{
    if (g == nullptr) {
        return nullptr;
    }

    return [g](IOContextPtr context, ByteBuffer data,
            ServicePtr service, PipelineStagePtr next) {
        if (next == nullptr)
            return;

        MessageHeaderPtr header = g(data);
        if (header != nullptr) {
//            context->setMetadata(MessageHeader::MetaTag, header);
            auto ctx = context->context();
            ctx = Context::withValue(ctx, MessageHeader::MetaTag, header);
            context->setContext(ctx);

            next->propagate(context, data, service);
        }
    };
}

PipelineStageWrapper::Handler MessagePipelineStage::messageFromHeader(MessageFactoryPtr factory)
{
    if (factory == nullptr) {
        return nullptr;
    }

    return [factory](IOContextPtr context, ByteBuffer data,
            ServicePtr service, PipelineStagePtr next) {
        if (next == nullptr)
            return;

        MessageHeaderPtr header = context->metadata<MessageHeaderPtr>(MessageHeader::MetaTag);
        if (header == nullptr)
            return;

        MessagePtr message = factory->generate(header->id());
        if (message == nullptr) {
            return;
        }
        message->setHeader(header);

        auto response = context->response();
        MessageBodyPtr body = message->body();
        if (body != nullptr) {
            Error error = body->deserialize(data);
            if (error != Error::Nil) {
                context->addFlag(IOContext::ErrorFlag);
                response->setError(error);
            }
        }

        if (response != nullptr) {
            response->setRawMessage(message);
        }
        next->propagate(context, data, service);
    };
}

CALLER_END
