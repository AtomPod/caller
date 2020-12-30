#include <caller/call/asio/tcpsockethandler.hpp>
#include <caller/common/error.hpp>

CALLER_BEGIN

AsioTcpSocketHandler::AsioTcpSocketHandler(AsioExecutionContext &context)
    : _M_ExecutionContext(context), _M_Socket(context.rawContext()),
      _M_ConnState(ConnectivityState::Idle), _M_Resolver(context.rawContext()),
      _M_DataBuffer(8192)
{

}

AsioTcpSocketHandler::~AsioTcpSocketHandler()
{

}

Future<void> AsioTcpSocketHandler::bind(const Endpoint &endpoint)
{

    return Future<void>();
}

Future<void> AsioTcpSocketHandler::connect(const Endpoint &endpoint)
{
    FutureInterface<void> futureSender;
    if (_M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Connecting ||
            _M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Ready) {
        futureSender.reportErrorCode(std::make_error_code(std::errc::already_connected));
        return futureSender.future();
    }

    executor()->execute([this, endpoint, futureSender]() mutable {
        resetSocket();
        if (_M_EndpointList.size() == 0) {
            resolverAddress(endpoint, futureSender);
        } else {
            connectWithEndpointList(_M_EndpointList, futureSender);
        }
    });
    return futureSender.future();
}

Future<void> AsioTcpSocketHandler::disconnect()
{
    FutureInterface<void> futureSender;
    closeSocketWithState(futureSender, ConnectivityState::TransitentFailure);
    return futureSender.future();
}

Future<void> AsioTcpSocketHandler::close()
{
//    FutureInterface<void> futureSender;
    closeSocketWithState(_M_CloseFuture, ConnectivityState::Shutdown);
//    return futureSender.future();
    return _M_CloseFuture.future();
}

Future<size_t> AsioTcpSocketHandler::read(ByteBuffer buffer)
{
    FutureInterface<size_t> futureSender;
    readNext(buffer, futureSender);
    return futureSender.future();
}

Future<size_t> AsioTcpSocketHandler::write(const ByteBuffer &buffer)
{
    FutureInterface<size_t> futureSender;

    if (_M_ConnState.load(std::memory_order_acquire) != ConnectivityState::Ready) {
        futureSender.reportErrorCode(MakeError(GenericError::not_connected));
        return futureSender.future();
    }

    asio::async_write(_M_Socket, asio::const_buffer(buffer.data(), buffer.readableLength()),
                      asio::transfer_all(),
                      [buffer, futureSender, this](std::error_code ec, size_t length) mutable {
        if (!ec) {
            futureSender.reportResult(length);
        } else {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureSender.reportErrorCode(ec);
        }
    });
    return futureSender.future();
}

Executor *AsioTcpSocketHandler::executor()
{
    return _M_ExecutionContext.executor();
}

Future<void> AsioTcpSocketHandler::closeFuture()
{
    return _M_CloseFuture.future();
}

void AsioTcpSocketHandler::resolverAddress(const Endpoint &endpoint, FutureInterface<void> futureSender)
{
    Protocol::resolver::query query(endpoint.host(),endpoint.port());
    _M_Resolver.async_resolve(query,
                              [this, futureSender](std::error_code e,
                              Protocol::resolver::iterator epIt) mutable {
        if (e) {
            futureSender.reportErrorCode(e);
            return;
        }

        this->_M_EndpointList.clear();
        auto end = Protocol::resolver::iterator();
        for (auto begin = epIt; begin != end; ++begin) {
            this->_M_EndpointList.push_back(begin->endpoint());
        }
        this->connectWithEndpointList(this->_M_EndpointList, futureSender);
    });
}

void AsioTcpSocketHandler::resetSocket()
{
    ::asio::error_code ec;
    _M_Socket.shutdown(asio::socket_base::shutdown_both, ec);
    _M_Socket.close(ec);
    _M_Socket = Socket(_M_ExecutionContext.rawContext());
}

void AsioTcpSocketHandler::connectWithEndpointList(
        const AsioTcpSocketHandler::EndpointList &endpoints,
        FutureInterface<void> futureInterface)
{
    setConnectivityState(ConnectivityState::Connecting);
    asio::async_connect(_M_Socket, endpoints, [this, futureInterface](std::error_code ec, Protocol::endpoint e) mutable {
        UNUSED(e);
        if (ec) {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureInterface.reportErrorCode(ec);
            return;
        }
        setConnectivityState(ConnectivityState::Ready);
        futureInterface.reportFinished();
    });
}

bool AsioTcpSocketHandler::setConnectivityState(const SocketHandler::ConnectivityState &state)
{
    if (_M_ConnState.load(std::memory_order_acquire) == ConnectivityState::Shutdown) {
        return false;
    }

    _M_ConnState.store(state, std::memory_order_release);
    return true;
}

void AsioTcpSocketHandler::readNext(ByteBuffer buffer, FutureInterface<size_t> futureSender)
{
    if (_M_ConnState.load(std::memory_order_acquire) != ConnectivityState::Ready) {
        futureSender.reportErrorCode(MakeError(GenericError::not_connected));
        return;
    }

    _M_Socket.async_read_some(asio::buffer(buffer.data(), buffer.capacity()),
                              [buffer, futureSender, this](std::error_code ec, size_t length) mutable {
        if (ec) {
            setConnectivityState(ConnectivityState::TransitentFailure);
            futureSender.reportErrorCode(ec);
        } else {
            futureSender.reportResult(length);
        }
    });
}

bool AsioTcpSocketHandler::closeSocketWithState(FutureInterface<void> futureSender, const SocketHandler::ConnectivityState &state)
{
    if (setConnectivityState(state)) {
        executor()->execute([this, futureSender]() mutable {
            if (_M_Socket.is_open()) {
                std::error_code code;
                _M_Socket.shutdown(asio::socket_base::shutdown_both, code);
                _M_Socket.close(code);

                if (code) {
                    futureSender.reportErrorCode(code);
                } else {
                    futureSender.reportFinished();
                }
            }
        });
        return true;
    }
    futureSender.reportErrorCode(std::make_error_code(std::errc::not_connected));
    return false;
}

CALLER_END
