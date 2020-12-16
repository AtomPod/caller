#ifndef CALLER_DECLARE_HPP
#define CALLER_DECLARE_HPP

#include <caller/core.hpp>
#include <memory>

CALLER_BEGIN

class IOContext;
typedef std::shared_ptr<IOContext> IOContextPtr;
typedef std::weak_ptr<IOContext>   IOContextWeakPtr;

class Service;
typedef std::shared_ptr<Service> ServicePtr;

class Request;
typedef std::shared_ptr<Request> RequestPtr;

class Response;
typedef std::shared_ptr<Response> ResponsePtr;

class RequestResponse;
typedef std::shared_ptr<RequestResponse> RequestResponsePtr;

class PipelineStage;
typedef std::shared_ptr<PipelineStage> PipelineStagePtr;

class Pipeline;
typedef std::shared_ptr<Pipeline> PipelinePtr;

class AsioService;
typedef std::shared_ptr<AsioService> AsioServicePtr;

class AsioTcpConnection;
typedef std::shared_ptr<AsioTcpConnection> AsioTcpConnectionPtr;

class AsioTcpService;
typedef std::shared_ptr<AsioTcpService> AsioTcpServicePtr;

class Transport;
typedef std::shared_ptr<Transport> TransportPtr;

class PipelineContext;
typedef std::shared_ptr<PipelineContext> PipelineContextPtr;

CALLER_END

#endif // CALLER_DECLARE_HPP
