#ifndef CALLER_MESSAGEBODY_HPP
#define CALLER_MESSAGEBODY_HPP
#include <caller/core.hpp>
#include <caller/common/serializable.hpp>

CALLER_BEGIN

class MessageBody : public Serializable
{
public:
    MessageBody();
    virtual ~MessageBody();
public:
    virtual bool isValid() const = 0;
};

CALLER_END

#endif // CALLER_MESSAGEBODY_HPP
