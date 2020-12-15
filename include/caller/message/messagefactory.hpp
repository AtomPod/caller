#ifndef CALLER_MESSAGEFACTORY_HPP
#define CALLER_MESSAGEFACTORY_HPP

#include <caller/core.hpp>
#include <unordered_map>
#include <functional>
#include <mutex>
#include <caller/message/declare.hpp>
#include <caller/common/id.hpp>

CALLER_BEGIN

class MessageFactory
{
public:
    typedef std::function<MessagePtr()> MessageGenerator;
private:
    typedef CALLER ID ID;
    typedef std::unordered_map<ID, MessageGenerator> GeneratorMap;
    typedef GeneratorMap::value_type  MapValue;
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
    ~MessageFactory() = default;
public:
    void            addGenerator(const ID &id, const MessageGenerator &body);
    MessagePtr      generate(const ID &id);
private:
    Mutex            _M_Mutex;
    GeneratorMap     _M_Generators;
};

CALLER_END

#endif // MESSAGEFACTORY_HPP
