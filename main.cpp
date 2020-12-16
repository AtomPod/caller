#include <iostream>
#include <caller/executor/asio/asioexecutor.hpp>
#include <caller/executor/threadexecutioncontext.hpp>
#include <caller/call/pipelinewritestage.hpp>
#include <caller/call/pipelinereadstage.hpp>
#include <caller/call/pipeline.hpp>
#include <string>

using namespace std;

USING_NAMESPACE_CALLER

struct Data {
    int age;
};

class TestReadPipeline : public CALLER PipelineTypedReadStage<std::string>
{
public:
    TestReadPipeline() {}
protected:
    virtual void handleTypedRead(CALLER PipelineContextPtr context,
                                 CALLER ByteBuffer buffer,
                                 const std::string &object) override {
        std::cout << "object: " << object << std::endl;
        auto next = nextStage();
        if (next != nullptr) {
            auto data = new Data();
            data->age = object.size();
            next->handleRead(context, buffer, data);
        }
    }

    virtual void causeException(CALLER PipelineContextPtr, const std::exception &e) override {
        std::cout << "exception: " << e.what() << '\n';
    }

};

class TestRead2Pipeline : public CALLER PipelineTypedReadStage<Data*>
{
protected:
    virtual void pipelineActive(PipelineContextPtr context)         {

    }

    virtual void handleTypedRead(CALLER PipelineContextPtr context,
                                 CALLER ByteBuffer buffer,
                                  Data *object) override {
        std::cout << "delta: " << object->age << std::endl;
        delete object;
    }

    virtual void causeException(CALLER PipelineContextPtr, const std::exception &e) override {
        std::cout << "exception2: " << e.what() << '\n';
    }

};


int main()
{
    CALLER AsioExecutionContext executionContext;
    CALLER ThreadExecutionContext tec(executionContext);

    CALLER PipelinePtr pipeline = CALLER Pipeline::make(nullptr);
    pipeline->append(PipelineStage::create<TestReadPipeline>());
    pipeline->append(PipelineStage::create<TestRead2Pipeline>());

    std::string test = "atomic";
    pipeline->handleRead(nullptr, CALLER ByteBuffer(), test);

    auto canceld = CALLER ThreadExecutionContext::executionContext()->executor()->execute([]() {
        std::cout << "timeout\n";
    }, std::chrono::seconds(3));

    std::cout << "start context\n";
    executionContext.start();
    return 0;
}
