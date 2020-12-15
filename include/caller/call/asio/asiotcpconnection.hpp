#ifndef CALLER_ASIOTCPCONNECTION_HPP
#define CALLER_ASIOTCPCONNECTION_HPP
#include <caller/core.hpp>
#include <caller/call/connection.hpp>
#include <caller/call/declare.hpp>
#include <asio.hpp>
#include <list>

CALLER_BEGIN

class AsioTcpConnection : public Connection, public std::enable_shared_from_this<AsioTcpConnection>
{   
    typedef ::asio::ip::tcp                     protocol;
    typedef ::asio::steady_timer                SteadyTimer;
    typedef protocol::resolver::endpoint_type   AsioEndpoint;
    typedef ::std::list<AsioEndpoint>           AsioEndpointList;
public:
    static AsioTcpConnectionPtr make(AsioService* service);
public:
    AsioTcpConnection(AsioService* service);
public:
    virtual void connect(const Endpoint &endpoint, const ConnectCallback &callback) ;
    virtual void read(ByteBuffer buffer,  const ReadCallback &callback)         ;
    virtual void write(const ByteBuffer &buffer, const WriteCallback &callback) ;
    virtual std::error_code shutdown(const ShutdownType &type)                  ;
    virtual std::error_code close()                                             ;

protected:
    virtual void onConnectivityStateChanged(const int &oldState, const int &currentState);

    void retryConnect();
    void resetSocket();
    void connectWithEndpointSequence(const AsioEndpointList &endpointSequence,const ConnectCallback &callback);
    void shutdownAction();
    void resolvedEndpoint(std::error_code e, protocol::resolver::iterator epIt);
private:
    protocol::socket                _M_Socket;
    protocol::resolver              _M_Resolver;
    AsioService*                    _M_Service;
    int                             _M_RetryCount;
    SteadyTimer                     _M_RetryTimer;
    AsioEndpointList                _M_EndpointList;
};

CALLER_END

#endif // CALLER_ASIOTCPCONNECTION_HPP
