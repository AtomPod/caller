#ifndef CALLER_SOCKETHANDLER_HPP
#define CALLER_SOCKETHANDLER_HPP
#include <caller/call/iohandler.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT SocketHandler : public IOHandler
{
public:
    enum ConnectivityState {
        Idle,
        Connecting,
        Ready,
        TransitentFailure,
        Shutdown
    };

public:
    SocketHandler();
    virtual ~SocketHandler();
};

CALLER_END

#endif // SOCKETHANDLER_HPP
