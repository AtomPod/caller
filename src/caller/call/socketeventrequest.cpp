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

void SocketEventFuncRequest::writeComplete(int64_t bytes) {
  if (_M_WriteCallback != nullptr) {
    if (_M_ExecutionContext != nullptr) {
      WriteCallback callback = _M_WriteCallback;
      _M_ExecutionContext->executor()->execute([callback, bytes]() {
        callback(bytes);
      });
    } else {
      _M_WriteCallback(bytes);
    }
  }
}

void SocketEventFuncRequest::readComplete(int64_t bytes) {
  if (_M_ReadCallback != nullptr) {
    if (_M_ExecutionContext != nullptr) {
      ReadCallback callback = _M_ReadCallback;
      _M_ExecutionContext->executor()->execute([callback, bytes]() {
        callback(bytes);
      });
    } else {
      _M_ReadCallback(bytes);
    }
  }
}

void SocketEventFuncRequest::error(const SocketEventRequest::ErrorType &errType, Error e) {
  if (_M_ErrorCallback != nullptr) {
    if (_M_ExecutionContext != nullptr) {
      ErrorCallback callback = _M_ErrorCallback;
      _M_ExecutionContext->executor()->execute([callback, errType, e]() {
        callback(errType, e);
      });
    } else {
      _M_ErrorCallback(errType, e);
    }
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
