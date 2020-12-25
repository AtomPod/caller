#ifndef CALLER_ENDPOINT_HPP
#define CALLER_ENDPOINT_HPP
#include <caller/core.hpp>
#include <string>

CALLER_BEGIN

class CALLER_DLL_EXPORT Endpoint
{
public:
    Endpoint();
    ~Endpoint();
public:
    std::string host() const;
    void setHost(const std::string &host);

    std::string port() const;
    void setPort(const std::string &port);

private:
    std::string _M_Host;
    std::string _M_Port;
};

CALLER_END

#endif // CALLER_ENDPOINT_HPP
