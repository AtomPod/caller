#ifndef CALLER_VALUECONTEXT_HPP
#define CALLER_VALUECONTEXT_HPP
#include <caller/context/context.hpp>
#include <caller/context/declare.hpp>
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class ValueContext : public ContextBase
{
    template<typename T>
    friend class std::shared_ptr;
public:
    ValueContext(const KeyType &key, const ValueType &value);
public:
    virtual ~ValueContext() override = default;
public:
    virtual ValueType    Value(const KeyType &key)  override;
private:
    KeyType         _M_Key;
    ValueType       _M_Value;
};

CALLER_END

#endif // CALLER_VALUECONTEXT_HPP
