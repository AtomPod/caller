#ifndef CALLER_CALLCONTEXT_HPP
#define CALLER_CALLCONTEXT_HPP
#include <caller/context/context.hpp>
#include <caller/call/declare.hpp>
#include <caller/common/id.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT IOContext : public Context, public std::enable_shared_from_this<IOContext>
{
public:
    enum Flags {
        SentFlag    = 1,
        ErrorFlag   = 2
    };
public:
    static IOContextPtr make(ContextPtr context = Context::backgroundContext());
public:
    IOContext(ContextPtr context = Context::backgroundContext());
    ~IOContext() override;
public:
    RequestPtr request() const;
    void setRequest(const RequestPtr &request);

    ResponsePtr response() const;
    void setResponse(const ResponsePtr &response);
public:
    virtual Error           Err()  override;
    virtual ValueType       Value(const KeyType &key) override;
    virtual OptionalTime    Deadline() override;
    virtual Channel         Done() override;

public:
    template<typename T>
    T       metadata(const KeyType &key, bool* exists = nullptr) const {
        bool _exists = false;
        ValueType value = _M_Context->Value(key);

        _exists = value.has_value();
        if (!_exists) {
            return value.convertTo<T>();
        }

        if (exists != nullptr) {
            *exists = _exists;
        }
        return T();
    }
public:
    /**
     * @brief setContext    设置当前的Context为ctx，不保证线程安全
     * @param ctx
     */
    void                    setContext(ContextPtr ctx);

    ContextPtr              context() const;

    /**
     * @brief withContext  新建一个IOContext并指定新context，不污染原本的IOContext
     * @param ctx
     * @return
     */
    IOContextPtr            withContext(ContextPtr ctx);

public:
    uint32_t flags() const;
    void     setFlags(const uint32_t &flags);
    void     addFlag(const uint32_t &flag);
    void     removeFlag(const uint32_t &flag);
    bool     hasFlag(const uint32_t &flag);

    Error   error() const;
    void    setError(const Error &error);
protected:
    virtual void            addChild(ContextPtr child)       override { UNUSED(child); }
    virtual void            removeChild(ContextPtr child)    override { UNUSED(child); }

private:
    RequestPtr                  _M_Request;
    ResponsePtr                 _M_Response;
    uint32_t                    _M_Flags;
    ContextPtr                  _M_Context;
    Error                       _M_Error;
};

CALLER_END

#endif // CALLER_CALLCONTEXT_HPP
