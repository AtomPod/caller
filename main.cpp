#include <iostream>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/executor/threadexecutioncontext.hpp>
#include <caller/call/pipelinewritestage.hpp>
#include <caller/call/pipelinereadstage.hpp>
#include <caller/call/pipelinewritereadstage.hpp>
#include <caller/call/pipeline.hpp>
#include <caller/call/asio/tcpsockethandler.hpp>
#include <caller/call/iopipelinecontext.hpp>
#include <caller/call/asio/udpsockethandler.hpp>
#include <caller/common/ringbuffer.hpp>
#include <caller/common/endian.hpp>
#include <caller/call/pipelinestage/delimiterbasedframedecoder.hpp>
#include <caller/call/pipelinestage/delimiterbasedframeencoder.hpp>
#include <string>
#include <caller/route/broadcastrouter.hpp>
#include <caller/route/event.hpp>
#include <caller/route/routefunc.hpp>
#include <caller/route/temporaryrouter.hpp>
#include <caller/message/messagefactory.hpp>
#include <caller/route/persistencerouter.hpp>
#include <caller/executor/singlethreadexecution.hpp>
#include <caller/async/futureinterface.hpp>
#include <caller/async/futureeventlistener.hpp>
#include <asio/posix/stream_descriptor.hpp>
#include <caller/call/socketeventrequest.hpp>

using namespace std;

USING_NAMESPACE_CALLER

struct Data {
    int age;
};

class Test2Read : public PipelineMultiTypeReadStage<int, double> {
 protected:
    virtual void handleTypeRead(const PipelineContextPtr& context, const ByteBuffer& buffer,
                                const double  &object) override {
      invokeReader(context, buffer, context);
    }

    virtual void handleTypeRead(const PipelineContextPtr& context, const ByteBuffer& buffer,
                                const int  &object) override {

    }
};


const Test2Read *r = new Test2Read();

class TestReadPipeline : public CALLER PipelineReadStage
{
public:
    TestReadPipeline() {}
protected:
    virtual void pipelineActive(const PipelineContextPtr & context)         {
        std::cout << "connected\n";
        auto message = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection:Keep-Alive\r\n\r\n";

        CALLER ByteBuffer buffer;
        buffer.copy(message, strlen(message));
        context->write(any(), buffer);
    }

    virtual void pipelineInactive(const PipelineContextPtr & context) {
        std::cout << "disconnected\n";

        CALLER Endpoint endpoint;
        endpoint.setHost("www.baid.com");
        endpoint.setPort("80");

        context->connect(endpoint).addListener(
                    CreateFutureEventListenerFunction([](const FutureEvent &e) {
            if (e.isFinished()) {
                std::cout << "finished: connected\n";
            } else if (e.isError()) {
                Error ec = e.errorCode();
                std::cout << "connect:" << ec.message() << std::endl;
            }
        }));

        invokePipelineInactive(context);

        //        context->connect(endpoint).whenCanceled([](const std::error_code &ec, const std::exception_ptr &e) {
        //            std::cout << "connect:" << ec.message() << std::endl;
        //        }).whenFinished([]() {
        //            std::cout << "finished: connected\n";
        //        });
    }

    virtual void pipelineReadComplete(const PipelineContextPtr & ctx)   {
        std::cout << "read complete\n";

        ctx->disconnect().addListener(
                    CreateFutureEventListenerFunction([](const FutureEvent &e) {
            if (e.isError()) {
                Error ec = e.errorCode();
                std::cout << "disconnected:" << ec.message() << std::endl;
            }
        })
                    );
        //        ctx->disconnect().whenCanceled([](const std::error_code &ec, const std::exception_ptr &e) {
        //            std::cout << "disconnected: " << ec.message() << std::endl;
        //        });
    }

    virtual void pipelineWriteComplete(const PipelineContextPtr &)   {
        std::cout << "write complete\n";
    }

    virtual void handleRead(const PipelineContextPtr &context,
                             const ByteBuffer &buffer,
                            const any &object) override {
        std::cout << StringView(buffer.data(), buffer.readableLength());
        invokeReader(context, buffer, object);
    }

    virtual void causeException(const PipelineContextPtr & ctx, const std::exception &e) override {
        std::cout << "exception: " << e.what() << '\n';
        invokeCauseException(ctx, e);
    }

};


class NewMessage : public CALLER Message {
public:
    NewMessage() : mid(static_cast<uint16_t>(messageId())), length(10) {

    }

    ~NewMessage() override {

    }

    static  ID        messageId() {
        return 0x19;
    }

    virtual ID          id() const  {
        return messageId();
    }

    virtual ID          sequenceNumber() const {
        return seq;
    }

    virtual void        setSequenceNumber(const ID &id) {
        setSeq(id);
    }

    virtual     Error   serialize(ByteBuffer &buffer)   const  {
        buffer.put(pif);
        buffer.put(length);

        ByteBuffer lengthBuff = buffer.slice(buffer.writeIndex() - sizeof(length), sizeof(length), false);

        buffer.put(mid);
        buffer.put(seq);
        buffer.put(age);
        buffer.put(StringView(name.data(), name.size()));
        buffer.put(eif);

        uint16_t len = static_cast<uint16_t>(buffer.readableLength());
        lengthBuff.put(len);
        return Error();
    }

    virtual     Error   deserialize(ByteBuffer buffer)  {
        uint16_t nameSize = 0;
        buffer.take(pif);
        buffer.take(length);
        buffer.take(mid);
        buffer.take(seq);
        buffer.take(age);
        buffer.take(nameSize);
        name.resize(nameSize);

        StringView view(name.data(), name.size());
        buffer.take(view, nameSize);
        buffer.take(eif);

        return Error();
    }

    uint16_t getPif() const;
    void setPif(const uint16_t &value);

    uint16_t getLength() const;
    void setLength(const uint16_t &value);

    uint16_t getMid() const;

    uint16_t getEif() const;
    void setEif(const uint16_t &value);

    uint16_t getAge() const;
    void setAge(const uint16_t &value);

    std::string getName() const;
    void setName(const std::string &value);

    uint16_t getSeq() const;
    void setSeq(const uint16_t &value);

private:
    uint16_t    pif;
    uint16_t    length;
    uint16_t    mid;
    uint16_t    seq;
    uint16_t    age;
    std::string name;
    uint16_t eif;
};


struct Message {
    uint16_t pif;
    uint16_t length;
    uint16_t mid;
    uint16_t eif;
};

struct Request {
    RefPtr<NewMessage> object;
    std::function<void(EventPtr)> callback;
};

class MessageRouter : public PipelineMultiTypeWriteStage<::Request, SocketEventMonitoredRequestPtr > {
public:
    MessageRouter(RouterPtr append) : _M_appendRouter(append) {

    }

    virtual void handleTypeWrite(const PipelineContextPtr & context, ByteBuffer & buffer, const ::Request &object) override {
        static int seq = 1;
        object.object->setSequenceNumber(seq++);

        std::cout << "invoke request\n";

        RouterPtr router = _M_appendRouter;
        SocketEventFuncRequestPtr listener = SocketEventFuncRequest::create([router, object](int64_t bytes) {
          std::cout << "add event\n";
          RoutePtr route = CreateRouteFuncInvoker(object.callback);
          route->setID(object.object->getMid());
          route->setSequenceNumber(object.object->getSeq());
          router->add(route);
        }, nullptr, nullptr);
        listener->setExtraData(object.object);
        invokeWriter(context, buffer,  StaticCastRefPtr<SocketEventRequest>(listener));
    }

    virtual void handleTypeWrite(const PipelineContextPtr & context, ByteBuffer & buffer,
                                 const SocketEventMonitoredRequestPtr &request) override {
      static int seq = 1;
      auto object = request->typedExtraData<::Request>();
      object.object->setSequenceNumber(seq++);

      RouterPtr router = _M_appendRouter;
      request->writeCompleteFuture().addListener(
        CreateFutureEventListenerFunction(
              [router, object](FutureEvent e) {
                RoutePtr route = CreateRouteFuncInvoker(object.callback);
                route->setID(object.object->getMid());
                route->setSequenceNumber(object.object->getSeq());
                router->add(route);
              }
        )
      );
      request->setExtraData(object.object);
      std::cout << "add socket event\n";

//      RouterPtr router = _M_appendRouter;
//      SocketEventFuncRequest *listener = new SocketEventFuncRequest([router, object](int64_t bytes) {
//        std::cout << "add event\n";
//        RoutePtr route = CreateRouteFuncInvoker(object.callback);
//        route->setID(object.object->getMid());
//        route->setSequenceNumber(object.object->getSeq());
//        router->add(route);
//      }, nullptr, nullptr);
//      listener->setExtraData(object.object);
      invokeWriter(context, buffer, StaticCastRefPtr<SocketEventRequest>(request));
  }
private:
    RouterPtr _M_appendRouter;
};

class MessageReadRouter : public PipelineTypeReadStage< std::vector<MessagePtr> > {
public:
    MessageReadRouter(RouterPtr run) : _M_runRouter(run) {

    }

    virtual void handleTypeRead(const PipelineContextPtr & context, const ByteBuffer & buffer, const std::vector<MessagePtr> &msg) override {
        for (auto m : msg) {
            EventPtr e = Event::make(m->id(), m->sequenceNumber(), m);
            _M_runRouter->post(e);
        }

        invokeReader(context, buffer, msg);
    }

private:
    RouterPtr _M_runRouter;
};

class MessageDelimiterBaseFrameDecoder : public DelimiterBasedFrameDecoder<uint16_t, uint16_t, MessagePtr, std::vector<MessagePtr>>
{
public:
    MessageDelimiterBaseFrameDecoder(MessageFactoryPtr factory) : DelimiterBasedFrameDecoder(8, 2, 0x1412, 0x1212, 4096), _M_MsgFactory(factory) {

    }

    virtual void handleRead(const PipelineContextPtr &context, const ByteBuffer &buffer, const any &object) override  {
        std::chrono::steady_clock::time_point beg = std::chrono::steady_clock::now();
        DelimiterBasedFrameDecoder::handleRead(context, buffer, object);
        auto end = std::chrono::steady_clock::now();

        std::cout << "d: " << (end - beg).count() << '\n';

    }

protected:
    virtual MessagePtr decode(const PipelineContextPtr & context,  ByteBuffer pack) {

        uint16_t mid = 0;
        size_t rindex = pack.readIndex();
        pack.setReadIndex(rindex + 4);
        pack.take(mid);
        pack.setReadIndex(rindex);

        MessagePtr msg = _M_MsgFactory->messageById(mid);
        if (msg != nullptr) {
            if (!msg->deserialize(pack)) {
                return msg;
            }
        }
        return nullptr;
    }

    virtual void causeException(const PipelineContextPtr & ctx, const std::exception &e) override {
        std::cout << "cause exeception: " << e.what() << '\n';
    }
private:
    MessageFactoryPtr _M_MsgFactory;
};


class MessagePrinter : public PipelineTypeReadStage<::Message*>
{
public:
    void handleTypeRead(const PipelineContextPtr &  context, const ByteBuffer &buffer, ::Message *object) {
        std::cout << "message: " << object->mid << '\n';

        if (nextStage()) {
            nextStage()->handleRead(context, buffer , object);
        }
    }
};


class OldMessage : public CALLER Message {
public:
    static  ID        messageId() {
        return 0x12;
    }

    virtual ID          id() const  {
        return messageId();
    }

    virtual     Error   serialize(ByteBuffer &buffer)   const  {
        return Error();
    }

    virtual     Error   deserialize(const ByteBuffer &buffer)  {
        return Error();
    }
};


class MessageDelimiterBaseFrameEncoder : public DelimiterBasedFrameEncoder< SocketEventRequestPtr >
{
public:
    MessageDelimiterBaseFrameEncoder() {

    }
public:

    virtual bool encode(const PipelineContextPtr & context,  SocketEventRequestPtr request , ByteBuffer &pack) override {
        RefPtr<NewMessage> object = request->typedExtraData<RefPtr<NewMessage>>();
        std::chrono::steady_clock::time_point beg = std::chrono::steady_clock::now();

        object->serialize(pack);

        auto end = std::chrono::steady_clock::now();

        std::cout << "size: " << pack.readableLength() << ", d: " << (end - beg).count() << '\n';
        return true;
    }
};


class Empty : public PipelineWriteStage
{
public:
    Empty() {}

    virtual void pipelineActive(const PipelineContextPtr &ctx) {
        std::cout << "connected\n";

        for (auto i = 0; i < 2; ++i) {

            RefPtr< NewMessage > msg = NewRefPtr< NewMessage >();
            msg->setPif(0x1412);
            msg->setEif(0x1212);
            msg->setAge(i);
            msg->setSeq(i + 1);
            msg->setName("hello: " + std::to_string(i));

            ::Request r;
            r.object = msg;

            auto now = std::chrono::steady_clock::now();
            r.callback = [now](EventPtr ptr) {
                auto end = std::chrono::steady_clock::now();
                auto msg = CALLER any_cast<MessagePtr>(ptr->payload());
                std::cout << "recv message: " << msg->sequenceNumber() << ", ns: " << (end - now).count() << '\n';
            };

            ctx->write(r, ByteBuffer(512));
        }

    }
};

void printRange(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    printf("time start: %llu, time end: %llu, dt: %llu\n", start.time_since_epoch().count(),
           end.time_since_epoch().count(), (end - start).count());
}

static int count = 0;
static std::chrono::high_resolution_clock::time_point now = std::chrono::high_resolution_clock::now();

int main()
{
#if 1

    CALLER AsioExecutionContext executionContext;
    CALLER ThreadExecutionContext tec(executionContext);

    CALLER RouterPtr brouter = CALLER Router::create<BroadcastRouter>();
    CALLER RouterPtr trouter = CALLER Router::create<TemporaryRouter>();
    CALLER RouterPtr router  = CALLER Router::create<PersistenceRouter>();

    brouter->add(router);
    brouter->add(trouter);

    CALLER IOHandler* socketHandler = new CALLER AsioUdpSocketHandler(executionContext);
    CALLER PipelineContextPtr pipelineContext = CALLER IOPipelineContext::make(socketHandler);

    auto rpipe = pipelineContext->pipeline();
    rpipe->append(PipelineStage::create<Empty>());

    rpipe->append(PipelineStage::create<MessageRouter>(trouter));
    rpipe->append(PipelineStage::create<MessageDelimiterBaseFrameEncoder>());

    CALLER MessageFactoryPtr factory = CALLER MessageFactory::make();
    factory->registerMessageMeta(CALLER MessageMeta::create<NewMessage>());

    rpipe->append(PipelineStage::create<MessageReadRouter>(brouter));
    rpipe->append(PipelineStage::create<MessageDelimiterBaseFrameDecoder>(factory));

    CALLER Endpoint endpoint;
    endpoint.setHost("127.0.0.1");
    endpoint.setPort("9097");

    auto future = pipelineContext->connect(endpoint);

    std::thread t1([future, pipelineContext]() mutable {
//        CALLER AsioExecutionContext executionContext;
//        CALLER ThreadExecutionContext tec(executionContext);
//        CALLER SingleThreadExecutor<AsioExecutionContext> executor;
//        CALLER ThreadExecutionContext tec(*executor.context());
        future.wait();
//        std::cout << "start run context in thread:" << executor.id() << '\n';

        std::string echo;
        std::cout << "enter echo: ";

        while (std::cin >> echo) {

            RefPtr< NewMessage > msg = NewRefPtr< NewMessage >();
            msg->setPif(0x1412);
            msg->setEif(0x1212);
            msg->setName("hello: " + echo);

            ::Request r;
            r.object = msg;

            auto now = std::chrono::steady_clock::now();
            r.callback = [now](EventPtr ptr) {
                auto end = std::chrono::steady_clock::now();
                auto msg = CALLER any_cast<MessagePtr>(ptr->payload());
                auto m   = StaticCastRefPtr<NewMessage>(msg);
                std::cout << "recv message: " << m->getName().c_str() << ", ns: " << (end - now).count() << '\n';
                std::cout << "running at thread: " << std::this_thread::get_id() << '\n';
            };

            pipelineContext->write(r, ByteBuffer(512));
            std::cout << "enter echo: ";
        }
    });

    std::thread t12([future, pipelineContext]() mutable {
//        CALLER AsioExecutionContext executionContext;
//        CALLER ThreadExecutionContext tec(executionContext);
//        CALLER SingleThreadExecutor<AsioExecutionContext> executor;
//        CALLER ThreadExecutionContext tec(*executor.context());
        future.wait();
//        std::cout << "start run context in thread:" << executor.id() << '\n';

        std::string echo = "atomic";

        while (true) {

            RefPtr< NewMessage > msg = NewRefPtr< NewMessage >();
            msg->setPif(0x1412);
            msg->setEif(0x1212);
            msg->setName("hello: " + echo);

            SocketEventMonitoredRequestPtr eventRequest = SocketEventMonitoredRequest::create();

            ::Request r;
            r.object = msg;

            auto now = std::chrono::steady_clock::now();
            r.callback = [now](EventPtr ptr) {
                auto end = std::chrono::steady_clock::now();
                auto msg = CALLER any_cast<MessagePtr>(ptr->payload());
                auto m   = StaticCastRefPtr<NewMessage>(msg);
                std::cout << "recv message: " << m->getName().c_str() << ", ns: " << (end - now).count() << '\n';
                std::cout << "running at thread: " << std::this_thread::get_id() << '\n';
            };
            eventRequest->setExtraData(r);
            pipelineContext->write(eventRequest, ByteBuffer(512));
            std::this_thread::sleep_for(std::chrono::milliseconds(300));
        }
    });

    executionContext.start();

    t1.join();
    std::cout << ::count << '\n';
#endif
    return 0;
}

uint16_t NewMessage::getPif() const
{
    return pif;
}

void NewMessage::setPif(const uint16_t &value)
{
    pif = value;
}

uint16_t NewMessage::getLength() const
{
    return length;
}

void NewMessage::setLength(const uint16_t &value)
{
    length = value;
}

uint16_t NewMessage::getMid() const
{
    return mid;
}


uint16_t NewMessage::getEif() const
{
    return eif;
}

void NewMessage::setEif(const uint16_t &value)
{
    eif = value;
}

uint16_t NewMessage::getAge() const
{
    return age;
}

void NewMessage::setAge(const uint16_t &value)
{
    age = value;
}

std::string NewMessage::getName() const
{
    return name;
}

void NewMessage::setName(const std::string &value)
{
    name = value;
}

uint16_t NewMessage::getSeq() const
{
return seq;
}

void NewMessage::setSeq(const uint16_t &value)
{
seq = value;
}
