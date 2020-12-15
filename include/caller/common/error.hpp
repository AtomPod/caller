#ifndef CALLER_ERROR_HPP
#define CALLER_ERROR_HPP
#include <caller/core.hpp>
#include <string>

CALLER_BEGIN

class Error
{
public:
    static Error Nil;
public:
    typedef int             Code;
    typedef std::string     String;
public:
    Error(const Code &code = 0, const String &msg = "");
    virtual ~Error();
public:
    Code code() const;
    void setCode(const Code &code);

    String message() const;
    void setMessage(const String &message);
public:
    bool operator==(const Error &e) const { return _M_Code == e.code(); }
    bool operator!=(const Error &e) const { return !(*this == e);       }
private:
    Code    _M_Code;
    String  _M_Message;
};

CALLER_END

#endif // CALLER_ERROR_HPP
