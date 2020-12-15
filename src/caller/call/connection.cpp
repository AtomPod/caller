#include <caller/call/connection.hpp>
#include <algorithm>

CALLER_BEGIN

Connection::Connection() : _M_ConnectivityState(ConnectivityState::Idle)
{

}

Connection::~Connection()
{

}

void Connection::transitentFailure()
{
    setConnectivityState(ConnectivityState::TransitentFailure);
}

int Connection::connectivityState() const
{
    return _M_ConnectivityState.load(std::memory_order_acquire);
}

Connection::StateDisconnectFunc Connection::connectStateSignal(const Connection::StateListener &listener)
{
    if (listener == nullptr)
        return nullptr;

    auto connection = _M_StateObserver.connect(listener);
    return [connection]() mutable {
        connection.disconnect();
    };
}

void Connection::notifyStateListener(const int &currentState)
{
    _M_StateObserver(currentState);
}

void Connection::setConnectivityState(const int &connectivityState)
{
    // 如果shutdown，那么改socket不能再次使用
    if (_M_ConnectivityState.load(std::memory_order_acquire) == ConnectivityState::Shutdown) {
        return;
    }

    int oldState = _M_ConnectivityState.exchange(connectivityState, std::memory_order_acq_rel);
    if (oldState != connectivityState) {
        onConnectivityStateChanged(oldState, connectivityState);
        notifyStateListener(connectivityState);
    }
}

Connection::Endpoint::Endpoint()
{

}

Connection::Endpoint::~Endpoint()
{

}

std::string Connection::Endpoint::host() const
{
    return _M_Host;
}

void Connection::Endpoint::setHost(const std::string &address)
{
    _M_Host = address;
}

std::string Connection::Endpoint::port() const
{
    return _M_Port;
}

void Connection::Endpoint::setPort(const std::string &port)
{
    _M_Port = port;
}

CALLER_END
