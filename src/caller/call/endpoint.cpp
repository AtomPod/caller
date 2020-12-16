#include <caller/call/endpoint.hpp>

CALLER_BEGIN

Endpoint::Endpoint()
{

}

std::string Endpoint::host() const
{
    return _M_Host;
}

void Endpoint::setHost(const std::string &host)
{
    _M_Host = host;
}

std::string Endpoint::port() const
{
    return _M_Port;
}

void Endpoint::setPort(const std::string &port)
{
    _M_Port = port;
}

CALLER_END
