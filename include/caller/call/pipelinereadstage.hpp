#ifndef CALLER_PIPELINEREADSTAGE_HPP
#define CALLER_PIPELINEREADSTAGE_HPP
#include <caller/call/pipelinestage.hpp>

CALLER_BEGIN

class CALLER_DLL_EXPORT PipelineReadStage : public PipelineStage
{
public:
    PipelineReadStage();
    virtual ~PipelineReadStage() override = default;
};

template<typename T>
class PipelineTypeReadStage : public PipelineReadStage {
public:
    PipelineTypeReadStage() = default;
    virtual ~PipelineTypeReadStage() override = default;
public:
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        const T* typedObject = CALLER AnyCast<T>(&object);
        handleTypeRead(context, buffer, *typedObject);
    }
protected:
    virtual void handleTypeRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const T &object) = 0;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypeReadStage<T*> : public PipelineReadStage {
public:
    PipelineTypeReadStage() = default;
    virtual ~PipelineTypeReadStage() override = default;
public:
    virtual void handleRead(const PipelineContextPtr& context, const ByteBuffer& buffer, const Any &object) override final {
        T* typedObject = CALLER AnyCast<T*>(object);
        handleTypeRead(context, buffer, typedObject);
    }
protected:
    virtual void handleTypeRead(const PipelineContextPtr& context, const ByteBuffer& buffer, T *object) = 0;
};

template<typename T>
class CALLER_DLL_EXPORT PipelineTypeReadHandlerBase {
 protected:
  void invokeReadHandler(const PipelineContextPtr &context, const ByteBuffer &buffer,
                         typename std::conditional< std::is_pointer<T>::value, T, const T &>::type object) {
    handleTypeRead(context, buffer, object);
  }

  virtual void handleTypeRead(const PipelineContextPtr& context, const ByteBuffer& buffer,
                         typename std::conditional< std::is_pointer<T>::value, T, const T &>::type object) = 0;
};


template <typename ...Types>
class CALLER_DLL_EXPORT PipelineTypeReadHandler;

template <typename T>
class CALLER_DLL_EXPORT PipelineTypeReadHandler<T> : public PipelineTypeReadHandlerBase<T> {

};

template <typename T, typename ...Types>
class CALLER_DLL_EXPORT PipelineTypeReadHandler<T, Types...>
    : public PipelineTypeReadHandler<Types...>
    , public PipelineTypeReadHandlerBase<T> {

};

/**
 * @brief The PipelineMultiTypeReadStage class 多类型处理模板类，支持多个类型自动分发到对应的处理函数中
 * 				例子：
 * 				class MessageWriteStage : public PipelineMultiTypeReadStage<int, int*, double> {
 * 					public:
 * 						// 如果传入的值是int类型，那么自动转发到该函数
 * 						virtual void handleTypeRead(const PipelineContextPtr &context, ByteBuffer & buffer, const int &object) {
 *
 *            }
 *
 *            // 如果传入的值是double类型，那么自动转发到该函数
 * 						virtual void handleTypeRead(const PipelineContextPtr &context, ByteBuffer & buffer, const double &object) {
 *
 *            }
 *
 *            // 如果传入的值是int*类型，那么自动转发到该函数
 * 						virtual void handleTypeRead(const PipelineContextPtr &context, ByteBuffer & buffer, int *object) {
 *
 *            }
 * 				}
 */
template <typename ...Types>
class CALLER_DLL_EXPORT PipelineMultiTypeReadStage : public PipelineReadStage, public PipelineTypeReadHandler<Types...> {
 public:
    PipelineMultiTypeReadStage() = default;
    virtual ~PipelineMultiTypeReadStage() override = default;
 public:
    virtual void handleRead(const PipelineContextPtr &context, const ByteBuffer &buffer, const Any &object) override final {
      invokeBaseReader<Types...>(context, buffer, object);
    }

 protected:
    template <typename T>
    void invokeBaseReader(const PipelineContextPtr &context, const ByteBuffer &buffer, const Any &object) {
      typename std::conditional< std::is_pointer<T>::value, T, const T &>::type arg = CALLER AnyCast<T>(object);
      PipelineTypeReadHandlerBase<T>::invokeReadHandler(context, buffer, arg);
    }

    template <typename T, typename ...Args, typename std::enable_if<sizeof ...(Args) != 0, int>::type = 0>
    void invokeBaseReader(const PipelineContextPtr &context, const ByteBuffer &buffer, const Any &object) {
      try {
        typename std::conditional< std::is_pointer<T>::value, T, const T &>::type arg = CALLER AnyCast<T>(object);
        PipelineTypeReadHandlerBase<T>::invokeReadHandler(context, buffer, arg);
      }  catch (const std::exception &) {
        invokeBaseReader<Args...>(context, buffer, object);
      }
    }
};

CALLER_END

#endif // CALLER_PIPELINEREADSTAGE_HPP
