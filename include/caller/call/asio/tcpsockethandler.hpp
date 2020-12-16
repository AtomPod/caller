#ifndef CALLER_ASIO_TCP_SOCKETHANDLER_HPP
#define CALLER_ASIO_TCP_SOCKETHANDLER_HPP
#include <caller/call/sockethandler.hpp>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/call/pipelinecontext.hpp>
#include <caller/call/pipeline.hpp>
#include <atomic>
#include <list>

CALLER_BEGIN

class AsioTcpSocketHandler : public SocketHandler
{
    friend class AsioTcpPipelineContext;
    typedef ::asio::ip::tcp                 Protocol;
    typedef ::asio::ip::tcp::socket         Socket;
    typedef ::asio::ip::tcp::resolver       EndpointResolver;
    typedef EndpointResolver::endpoint_type AsioEndpoint;
    typedef std::list<AsioEndpoint>         EndpointList;
public:
    AsioTcpSocketHandler(AsioExecutionContext &context);
    virtual ~AsioTcpSocketHandler() override;
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

protected:
    virtual void onConnectivityStateChanged(const ConnectivityState &state);
private:
    AsioExecutionContext       &_M_ExecutionContext;
    Socket                      _M_Socket;
    std::atomic_int             _M_ConnState;
    EndpointList                _M_EndpointList;
    EndpointResolver            _M_Resolver;
    ByteBuffer                  _M_DataBuffer;

    PipelinePtr                 _M_ReadPipeline;
    PipelinePtr                 _M_WritePipeline;
};

class AsioTcpPipelineContext : public PipelineContext, public std::enable_shared_from_this<AsioTcpPipelineContext>
{
public:
    static PipelineContextPtr make(AsioTcpSocketHandler *handler);
public:
    AsioTcpPipelineContext(AsioTcpSocketHandler *handler);
    virtual ~AsioTcpPipelineContext() override;
public:
    virtual Future<void>        connect(const Endpoint &endpoint)                   override;
    virtual Future<void>        disconnect()                                        override;
    virtual Future<void>        close()                                             override;
    virtual Future<size_t>      read(ByteBuffer buffer)                             override;
    virtual void                write(const any &object, const ByteBuffer &buffer)  override;
    virtual Executor*           executor()                                          override;
public:
    virtual void                notifyActive() override;
    virtual void                notifyInactive() override;
    virtual void                notifyReadComplete() override;
    virtual void                notifyWriteComplete() override;
protected:
    void                        notifyException(const std::exception &e);
private:
    AsioTcpSocketHandler *_M_Handler;
    PipelinePtr           _M_ReadPipeline;
    PipelinePtr           _M_WritePipeline;
};

CALLER_END

#endif // SOCKETHANDLER_HPP
