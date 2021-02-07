#include <caller/call/socketeventrequest.hpp>
#include <caller/executor/threadexecutioncontext.hpp>

CALLER_BEGIN

SocketEventRequest::SocketEventRequest()
{

}

SocketEventRequest::~SocketEventRequest()
{

}

SocketEventRequestBase::SocketEventRequestBase() {}

SocketEventFuncRequestPtr SocketEventFuncRequest::create(const WriteCallback &writeCallback,
                                                         const ReadCallback &readCallback,
                                                         const ErrorCallback &errorCallback,
                                                         ExecutionContext *executionContext)
{
  return NewRefPtr<SocketEventFuncRequest>(writeCallback, readCallback, errorCallback, executionContext);
}

SocketEventFuncRequest::SocketEventFuncRequest(const SocketEventFuncRequest::WriteCallback &writeCallback,
                                                 const SocketEventFuncRequest::ReadCallback &readCallback,
                                                 const SocketEventFuncRequest::ErrorCallback &errorCallback,
                                                 ExecutionContext *executionContext)
    : _M_ExecutionContext(executionContext)
    , _M_WriteCallback(writeCallback)
    , _M_ReadCallback(readCallback)
    , _M_ErrorCallback(errorCallback)
{
  if (_M_ExecutionContext == nullptr) {
    _M_ExecutionContext = ThreadExecutionContext::executionContext();
  }
}

SocketEventMonitoredRequestPtr SocketEventMonitoredRequest::create()
{
  return NewRefPtr<SocketEventMonitoredRequest>();
}

SocketEventMonitoredRequest::SocketEventMonitoredRequest()
{

}

CALLER_END
