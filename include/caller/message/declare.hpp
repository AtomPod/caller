#ifndef CALLER_MESSAGE_DECLARE_HPP
#define CALLER_MESSAGE_DECLARE_HPP

#include <caller/core.hpp>
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class Message;
typedef RefPtr<Message> MessagePtr;

class MessageFactory;
typedef RefPtr<MessageFactory> MessageFactoryPtr;

CALLER_END

#endif // CALLER_MESSAGE_DECLARE_HPP
