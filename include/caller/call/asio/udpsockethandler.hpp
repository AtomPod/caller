#ifndef CALLER_ASIO_UDPSOCKETHANDLER_HPP
#define CALLER_ASIO_UDPSOCKETHANDLER_HPP

#include <caller/call/sockethandler.hpp>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/call/pipelinecontext.hpp>
#include <caller/call/pipeline.hpp>
#include <atomic>
#include <list>

CALLER_BEGIN

class AsioUdpSocketHandler : public SocketHandler
{
    friend class AsioTcpPipelineContext;
    typedef ::asio::ip::udp                 Protocol;
    typedef ::asio::ip::udp::socket         Socket;
    typedef ::asio::ip::udp::resolver       EndpointResolver;
    typedef EndpointResolver::endpoint_type AsioEndpoint;
    typedef std::list<AsioEndpoint>         EndpointList;
public:
    AsioUdpSocketHandler(AsioExecutionContext &context);
    virtual ~AsioUdpSocketHandler() override;
public:
    virtual Future<void>        bind(const Endpoint &endpoint)      override;
    virtual Future<void>        connect(const Endpoint &endpoint)   override;
    virtual Future<void>        disconnect()                        override;
    virtual Future<void>        close()                             override;
    virtual Future<size_t>      read(ByteBuffer buffer)             override;
    virtual Future<size_t>      write(const ByteBuffer &buffer)     override;
    virtual Executor*           executor()                          override;
protected:
    void    resolverAddress(const Endpoint &endpoint, FutureInterface<void> futureSender);
    void    resetSocket();
    void    connectWithEndpointList(const EndpointList &endpoints, FutureInterface<void> futureInterface);
    bool    setConnectivityState(const ConnectivityState &state);
    void    readNext(ByteBuffer buffer, FutureInterface<size_t> futureSender);
    bool    closeSocketWithState(FutureInterface<void> futureSender ,const ConnectivityState& state);
private:
    AsioExecutionContext       &_M_ExecutionContext;
    Socket                      _M_Socket;
    std::atomic_int             _M_ConnState;
    EndpointList                _M_EndpointList;
    EndpointResolver            _M_Resolver;
    ByteBuffer                  _M_DataBuffer;
};

CALLER_END

#endif // UDPSOCKETHANDLER_HPP
