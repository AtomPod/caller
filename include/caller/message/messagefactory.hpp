#ifndef CALLER_MESSAGEFACTORY_HPP
#define CALLER_MESSAGEFACTORY_HPP

#include <caller/core.hpp>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <caller/message/declare.hpp>
#include <caller/common/id.hpp>
#include <caller/message/message.hpp>
#include <typeinfo>

#ifdef __GNUC__

#if __GNUC__ >= 3
#include <cxxabi.h>
#define __HAVE_CXX_ABI_
#endif

#endif

CALLER_BEGIN

class MessageMeta
{
public:
    class Type
    {
    public:
        Type() {}
        virtual ~Type() {}
    public:
        virtual const 	   char *name() const = 0;
        virtual MessagePtr create()           = 0;
    };

    template<typename T>
    class MessageType : public Type {
    public:
        MessageType(const char *name = nullptr) : _M_name(name) {
            if (name == nullptr) {
#ifdef __HAVE_CXX_ABI_
                int status = 0;
                const std::type_info  &ti 	= typeid(T).name();
                const char			  *name = ti.name();
                _M_name 					= abi::__cxa_demangle(name, 0, 0, &status);
                if (_M_name == nullptr) {
                    _M_name = name;
                }
#else
                _M_name = typeid(T).name();
#endif
            }
        }
        virtual ~MessageType() {}
    public:
        virtual const 	    char* name() const { return _M_name; }
        virtual MessagePtr  create() 		   { return Message::create<T>(); }
    private:
        const char *_M_name;
    };

    typedef std::string AttributeName;
    typedef std::string AttributeValue;
    typedef std::unordered_map<AttributeName, AttributeValue> Attributes;
    typedef std::list<AttributeValue> AttributeValues;
public:
    template<typename T>
    static MessageMeta *create(const ID &id, const char *name = nullptr) {
        return new MessageMeta(id, new MessageType<T>(name));
    }

    template<typename T>
    static MessageMeta *create(const char *name = nullptr,
                               typename std::enable_if< std::is_function<decltype (T::messageId)>::value, void >::type* = nullptr) {
        return create<T>(T::messageId(), name);
    }
public:
    MessageMeta(const ID &id, Type *typ);
    ~MessageMeta();

public:
    Type 				   *type();
    ID    	 	 			id();
    void					attributes(const std::initializer_list<AttributeName> &attributes, AttributeValues &values);
    AttributeValue   		attribute(const AttributeName &name, bool *exist = nullptr);
    MessageMeta*			setAttribute(const AttributeName &name, const AttributeValue &value);
    MessageMeta*			setAttribute(const AttributeName &name, AttributeValue &&value);

private:
    ID  					_M_ID;
    std::unique_ptr<Type>	_M_Type;
    Attributes				_M_Attributes;
    std::mutex				_M_Mutex;
};

class MessageFactory
{
private:
    typedef CALLER ID ID;
    typedef std::unordered_map<ID, MessageMeta*> MessageMetaMap;
    typedef MessageMetaMap::value_type  MapValue;
    typedef std::mutex Mutex;
    typedef std::unique_lock<Mutex> Locker;
public:
    static MessageFactoryPtr make();
public:
    MessageFactory();
    MessageFactory(const MessageFactory &) = delete;
    MessageFactory( MessageFactory &&) = delete;
    MessageFactory &operator=(const MessageFactory&) = delete;
    MessageFactory &operator=(MessageFactory&&) = delete;
    ~MessageFactory();
public:
    void            registerMessageMeta(MessageMeta *meta);
    MessagePtr      messageById(const ID &id);
    MessageMeta*    messageMetaById(const ID &id);
private:
    Mutex              _M_Mutex;
    MessageMetaMap     _M_MessageMetas;
};

CALLER_END

#endif // MESSAGEFACTORY_HPP
