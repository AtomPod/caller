#include <caller/call/asio/asiotcpconnection.hpp>
#include <caller/call/asio/asioservice.hpp>
#include <cassert>
#include <iostream>

CALLER_BEGIN

AsioTcpConnectionPtr AsioTcpConnection::make(AsioService *service)
{
    return std::make_shared<AsioTcpConnection>(service);
}

AsioTcpConnection::AsioTcpConnection(AsioService *service)
    : _M_Socket(service->context().rawContext()),
      _M_Resolver(service->context().rawContext()),
      _M_Service(service), _M_RetryCount(0),
      _M_RetryTimer(service->context().rawContext())
{

}

void AsioTcpConnection::connect(const Endpoint &endpoint, const Connection::ConnectCallback &callback)
{
//    assert(callback != nullptr);
//    if (callback == nullptr) {
//        return;
//    }

    int state = connectivityState();
    if (state == ConnectivityState::Idle || state == ConnectivityState::Shutdown) {
        setConnectivityState(ConnectivityState::Connecting);

        if (_M_EndpointList.size() == 0) {
            auto _self = shared_from_this();
            protocol::resolver::query query(endpoint.host(),endpoint.port());
            _M_Resolver.async_resolve(query, [_self, callback](std::error_code e, protocol::resolver::iterator epIt) {
                _self->resolvedEndpoint(e, epIt);
                if (!e) {
                    _self->connectWithEndpointSequence(_self->_M_EndpointList, callback);
                }
            });
        } else {
            connectWithEndpointSequence(_M_EndpointList, callback);
        }
    }
}

void AsioTcpConnection::read(ByteBuffer buffer, const Connection::ReadCallback &callback)
{
    assert(callback != nullptr);
    if (callback == nullptr) {
        return;
    }

    int state = connectivityState();
    if (state != ConnectivityState::Ready) {
        if (callback)
            callback(asio::error::not_connected, 0);
        return;
    }

    auto _self = shared_from_this();
    asio::async_read(_M_Socket, asio::buffer(buffer.data(), buffer.capacity()),
                     [_self, buffer, callback](std::error_code ec, size_t length) {
        if (ec) {
            _self->setConnectivityState(Connection::TransitentFailure);
        }
        callback(ec, length);
    });
}

void AsioTcpConnection::write(const ByteBuffer &buffer, const Connection::WriteCallback &callback)
{
    assert(callback != nullptr);
    if (callback == nullptr) {
        return;
    }

    int state = connectivityState();
    if (state != ConnectivityState::Ready) {
        if (callback)
            callback(asio::error::not_connected, 0);
        return;
    }

    auto _self = shared_from_this();
    asio::async_write(_M_Socket, asio::const_buffer(buffer.data(), buffer.length()),
                     asio::transfer_all(),
                     [_self, buffer, callback](std::error_code ec, size_t length) {
        if (ec) {
            _self->setConnectivityState(Connection::TransitentFailure);
        }
        callback(ec, length);
    });
}

std::error_code AsioTcpConnection::shutdown(const Connection::ShutdownType &type)
{
    asio::socket_base::shutdown_type typ = asio::socket_base::shutdown_both;
    if (type == ShutdownRead) {
        typ = asio::socket_base::shutdown_receive;
    } else if (type == ShutdownWrite) {
        typ = asio::socket_base::shutdown_send;
    }

    asio::error_code e;
    _M_Socket.shutdown(typ,e);

    if (type == ShutdownBoth) {
        setConnectivityState(Connection::Shutdown);
    }
    return e;
}

std::error_code AsioTcpConnection::close()
{
    std::error_code e;
    _M_Socket.shutdown(asio::socket_base::shutdown_both, e);
    _M_Socket.close(e);
    setConnectivityState(Connection::Shutdown);
    return e;
}

void AsioTcpConnection::onConnectivityStateChanged(const int &oldState, const int &currentState)
{
    UNUSED(oldState);

    if (currentState == ConnectivityState::Ready) {
        _M_RetryCount = 0;
        _M_RetryTimer.cancel();
    } else if (currentState == ConnectivityState::TransitentFailure) {
        if (_M_RetryCount < 30) {
            _M_RetryCount++;
        }

        auto _self = shared_from_this();
        _M_RetryTimer.cancel();
        _M_RetryTimer.expires_from_now(std::chrono::seconds(_M_RetryCount));
        _M_RetryTimer.async_wait([_self](asio::error_code e) {
            if (!e) {
                _self->retryConnect();
            }
        });
    } else if (currentState == ConnectivityState::Shutdown) {
        shutdownAction();
    }
}

void AsioTcpConnection::retryConnect()
{
    resetSocket();
    connectWithEndpointSequence(_M_EndpointList, nullptr);
}

void AsioTcpConnection::resetSocket()
{
    if (_M_Socket.is_open()) {
        ::asio::error_code ec;
        _M_Socket.shutdown(asio::socket_base::shutdown_both, ec);
        _M_Socket.close(ec);
        _M_Socket = protocol::socket(_M_Service->context().rawContext());
    }
}

void AsioTcpConnection::connectWithEndpointSequence(
        const AsioTcpConnection::AsioEndpointList &endpointSequence,
        const Connection::ConnectCallback &callback)
{
    auto _self = shared_from_this();
    _self->setConnectivityState(Connection::Connecting);
    asio::async_connect(_M_Socket, endpointSequence,
                        [_self, callback](std::error_code ec, protocol::endpoint e) {
        UNUSED(e);
        if (!ec) {
            _self->setConnectivityState(Connection::Ready);
        } else {
            _self->setConnectivityState(Connection::TransitentFailure);
        }

        if (callback != nullptr) {
            callback(ec);
        }
    });
}

void AsioTcpConnection::shutdownAction()
{
    _M_RetryTimer.cancel();
    _M_RetryCount = 0;
    if (_M_Socket.is_open()) {
        ::asio::error_code ec;
        _M_Socket.shutdown(asio::socket_base::shutdown_both, ec);
        _M_Socket.close(ec);
        _M_Socket = protocol::socket(_M_Service->context().rawContext());
    }
}

void AsioTcpConnection::resolvedEndpoint(std::error_code e, protocol::resolver::iterator epIt)
{
    if (!e) {
        auto endIt = protocol::resolver::iterator();
        for (auto it = epIt; it != endIt; ++it) {
            _M_EndpointList.push_back(it->endpoint());
        }
    }
}

CALLER_END
