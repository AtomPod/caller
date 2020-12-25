#ifndef CALLER_CONTEXT_DECLARE_H
#define CALLER_CONTEXT_DECLARE_H
#include <caller/common/refptr.hpp>

CALLER_BEGIN

class Context;
typedef RefPtr<Context>         ContextPtr;
typedef WeakRefPtr<Context>     ContextWeakPtr;

class ValueContext;
typedef RefPtr<ValueContext>    ValueContextPtr;

class CancelableContext;
typedef RefPtr<CancelableContext>  CancelableContextPtr;

CALLER_END

#endif // CALLER_CONTEXT_DECLARE_H
