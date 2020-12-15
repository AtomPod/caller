#ifndef CALLER_CONTEXT_DECLARE_H
#define CALLER_CONTEXT_DECLARE_H
#include <memory>
#include <caller/core.hpp>

CALLER_BEGIN

class Context;
typedef std::shared_ptr<Context> ContextPtr;
typedef std::weak_ptr<Context>   ContextWeakPtr;

class ValueContext;
typedef std::shared_ptr<ValueContext>   ValueContextPtr;

class CancelableContext;
typedef std::shared_ptr<CancelableContext>  CancelableContextPtr;

CALLER_END

#endif // CALLER_CONTEXT_DECLARE_H
