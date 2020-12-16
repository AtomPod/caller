#ifndef CALLER_SERIALIZABLE_HPP
#define CALLER_SERIALIZABLE_HPP
#include <caller/core.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/common/error.hpp>

CALLER_BEGIN

class Serializable
{
public:
    Serializable();
    virtual ~Serializable();
public:
    virtual     Error   serialize(ByteBuffer buffer)   const = 0;
    virtual     Error   deserialize(ByteBuffer buffer)       = 0;
};

CALLER_END

#endif // SERIALIZABLE_HPP
