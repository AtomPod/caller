#include <caller/message/messagefactory.hpp>

CALLER_BEGIN

MessageFactoryPtr MessageFactory::make()
{
    return std::make_shared<MessageFactory>();
}

MessageFactory::MessageFactory()
{

}

void MessageFactory::addGenerator(const MessageFactory::ID &id, const MessageFactory::MessageGenerator &body)
{
    Locker locker(_M_Mutex);
    _M_Generators.insert(MapValue(id, body));
}

MessagePtr MessageFactory::generate(const MessageFactory::ID &id)
{
    MessageGenerator generator;
    {
        Locker locker(_M_Mutex);
        auto it = _M_Generators.find(id);
        if (it != _M_Generators.end()) {
            generator = it->second;
        }
    }

    if (generator != nullptr) {
        return generator();
    }
    return nullptr;
}

CALLER_END
