#ifndef CALLER_PIPELINEWRITESTAGE_HPP
#define CALLER_PIPELINEWRITESTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineWriteStage : public PipelineStage
{
public:
    PipelineWriteStage();
    virtual ~PipelineWriteStage() override = default;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypeWriteStage : public PipelineWriteStage
{
public:
    PipelineTypeWriteStage() = default;
    virtual ~PipelineTypeWriteStage() override = default;
public:
    virtual void handleWrite(const PipelineContextPtr &context, ByteBuffer & buffer, const any &object) override final {
        T typedObject = CALLER any_cast<T>(object);
        handleTypeWrite(context, buffer, typedObject);
    }
protected:
    virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer &buffer, const T &object) = 0;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypeWriteStage<T*> : public PipelineWriteStage {
public:
    PipelineTypeWriteStage() = default;
    virtual ~PipelineTypeWriteStage() override = default;
public:
    virtual void handleWrite(const PipelineContextPtr &context, ByteBuffer & buffer, const any &object) override final {
        T* typedObject = CALLER any_cast<T*>(object);
        handleTypeWrite(context, buffer, typedObject);
    }
protected:
    virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer & buffer, T *object) = 0;
};

template <typename T>
class CALLER_DLL_EXPORT PipelineTypeWriteHandlerBase {
 protected:
    void invokeHandler(const PipelineContextPtr &context, ByteBuffer &buffer,
                     typename std::conditional< std::is_pointer<T>::value, T, const T &>::type object) {
      handleTypeWrite(context, buffer, object);
    }

    virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer &buffer,
                                 typename std::conditional< std::is_pointer<T>::value, T, const T &>::type object) = 0;
};

template <typename ...Types>
class CALLER_DLL_EXPORT PipelineTypeWriteHandler;

template <typename T>
class CALLER_DLL_EXPORT PipelineTypeWriteHandler<T> : public PipelineTypeWriteHandlerBase<T> {

};

template <typename T, typename ...Types>
class CALLER_DLL_EXPORT PipelineTypeWriteHandler<T, Types...>
    : public PipelineTypeWriteHandler<Types...>
    , public PipelineTypeWriteHandlerBase<T> {

};

/**
 * @brief The PipelineMultiTypeWriteStage class 多类型处理模板类，支持多个类型自动分发到对应的处理函数中
 * 				例子：
 * 				class MessageWriteStage : public PipelineMultiTypeWriteStage<int, int*, double> {
 * 					public:
 * 						// 如果传入的值是int类型，那么自动转发到该函数
 * 						virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer & buffer, const int &object) {
 *
 *            }
 *
 *            // 如果传入的值是double类型，那么自动转发到该函数
 * 						virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer & buffer, const double &object) {
 *
 *            }
 *
 *            // 如果传入的值是int*类型，那么自动转发到该函数
 * 						virtual void handleTypeWrite(const PipelineContextPtr &context, ByteBuffer & buffer, int *object) {
 *
 *            }
 * 				}
 */
template <typename ...Types>
class CALLER_DLL_EXPORT PipelineMultiTypeWriteStage : public PipelineWriteStage, public PipelineTypeWriteHandler<Types...> {
 public:
    PipelineMultiTypeWriteStage() = default;
    virtual ~PipelineMultiTypeWriteStage() override = default;
 public:
    virtual void handleWrite(const PipelineContextPtr &context, ByteBuffer &buffer, const any &object) override final {
      invoke<Types...>(context, buffer, object);
    }

 protected:
    template <typename T>
    void invoke(const PipelineContextPtr &context, ByteBuffer &buffer, const any &object) {
      typename std::conditional< std::is_pointer<T>::value, T, const T &>::type arg = CALLER any_cast<T>(object);
      PipelineTypeWriteHandlerBase<T>::invokeHandler(context, buffer, arg);
    }

    template <typename T, typename ...Args, typename std::enable_if<sizeof ...(Args) != 0, int>::type = 0>
    void invoke(const PipelineContextPtr &context, ByteBuffer &buffer, const any &object) {
      try {
        typename std::conditional< std::is_pointer<T>::value, T, const T &>::type arg = CALLER any_cast<T>(object);
        PipelineTypeWriteHandlerBase<T>::invokeHandler(context, buffer, arg);
      }  catch (const std::exception &) {
        invoke<Args...>(context, buffer, object);
      }
    }
};

CALLER_END

#endif // CALLER_PIPELINEWRITESTAGE_HPP
