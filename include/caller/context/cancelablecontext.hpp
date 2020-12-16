#ifndef CALLER_CANCELABLECONTEXT_HPP
#define CALLER_CANCELABLECONTEXT_HPP
#include <caller/context/context.hpp>
#include <mutex>
#include <set>

CALLER_BEGIN

class CALLER_DLL_EXPORT CancelableContext : public ContextBase
{
    friend class Context;
    friend class ContextBase;

    template<typename T>
    friend class ::std::shared_ptr;

    typedef ::std::set<ContextPtr>  Childrens;
public:
    CancelableContext();
public:
    virtual ~CancelableContext() override;
protected:
    void    cancel();
};

CALLER_END

#endif // CALLER_CANCELABLECONTEXT_HPP
