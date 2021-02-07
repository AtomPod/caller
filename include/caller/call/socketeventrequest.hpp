#ifndef CALLER_SOCKETEVENTREQUEST_HPP
#define CALLER_SOCKETEVENTREQUEST_HPP
#include <caller/core.hpp>
#include <caller/common/error.hpp>
#include <caller/common/refptr.hpp>
#include <caller/common/any.hpp>
#include <caller/executor/executor.hpp>
#include <caller/async/future.hpp>
#include <caller/call/declare.hpp>
#include <functional>

CALLER_BEGIN

/**
 * @brief The SocketEventListener class 监听socket完成后的响应
 * 				注：由于是通过any进行传输，所以传输时，只能传入SocketEventListener指针
 * 					 否则无法识别，即子类指针需要转换为SocketEventListener*再使用any进行传输
 */
class SocketEventRequest : public std::enable_shared_from_this<SocketEventRequest> {
 public:
  enum ErrorType {
    ERROR_TYPE_WRITE,
    ERROR_TYPE_READ
  };
 public:
  SocketEventRequest();
  virtual ~SocketEventRequest();
 public:
  virtual void writeComplete(int64_t bytes) = 0;
  virtual void readComplete(int64_t bytes) = 0;
  virtual void error(const ErrorType &errType, Error err) = 0;
  virtual any  extraData() = 0;

  template<typename T>
  inline T typedExtraData() {
    any data = extraData();
    T typedData = CALLER any_cast<T>(data);
    return typedData;
  }
};

class SocketEventRequestBase : public SocketEventRequest {
 public:
  SocketEventRequestBase();
 public:
  virtual void writeComplete(int64_t) override {}
  virtual void readComplete(int64_t) override {}
  virtual void error(const ErrorType &, Error) override {}
  virtual any  extraData() override { return _M_ExtraData; }
 public:
  void setExtraData(const any &extraData) { _M_ExtraData = extraData; }
 private:
  any _M_ExtraData;
};

class SocketEventMonitoredRequest : public SocketEventRequestBase {
 public:
  struct ErrorArguments {
    ErrorType errorType;
    Error error;
  };
 public:
  static SocketEventMonitoredRequestPtr create();
 public:
  SocketEventMonitoredRequest();
 public:
  Future<int64_t> writeCompleteFuture() { return _M_WritePromise.future(); }
  Future<int64_t> readCompleteFuture() { return _M_ReadPromise.future(); }
  Future<ErrorArguments> errorFuture() { return _M_ErrorPromise.future(); }

 protected:
  virtual void writeComplete(int64_t bytes) override { _M_WritePromise.reportResult(bytes); }
  virtual void readComplete(int64_t bytes) override { _M_ReadPromise.reportResult(bytes); }
  virtual void error(const ErrorType &errType, Error err) override { _M_ErrorPromise.reportResult({errType, err}); }

 private:
  FutureInterface<int64_t> _M_WritePromise;
  FutureInterface<int64_t> _M_ReadPromise;
  FutureInterface<ErrorArguments> _M_ErrorPromise;
};

class SocketEventFuncRequest : public SocketEventRequestBase {
 public:
  typedef std::function<void(int64_t)> WriteCallback;
  typedef std::function<void(int64_t)> ReadCallback;
  typedef std::function<void(const ErrorType &, Error)> ErrorCallback;
 public:
  static SocketEventFuncRequestPtr create(const WriteCallback &writeCallback,
                                          const ReadCallback &readCallback,
                                          const ErrorCallback &errorCallback,
                                          ExecutionContext *executionContext = nullptr);
 public:
  SocketEventFuncRequest(const WriteCallback &writeCallback,
                          const ReadCallback &readCallback,
                          const ErrorCallback &errorCallback,
                          ExecutionContext *executionContext = nullptr);
 public:
  virtual void writeComplete(int64_t bytes) override {
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
  virtual void readComplete(int64_t bytes) override {
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
  virtual void error(const ErrorType &errType, Error e) override {
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
 private:
  ExecutionContext *_M_ExecutionContext;
  WriteCallback _M_WriteCallback;
  ReadCallback  _M_ReadCallback;
  ErrorCallback _M_ErrorCallback;
};

CALLER_END

#endif // SOCKETEVENTLISTENER_HPP
