#include <caller/common/error.hpp>

CALLER_BEGIN

Error Error::Nil(0, "");

Error::Error(const Code &code, const String &msg) : _M_Code(code), _M_Message(msg)
{

}

Error::~Error()
{

}

Error::Code Error::code() const
{
    return _M_Code;
}

void Error::setCode(const Code &code)
{
    _M_Code = code;
}

Error::String Error::message() const
{
    return _M_Message;
}

void Error::setMessage(const String &message)
{
    _M_Message = message;
}

CALLER_END
