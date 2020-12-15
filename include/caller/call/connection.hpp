#ifndef CALLER_CONNECTION_HPP
#define CALLER_CONNECTION_HPP
#include <caller/core.hpp>
#include <system_error>
#include <functional>
#include <caller/common/bytebuffer.hpp>
#include <atomic>
#include <unordered_set>
#include <caller/common/signal.hpp>

CALLER_BEGIN

class Connection
{
public:
    typedef std::function<void(const int &currentState)> StateListener;
    typedef signal<int>                                  StateSignaler;
    typedef std::function<void()>                        StateDisconnectFunc;
public:
    enum ShutdownType {
        ShutdownRead  = 1,
        ShutdownWrite = 2,
        ShutdownBoth  = ShutdownRead | ShutdownWrite
    };
public:
    typedef std::function<void(std::error_code)>         ConnectCallback;
    typedef std::function<void(std::error_code, size_t)> ReadCallback;
    typedef std::function<void(std::error_code, size_t)> WriteCallback;
public:
    enum ConnectivityState {
        Idle,
        Connecting,
        Ready,
        TransitentFailure,
        Shutdown
    };

    class Endpoint
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
public:
    Connection();
    Connection(const Connection &)          = delete;
    Connection& operator=(const Connection&) = delete;
    virtual ~Connection();
public:
    virtual void connect(const Endpoint &endpoint, const ConnectCallback &callback) = 0;
    virtual void read(ByteBuffer buffer,  const ReadCallback &callback)         = 0;
    virtual void write(const ByteBuffer &buffer, const WriteCallback &callback) = 0;
    virtual std::error_code shutdown(const ShutdownType &type)                  = 0;
    virtual std::error_code close()                                             = 0;

public:
    virtual void transitentFailure();
    int          connectivityState() const;

public:
    StateDisconnectFunc    connectStateSignal(const StateListener &listener);

protected:
    void    notifyStateListener(const int &currentState);

protected:
    void setConnectivityState(const int &connectivityState);
    virtual void onConnectivityStateChanged(const int &oldState, const int &currentState) = 0;

private:
    std::atomic_int   _M_ConnectivityState;
    StateSignaler     _M_StateObserver;
};

CALLER_END

#endif // CONNECTION_HPP
