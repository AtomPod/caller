#ifndef CALLER_MESSAGEHEADER_HPP
#define CALLER_MESSAGEHEADER_HPP
#include <caller/core.hpp>
#include <caller/common/serializable.hpp>
#include <caller/common/id.hpp>
#include <caller/context/context.hpp>

CALLER_BEGIN

class MessageHeader : public Serializable
{
public:
    static const Context::KeyType   MetaTag;
public:
    MessageHeader();
    virtual ~MessageHeader();
public:
    virtual ID      id()   const = 0;
    virtual size_t  size() const = 0;
public:
    virtual bool    isValid() const = 0;
};

CALLER_END

#endif // MESSAGEHEADER_HPP
