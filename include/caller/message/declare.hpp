#ifndef CALLER_MESSAGE_DECLARE_HPP
#define CALLER_MESSAGE_DECLARE_HPP

#include <caller/core.hpp>
#include <memory>

CALLER_BEGIN

class MessageBody;
typedef std::shared_ptr<MessageBody> MessageBodyPtr;

class MessageHeader;
typedef std::shared_ptr<MessageHeader> MessageHeaderPtr;

class Message;
typedef std::shared_ptr<Message> MessagePtr;

class MessageFactory;
typedef std::shared_ptr<MessageFactory> MessageFactoryPtr;

CALLER_END

#endif // CALLER_MESSAGE_DECLARE_HPP
