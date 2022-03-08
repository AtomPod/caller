#include <caller/context/cancelablecontext.hpp>
#include <caller/context/error.hpp>

CALLER_BEGIN

CancelableContext::CancelableContext() : ContextBase()
{

}

CancelableContext::~CancelableContext()
{

}

void CancelableContext::cancel()
{
    _M_DoneSender.reportResult(makeContextError(ContextError::ContextCancel));
}

CALLER_END
