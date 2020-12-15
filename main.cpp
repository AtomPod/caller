#include <iostream>
#include <caller/call/iocontext.hpp>
#include <caller/call/request.hpp>
#include <caller/call/response.hpp>
#include <caller/call/service.hpp>
#include <caller/common/bytebuffer.hpp>
#include <caller/route/router.hpp>
#include <caller/route/routefunc.hpp>
#include <caller/middleware/router_pipeline_stage.hpp>
#include <caller/middleware/message_pipeline_stage.hpp>
#include <caller/middleware/unpack_pipeline_stage.hpp>
#include <caller/message/message.hpp>
#include <caller/message/messageheader.hpp>
#include <caller/message/messagebody.hpp>
#include <caller/call/contextresponse.hpp>
#include <thread>
#include <caller/call/pipeline.hpp>
#include <caller/call/pipelinestagewrapper.hpp>
#include <caller/message/messagefactory.hpp>
#include <caller/async/future.hpp>
#include <caller/call/asio/asiotcpservice.hpp>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/executor/threadexecutioncontext.hpp>
#include <caller/route/broadcastrouter.hpp>
#include <caller/route/persistencerouter.hpp>
#include <caller/route/temporaryrouter.hpp>

using namespace std;

std::thread th;

class MessageHeader : public CALLER MessageHeader {
public:
#pragma pack(push, 1)
    struct header {
        uint16_t  size;
        uint16_t  serial;
        uint16_t  message_id;
    };
#pragma pack(pop)
public:
    typedef std::shared_ptr<MessageHeader> Ptr;
    static Ptr make() {
        Ptr p = std::make_shared<MessageHeader>();
        p->_M_header = new header();
        return p;
    }
    static Ptr makeEmpty() {
        return std::make_shared<MessageHeader>();
    }
    static CALLER MessageHeaderPtr generator(CALLER ByteBuffer data) {
        Ptr header = make();
        if (header->deserialize(data) == CALLER Error::Nil) {
            return std::move(header);
        }
        return nullptr;
    }
public:
    MessageHeader() {}
public:
    uint16_t size() { return _M_header == nullptr ? 0 : _M_header->size; }
    void setSize(uint16_t s) { _M_header->size = s; }

    uint16_t serial() { return _M_header == nullptr ? 0 : _M_header->serial; }
    void setSerial(uint16_t s) { _M_header->serial = s; }

    uint16_t messageID() { return _M_header == nullptr ? 0 : _M_header->message_id; }
    void setmessageID(uint16_t s) { _M_header->message_id = s; }

public:
    virtual CALLER  ID  id()   const {
        return  CALLER ID(_M_header->message_id);
    }

    virtual size_t      size() const {
        return _M_header->size;
    }

public:
    virtual     bool    isValid() const {
        return _M_header != nullptr;
    }

    virtual     CALLER Error     serialize(CALLER ByteBuffer buffer)const {
       buffer.put(_M_header->size);
       buffer.put(_M_header->serial);
       buffer.put(_M_header->message_id);
       return CALLER Error::Nil;
    }

    virtual     CALLER Error     deserialize(CALLER ByteBuffer buffer) {
        CALLER StringView view;
        if (!buffer.take(view, 0, sizeof(header))) {
            return CALLER Error(-1);
        }
        _M_header = reinterpret_cast<header*>(const_cast<char*>(view.data()));
        _M_header->size = CALLER fromBigEndian(_M_header->size);
        _M_header->serial = CALLER fromBigEndian(_M_header->serial);
        _M_header->message_id = CALLER fromBigEndian(_M_header->message_id);
        return CALLER Error::Nil;
    }
private:
    header *_M_header;
};

template<typename T>
class CMessage : public CALLER Message {
public:
    static std::shared_ptr<CMessage> make() {
        return std::make_shared<CMessage>();
    }
public:
    virtual CALLER ID          id() const {
        if (m_header == nullptr)
            return 0;
        return (CALLER ID(m_header->messageID()) | (CALLER ID(m_header->serial()) << 16));
    }

    virtual void setHeader(CALLER MessageHeaderPtr header) {
        m_header = std::static_pointer_cast<MessageHeader>(header);
    }

    virtual CALLER MessageHeaderPtr header() const {
        return m_header;
    }

    virtual void setBody(CALLER MessageBodyPtr body) {
        UNUSED(body);
    }

    virtual CALLER MessageBodyPtr  body() const {
        return nullptr;
    }
private:
    std::shared_ptr<MessageHeader> m_header;
};

template <typename T>
CALLER MessagePtr makeCMessageGenerator() {
    return CMessage<T>::make();
}

class UnpackPipelineStage : public CALLER UnpackPipelineStage
{
public:
    static CALLER UnpackPipelineStagePtr make() {
        return std::make_shared<UnpackPipelineStage>();
    }
public:
    UnpackPipelineStage() {}
public:
    virtual size_t  packageSize(const CALLER RingBuffer &buffer) {
        uint16_t size = 0;
        if (buffer.copyTo(reinterpret_cast<CALLER RingBuffer::Element*>(&size), 2, 0) != 2) {
            return 0;
        }
        size = CALLER fromBigEndian(size);
        std::cout << "package size: " << size << '\n';
        return size;
    }
};

int main()
{
    CALLER AsioExecutionContext executionContext;
    CALLER ThreadExecutionContext tec(executionContext);

#include <caller/route/persistencerouter.hpp>
#include <caller/route/temporaryrouter.hpp>

    CALLER RouterPtr broadcast   = CALLER BroadcastRouter::make();
    CALLER RouterPtr persistence = CALLER PersistenceRouter::make();
    CALLER RouterPtr temporary   = CALLER TemporaryRouter::make();

    broadcast->add(persistence);
    broadcast->add(temporary);

    CALLER UnpackPipelineStagePtr unpackStage = UnpackPipelineStage::make();
    CALLER RouterPipelineStagePtr routerStage = CALLER RouterPipelineStage::make(broadcast, temporary);
    CALLER RouterPtr              router = routerStage->router();
    CALLER MessageFactoryPtr      tcpMessageFactory = CALLER MessageFactory::make();

    CALLER AsioTcpConnection::Endpoint endpoint;
    endpoint.setHost("cuav-l03.cuav.net");
    endpoint.setPort("4341");

    CALLER AsioTcpService::Config config;
    config.setRouter(router);
    config.setMessageFactory(tcpMessageFactory);
    config.setEndpoint(endpoint);

    CALLER AsioTcpServicePtr tcpService = CALLER AsioTcpService::make(executionContext, &config);
    tcpService->addWriteHandler(tcpService->transportStage());
    tcpService->addWriteHandler(routerStage->requestStage());
    tcpService->addReadHandler(unpackStage);
    tcpService->addReadHandler(CALLER MessagePipelineStage::messageHeaderParser(MessageHeader::generator));
    tcpService->addReadHandler(CALLER MessagePipelineStage::messageFromHeader(tcpService->messageFactory()));
    tcpService->addReadHandler(routerStage->responseStage());



//    CALLER ExecutionContext exeCtx(CALLER AsioExecutor::make(tcpService));

//    auto [context, cancel] = CALLER Context::withCancel(CALLER Context::backgroundContext());

//    CALLER FutureInterface<int> promise;
//    auto result = promise.future();
//    result.whenFinished([](int a) {
//        std::cout << "a: " << a << ", running thread: " << std::this_thread::get_id() << '\n';
//    }).whenCanceled([](int errcode, const std::exception_ptr &e) {

//    });

//    auto ctx = context;
//    std::thread t2([promise, ctx]() mutable {
//        CALLER AsioExecutionContext executionContext;
//        CALLER ThreadExecutionContext tec(executionContext);

//        auto result = promise.future();
//        result.whenFinished([](int a) {
//            std::cout << "t2 a: " << a << ", running thread: " << std::this_thread::get_id() << '\n';
//        });

//        CALLER Context::ChannelCancelFunc cancel;
//        ctx->Done().whenFinished([](CALLER Context::Error err) {
//            std::cout << "Error: " << err << ", running thread: " << std::this_thread::get_id() << '\n';
//        }, &cancel);

//        executionContext.start();
//        cancel();
//    });
//    cancel();

//    std::thread t1([promise, ctx]() mutable {
//        std::this_thread::sleep_for(std::chrono::seconds(2));
//        std::cout << "this_thread: " << std::this_thread::get_id() << '\n';
//        promise.reportResult(15);

//        CALLER Future<CALLER Context::Error>::CancelFunc cancel;
//        ctx->Done().whenFinished([](CALLER Context::Error err) {
//            std::cout << "Error: " << err << ", running thread: " << std::this_thread::get_id() << '\n';
//        }, &cancel);
//        cancel();
//    });

    executionContext.start();
//    t1.join();
//    t2.join();
    return 0;

//    CALLER FutureInterface<CALLER ResponsePtr> futureI;
//    CALLER Future<CALLER ResponsePtr> future = futureI.future();
//    future.whenFinished([](CALLER ResponsePtr resp) {
//        std::cout << resp->rawMessage() << '\n';
//    });

//    CALLER MessageFactoryPtr factory = CALLER MessageFactory::make();
//    factory->addGenerator(0x0001, makeCMessageGenerator<int>);

//    CALLER PipelinePtr pipeline = CALLER Pipeline::make(nullptr);
//    CALLER RouterPipelineStagePtr routerStage = CALLER RouterPipelineStage::make(0x0000FFFF);
//    CALLER RouterPtr router = routerStage->router();

//    pipeline->append(CALLER PipelineStageWrapper::make(routerStage->requestStage()));
//    pipeline->append(CALLER PipelineStageWrapper::make(
//                        [](CALLER IOContextPtr ctx,
//                         CALLER ByteBuffer data,
//                         CALLER ServicePtr service,
//                         CALLER PipelineStagePtr next) {

//                         ctx->setMetadata("atom", 12);
//                         ctx->addFlag(CALLER IOContext::SentFlag);
//                         std::cout << "sent\n";

//                         th = std::thread([=]() {
//                             std::this_thread::sleep_for(std::chrono::seconds(3));
//                             auto message = MessageHeader::make();
//                             message->setSize(6);
//                             message->setSerial(21);
//                             message->setmessageID(0x0001);

//                             CALLER ByteBuffer data;
//                             message->serialize(data);
//                             next->propagate(ctx, data, service);
//                         });
//                     }));

//    pipeline->append(UnpackPipelineStage::make());
//    pipeline->append(CALLER PipelineStageWrapper::make(
//                         CALLER MessagePipelineStage::messageHeaderParser(MessageHeader::generator)
//                    ));
//    pipeline->append(CALLER PipelineStageWrapper::make(
//                        CALLER MessagePipelineStage::messageFromHeader(factory)
//                    ));
//    pipeline->append(CALLER PipelineStageWrapper::make(routerStage->responseStage()));

//    CALLER RoutePtr route = CALLER routeFromFunc(0x00010001, [futureI] (
//                            CALLER IOContextPtr ptr, CALLER ResponsePtr resp) mutable {
//        auto message = resp->rawMessage();
//        auto header  = message->header();
//        auto rheader = std::static_pointer_cast<MessageHeader>(header);

//        std::cout << "test ptr: " << ptr->metadata<const char*>("hello") << ","
//                  << ptr->metadata<int>("atomic") << ',' << rheader->serial() << '\n';
//        futureI.reportResult(resp);
//    });
//    route->setMask(0x0000FFFF);

//    auto context = CALLER IOContext::make();
//    CALLER RoutePtr route2 = CALLER routeFromFunc(0x00000001, [](
//                                                 CALLER IOContextPtr ptr, CALLER ResponsePtr resp) {
//                             auto message = resp->rawMessage();
//                             auto header  = message->header();
//                             auto rheader = std::static_pointer_cast<MessageHeader>(header);

//                             std::cout << "test callback ptr: " << ptr->metadata<const char*>("hello") << '\n';
//                         });
//    route2->setMask(0x0000FFFF);
//    route2->setContext(context);
//    router->add(route2);

//    CALLER RouterPipelineStage::setRouteToContext(context, route);
//    context->setMetadata("hello", "atomic");

//    pipeline->propagate(context, CALLER ByteBuffer(), nullptr);

//    for (;;){}
//    th.join();
//    return 0;
}
