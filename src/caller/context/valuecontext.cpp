#include <caller/context/valuecontext.hpp>

CALLER_BEGIN

ValueContext::ValueContext(const KeyType &key, const ValueType &value)
    : _M_Key(key), _M_Value(value)
{

}

Context::ValueType ValueContext::Value(const Context::KeyType &key)
{
    if (key == _M_Key) {
        return _M_Value;
    }
    return ContextBase::Value(key);
}

CALLER_END
