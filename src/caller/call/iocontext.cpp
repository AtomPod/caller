#include <caller/call/iocontext.hpp>
#include <caller/call/request.hpp>
#include <caller/call/response.hpp>

CALLER_BEGIN

IOContextPtr IOContext::make(ContextPtr ctx)
{
    auto context = std::make_shared<IOContext>(ctx);
    return context;
}

IOContext::IOContext(ContextPtr context) : _M_Flags(0), _M_Context(context)
{

}

IOContext::~IOContext()
{

}

RequestPtr IOContext::request() const
{
    return _M_Request;
}

void IOContext::setRequest(const RequestPtr &request)
{
    _M_Request = request;
}

ResponsePtr IOContext::response() const
{
    return _M_Response;
}

void IOContext::setResponse(const ResponsePtr &m_Response)
{
    _M_Response = m_Response;
}

Context::Error IOContext::Err()
{
    return _M_Context->Err();
}

Context::ValueType IOContext::Value(const Context::KeyType &key)
{
    return _M_Context->Value(key);
}

Context::OptionalTime IOContext::Deadline()
{
    return _M_Context->Deadline();
}

Context::Channel IOContext::Done()
{
    return _M_Context->Done();
}

void IOContext::setContext(ContextPtr ctx)
{
    _M_Context = ctx;
}

ContextPtr IOContext::context() const
{
    return _M_Context;
}

IOContextPtr IOContext::withContext(ContextPtr ctx)
{
    auto context = IOContext::make(ctx);
    context->setRequest(request());
    context->setResponse(response());
    context->setFlags(flags());
    return context;
}

uint32_t IOContext::flags() const
{
    return _M_Flags;
}

void IOContext::setFlags(const uint32_t &M_Flags)
{
    _M_Flags = M_Flags;
}

void IOContext::addFlag(const uint32_t &flag)
{
    _M_Flags |= flag;
}

void IOContext::removeFlag(const uint32_t &flag)
{
    _M_Flags &= ~flag;
}

bool IOContext::hasFlag(const uint32_t &flag)
{
    return (_M_Flags & flag) != 0;
}

IOContext::Error IOContext::error() const
{
    return _M_Error;
}

void IOContext::setError(const Error &error)
{
    addFlag(ErrorFlag);
    _M_Error = error;
}

CALLER_END
