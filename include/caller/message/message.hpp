#ifndef CALLER_MESSAGE_HPP
#define CALLER_MESSAGE_HPP

#include <caller/core.hpp>
#include <caller/message/declare.hpp>
#include <caller/common/id.hpp>
#include <caller/common/serializable.hpp>

CALLER_BEGIN

/**
 * @brief The Message class 消息类，用于解析使用，外部基础该类，
 *        传入request或者response中，在解析的时候，可通过内部解析
 *        这样可通过模板等基础，在外部直接给予body解析，而内部通过一个
 *        外部构建器，用来解析header并传回该类中，或者通过一个中间件
 *        直接读取对应解析ID后再进行判断给内部解析，这取决于是否使用
 *        中间件来处理
 */
class Message : public Serializable
{
public:
    template<typename T>
    static MessagePtr create() {
        return NewRefPtr<T>();
    }

public:
    Message();
    virtual ~Message();
public:
    virtual ID          id() const = 0;
    virtual ID          sequenceNumber() const = 0;
    virtual void        setSequenceNumber(const ID &id) = 0;
};

CALLER_END

#endif // MESSAGE_HPP
