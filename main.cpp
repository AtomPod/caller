#include <iostream>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/executor/threadexecutioncontext.hpp>
#include <caller/call/pipelinewritestage.hpp>
#include <caller/call/pipelinereadstage.hpp>
#include <caller/call/pipeline.hpp>
#include <caller/call/asio/tcpsockethandler.hpp>
#include <string>

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
        Future<size_t> result = context->write(any(), buffer);
        result.whenFinished([](size_t size) {
            std::cout << "size: " << size << '\n';
        });
    }

    virtual void pipelineInactive(PipelineContextPtr context) {
        std::cout << "disconnected\n";
    }

    virtual void pipelineReadComplete(PipelineContextPtr ctx)   {
        std::cout << "read complete\n";
    }

    virtual void pipelineWriteComplete(PipelineContextPtr)   {
        std::cout << "write complete\n";
    }

    virtual void handleRead(CALLER PipelineContextPtr context,
                                 CALLER ByteBuffer buffer,
                                 const any &object) override {
        std::cout << StringView(buffer.data(), buffer.length());
        auto next = nextStage();
        if (next != nullptr) {
            next->handleRead(context, buffer, object);
        }
    }

    virtual void causeException(CALLER PipelineContextPtr, const std::exception &e) override {
        std::cout << "exception: " << e.what() << '\n';
    }

};

int main()
{
    CALLER AsioExecutionContext executionContext;
    CALLER ThreadExecutionContext tec(executionContext);

    CALLER AsioTcpSocketHandler* socketHandler = new CALLER AsioTcpSocketHandler(executionContext);
    CALLER PipelineContextPtr pipelineContext = CALLER AsioTcpPipelineContext::make(socketHandler);

    auto rpipe = pipelineContext->readPipeline();
    rpipe->append(PipelineStage::create<TestReadPipeline>());

    CALLER Endpoint endpoint;
    endpoint.setHost("www.baid.com");
    endpoint.setPort("80");

    pipelineContext->connect(endpoint);
    executionContext.start();
    return 0;
}
