#ifndef CALLER_TYPEDMESSAGE_HPP
#define CALLER_TYPEDMESSAGE_HPP
#include <caller/message/message.hpp>

CALLER_BEGIN

template<typename MessageBody>
class TypedMessage : public Message
{
public:
    TypedMessage();
    virtual ~TypedMessage();
public:
    virtual ID          id() const { return MessageBody::id(); }
public:
    MessageBody 	   *body() const;

private:
    std::unique_ptr<MessageBody> _M_Body;
};

CALLER_END

#endif // TYPEDMESSAGE_HPP
