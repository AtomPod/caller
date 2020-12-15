#ifndef CALLER_ASIOTCPSERVICE_HPP
#define CALLER_ASIOTCPSERVICE_HPP
#include <caller/call/asio/asioservice.hpp>
#include <caller/call/asio/asiotcpconnection.hpp>
#include <caller/call/declare.hpp>

CALLER_BEGIN

class AsioTcpService : public AsioService, public std::enable_shared_from_this<AsioTcpService>
{
public:
    class Config : public AsioService::Config {
    public:
        AsioTcpConnection::Endpoint endpoint() const;
        void setEndpoint(const AsioTcpConnection::Endpoint &endpoint);
    private:
        AsioTcpConnection::Endpoint m_endpoint;
    };
public:
    static AsioTcpServicePtr make(AsioExecutionContext &context,const Config *config);
public:
    AsioTcpService(AsioExecutionContext &context, const Config *config);

protected:
    virtual void onConnectivityStateChanged(int newState);
    virtual void onData(ByteBuffer data);

public:
    virtual PipelineStagePtr            transportStage() const;
    // invoke 调用传入IOContextPtr与RequestPtr，返回一个Future，当响应时，通过 Future进行响应
    virtual Future<RequestResponsePtr>  invoke(IOContextPtr ctx, RequestPtr request);

protected:
    void readData();

private:
    ByteBuffer             _M_DataBuffer;
    AsioTcpConnectionPtr   _M_Connection;
    const Config*          _M_Config;
};

CALLER_END

#endif // ASIOTCPSERVICE_HPP
