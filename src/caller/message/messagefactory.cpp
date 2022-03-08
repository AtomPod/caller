#include <caller/message/messagefactory.hpp>
#include <assert.h>

CALLER_BEGIN

static MessageMeta::AttributeValue emptyAttributeValue;

MessageFactoryPtr MessageFactory::make()
{
    return NewRefPtr<MessageFactory>();
}

MessageFactory::MessageFactory()
{

}

MessageFactory::~MessageFactory()
{
    Locker locker(_M_Mutex);
    for (auto meta : _M_MessageMetas) {
         delete meta.second;
    }
}

void   MessageFactory::registerMessageMeta(MessageMeta *meta)
{
    if (meta == nullptr) {
        return;
    }

    Locker locker(_M_Mutex);
    _M_MessageMetas.insert(MapValue(meta->id(), meta));
}

MessagePtr MessageFactory::messageById(const MessageFactory::ID &id)
{
    MessageMeta *meta = messageMetaById(id);
    MessageMeta::Type *typ = nullptr;
    if (meta != nullptr && (typ = meta->type()) != nullptr) {
        return typ->create();
    }
    return nullptr;
}

MessageMeta *MessageFactory::messageMetaById(const MessageFactory::ID &id)
{
    Locker locker(_M_Mutex);
    auto it = _M_MessageMetas.find(id);
    if (it != _M_MessageMetas.end()) {
        return  it->second;
    }

    return nullptr;
}

MessageMeta::MessageMeta(const ID &id, MessageMeta::Type *typ) : _M_ID(id), _M_Type(typ)
{
    assert(typ != nullptr);
}

MessageMeta::~MessageMeta()
{

}

MessageMeta::Type *MessageMeta::type()
{
   return _M_Type.get();
}

ID MessageMeta::id()
{
    return _M_ID;
}

void MessageMeta::attributes(const std::initializer_list<MessageMeta::AttributeName> &attributes, MessageMeta::AttributeValues &values)
{
    values.clear();
    if (attributes.size() == 0)
        return;

    std::unique_lock<std::mutex> locker(_M_Mutex);
    for (auto attr : attributes) {
        auto found = _M_Attributes.find(attr);
        if (found != _M_Attributes.end()) {
            values.push_back(found->second);
        }
    }
}

MessageMeta::AttributeValue MessageMeta::attribute(const MessageMeta::AttributeName &name, bool *exist)
{
    std::unique_lock<std::mutex> locker(_M_Mutex);

    auto found = _M_Attributes.find(name);
    if (found == _M_Attributes.end()) {
        if (exist != nullptr) {
            *exist = false;
        }
        return emptyAttributeValue;
    }

    if (exist != nullptr) {
        *exist = true;
    }
    return found->second;
}

MessageMeta *MessageMeta::setAttribute(const MessageMeta::AttributeName &name, const MessageMeta::AttributeValue &value)
{
    std::unique_lock<std::mutex> locker(_M_Mutex);
    _M_Attributes.insert_or_assign(name, value);
    return this;
}

MessageMeta *MessageMeta::setAttribute(const MessageMeta::AttributeName &name, MessageMeta::AttributeValue &&value)
{
    std::unique_lock<std::mutex> locker(_M_Mutex);
    _M_Attributes.insert_or_assign(name, std::move(value));
    return this;
}

CALLER_END
