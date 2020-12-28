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

using namespace std;

USING_NAMESPACE_CALLER

struct Data {
    int age;
};


class TestReadPipeline : public CALLER PipelineReadStage
{
public:
    TestReadPipeline() {}
protected:
    virtual void pipelineActive(PipelineContextPtr context)         {
        std::cout << "connected\n";
        auto message = "GET / HTTP/1.1\r\nHost: www.baidu.com\r\nConnection:Keep-Alive\r\n\r\n";

        CALLER ByteBuffer buffer;
        buffer.copy(message, strlen(message));
        context->write(any(), buffer);
    }

    virtual void pipelineInactive(PipelineContextPtr context) {
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

        invokePipelineActive(context);

//        context->connect(endpoint).whenCanceled([](const std::error_code &ec, const std::exception_ptr &e) {
//            std::cout << "connect:" << ec.message() << std::endl;
//        }).whenFinished([]() {
//            std::cout << "finished: connected\n";
//        });
    }

    virtual void pipelineReadComplete(PipelineContextPtr ctx)   {
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

    virtual void pipelineWriteComplete(PipelineContextPtr)   {
        std::cout << "write complete\n";
    }

    virtual void handleRead(CALLER PipelineContextPtr context,
                                 CALLER ByteBuffer buffer,
                                 const any &object) override {
        std::cout << StringView(buffer.data(), buffer.length());
        invokeReader(context, buffer, object);
    }

    virtual void causeException(CALLER PipelineContextPtr ctx, const std::exception &e) override {
        std::cout << "exception: " << e.what() << '\n';
        invokeCauseException(ctx, e);
    }

};

struct Message {
    uint16_t pif;
    uint16_t length;
    uint16_t mid;
    uint16_t eif;
};

struct Request {
    ::Message *object;
    std::function<void(EventPtr)> callback;
};

class MessageRouter : public PipelineTypedWriteStage<::Request> {
public:
    MessageRouter(RouterPtr append) : _M_appendRouter(append) {

    }

    virtual void handleTypedWrite(PipelineContextPtr context, ByteBuffer buffer, const ::Request &object) override {
        _M_Route = CreateRouteFuncInvoker(object.callback);

        _M_Route->setID(object.object->mid);
        _M_Route->setSequenceNumber(0x12);

        std::cout << "write: " << _M_Route->id() << '\n';

        _M_appendRouter->add(_M_Route);

        invokeWriter(context, buffer, object.object);
    }

private:
    RouterPtr _M_appendRouter;
    RoutePtr  _M_Route;
};

class MessageReadRouter : public PipelineTypedReadStage<::Message*> {
public:
    MessageReadRouter(RouterPtr run) : _M_runRouter(run) {

    }

    virtual void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, ::Message* msg) override {
        EventPtr e = Event::make(msg->mid, 0x12, msg);
        _M_runRouter->post(e);

        invokeReader(context, buffer, msg);
    }

private:
    RouterPtr _M_runRouter;
};


class MessageDelimiterBaseFrameEncoder : public DelimiterBasedFrameEncoder<::Message*>
{
public:
    MessageDelimiterBaseFrameEncoder() {

    }
public:
    virtual bool encode(PipelineContextPtr context, ::Message* object , ByteBuffer &pack) override {
        pack.put(object->pif);
        pack.put(object->length);
        pack.put(object->mid);
        pack.put(object->eif);
        std::cout << "size: " << pack.length() << '\n';
        return true;
    }
};


class MessageDelimiterBaseFrameDecoder : public DelimiterBasedFrameDecoder<uint16_t, uint16_t>
{
public:
    MessageDelimiterBaseFrameDecoder() : DelimiterBasedFrameDecoder(6, 2, 0x1412, 0x1212, 4096) {

    }

protected:
    virtual any decode(PipelineContextPtr context,  ByteBuffer pack) {
        static int i = 0;
        ++i;

        ::Message* newMsg = new ::Message();
        pack.take(newMsg->pif, 0);
        pack.take(newMsg->length, 2);
        pack.take(newMsg->mid, 4);
        pack.take(newMsg->eif, 6);
        return newMsg;
    }
};


class MessagePrinter : public PipelineTypedReadStage<::Message*>
{
public:
    void handleTypedRead(PipelineContextPtr context, ByteBuffer buffer, ::Message *object) {
        std::cout << "message: " << object->mid << '\n';

        if (nextStage()) {
            nextStage()->handleRead(context, buffer , object);
        }
    }
};

class Empty : public PipelineWriteStage
{
public:
    Empty() {}

    virtual void pipelineActive(PipelineContextPtr ctx) {
        std::cout << "connected\n";

        for (auto i = 0; i < 2; ++i) {
            ::Message* msg = new ::Message();
            msg->pif = 0x1412;
            msg->eif = 0x1212;
            msg->length = sizeof(::Message);
            msg->mid    = uint16_t(0x12 + i);

            ::Request r;
            r.object = msg;
            r.callback = [](EventPtr ptr) {
                ::Message* msg = CALLER any_cast<::Message*>(ptr->payload());
                std::cout << "recv message: " << msg->mid << '\n';
            };

            ctx->write(r, ByteBuffer());
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

    virtual     Error   serialize(ByteBuffer buffer)   const  {
        return Error();
    }

    virtual     Error   deserialize(ByteBuffer buffer)  {
        return Error();
    }
};

class NewMessage : public CALLER Message {
public:
    static  ID        messageId() {
        return 0x15;
    }

    virtual ID          id() const  {
        return messageId();
    }

    virtual     Error   serialize(ByteBuffer buffer)   const  {
        return Error();
    }

    virtual     Error   deserialize(ByteBuffer buffer)  {
        return Error();
    }
};

void printRange(std::chrono::steady_clock::time_point start, std::chrono::steady_clock::time_point end) {
    printf("time start: %llu, time end: %llu, dt: %llu\n", start.time_since_epoch().count(),
              end.time_since_epoch().count(), (end - start).count());
}

int main()
{
//    FutureInterface<std::chrono::steady_clock::time_point> interface;

////    for (auto i = 0; i < 15; ++i) {
////        interface.future().whenFinished([](std::chrono::steady_clock::time_point beg) {
////            auto end = std::chrono::steady_clock::now();
////            std::cout << "endi: " << (end - beg).count() << '\n';
////        });
////    }

//    std::thread t1([interface]() mutable {
//        interface.reportResult(std::chrono::steady_clock::now());
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//    });

//    t1.join();

//    return 0;
#if 1


    CALLER AsioExecutionContext executionContext;
    CALLER ThreadExecutionContext tec(executionContext);
//    CALLER CoreSingleThreadExecutor singleExecutor;

//    CALLER MessageFactoryPtr factory = CALLER MessageFactory::make();
//    factory->registerMessageMeta(CALLER MessageMeta::create<OldMessage>());
//    factory->registerMessageMeta(CALLER MessageMeta::create<NewMessage>());

//    MessageMeta *meta = factory->messageMetaById(0x12);

//    CALLER MessagePtr _0x12Msg = factory->messageById(0x12);

//    RefPtr<OldMessage> _T0x12Msg = StaticCastRefPtr<OldMessage>(_0x12Msg);

//    CALLER MessagePtr _0x13Msg = factory->messageById(0x15);

//    std::cout << _0x13Msg->id() << '\n';
//    std::cout << meta->type()->name() << "," << _T0x12Msg->id() << '\n';

//    return 0;

//    CALLER RouterPtr brouter = CALLER Router::create<BroadcastRouter>();
//    CALLER RouterPtr router  = CALLER Router::create<PersistenceRouter>();
//    CALLER RouterPtr trouter = CALLER Router::create<TemporaryRouter>();

//    brouter->add(trouter);
//    brouter->add(router);

//    auto route2 = NewRefPtr<RouteFunc>([](const EventPtr &e) {
//        std::cout << "route2: " << e->id() << '\n';
//    });

//    route2->setID(0);
//    route2->setIDMask(0);
//    route2->setSequenceNumberMask(0);
//    trouter->add(route2);


//    auto route = NewRefPtr<RouteFunc>([](const EventPtr &e) {
//        std::cout << "route1: " << e->id() << '\n';
//    });

//    route->setID(0x10);
//    route->setIDMask(0x10);
//    route->setSequenceNumberMask(0);
//    router->add(route);

//    ::Message* msg = new ::Message();
//    msg->pif = 0x1412;
//    msg->eif = 0x1212;
//    msg->length = sizeof(::Message);
//    msg->mid    = 0x12;

//    brouter->post(CALLER Event::make(msg->mid, 1, msg, CALLER Error(MakeError(CALLER GenericError::timed_out))));
//    brouter->post(CALLER Event::make(msg->mid, 0, msg, CALLER Error()));

//    return 0;

    CALLER RouterPtr brouter = CALLER Router::create<BroadcastRouter>();
    CALLER RouterPtr trouter = CALLER Router::create<TemporaryRouter>();
    CALLER RouterPtr router  = CALLER Router::create<PersistenceRouter>();

    auto r = CreateRouteFuncInvoker([](EventPtr e) {
        std::cout << "proute: " << e->id() << '\n';
    });

    r->setID(0x13);
    r->setSequenceNumberMask(0x00);

    auto r2 = CreateRouteFuncInvoker([](EventPtr e) {
        std::cout << "proute: " << e->id() << '\n';
    });

    r2->setID(0x12);
    r2->setSequenceNumberMask(0x00);

    router->add(r);
    router->add(r2);
    brouter->add(router);
    brouter->add(trouter);

    CALLER AsioUdpSocketHandler* socketHandler = new CALLER AsioUdpSocketHandler(executionContext);
    CALLER PipelineContextPtr pipelineContext = CALLER IOPipelineContext::make(socketHandler);

    auto rpipe = pipelineContext->pipeline();
    rpipe->append(PipelineStage::create<Empty>());

    rpipe->append(PipelineStage::create<MessageRouter>(trouter));
    rpipe->append(PipelineStage::create<MessageDelimiterBaseFrameEncoder>());

    rpipe->append(PipelineStage::create<MessageReadRouter>(brouter));
    rpipe->append(PipelineStage::create<MessageDelimiterBaseFrameDecoder>());

    CALLER Endpoint endpoint;
    endpoint.setHost("127.0.0.1");
    endpoint.setPort("9097");

    pipelineContext->connect(endpoint);

    executionContext.start();
#endif
    return 0;
}
