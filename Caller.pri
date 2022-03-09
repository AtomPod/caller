$$PWD/includePATH += $$PWD/include

$$PWD/include(third_party/asio/AsioLibrary.pri)
$$PWD/include(third_party/sigslot/SigSlotLibrary.pri)

SOURCES += \
        $$PWD/src/caller/call/iohandler.cpp \
        $$PWD/src/caller/call/iopipelinecontext.cpp \
        $$PWD/src/caller/call/pipelinewritereadstage.cpp \
        $$PWD/src/caller/async/futureeventlistener.cpp \
        $$PWD/src/caller/call/socketeventrequest.cpp \
        $$PWD/src/caller/message/typedmessage.cpp \
        $$PWD/src/caller/route/event.cpp \
        $$PWD/src/caller/call/asio/tcpsockethandler.cpp \
        $$PWD/src/caller/call/asio/udpsockethandler.cpp \
        $$PWD/src/caller/call/sockethandler.cpp \
        $$PWD/src/caller/call/pipelinereadstage.cpp \
        $$PWD/src/caller/call/pipelinewritestage.cpp \
        $$PWD/src/caller/call/endpoint.cpp \
        $$PWD/src/caller/call/pipelinecontext.cpp \
        $$PWD/src/caller/context/valuecontext.cpp \
        $$PWD/src/caller/context/cancelablecontext.cpp \
        $$PWD/src/caller/route/persistencerouter.cpp \
        $$PWD/src/caller/route/temporaryrouter.cpp \
        $$PWD/src/caller/route/broadcastrouter.cpp \
        $$PWD/src/caller/async/futureinterfacebaseimpl.cpp \
        $$PWD/src/caller/async/futureinterface.cpp\
        $$PWD/src/caller/common/error.cpp \
        $$PWD/src/caller/executor/asio/asioexecutor.cpp \
        $$PWD/src/caller/executor/threadexecutioncontext.cpp \
        $$PWD/src/caller/message/messagefactory.cpp \
        $$PWD/src/caller/call/pipeline.cpp \
        $$PWD/src/caller/call/pipelinestage.cpp \
        $$PWD/src/caller/message/message.cpp \
        $$PWD/src/caller/common/serializable.cpp \
        $$PWD/src/caller/common/ringbuffer.cpp \
        $$PWD/src/caller/common/stringview.cpp \
        $$PWD/src/caller/route/routefunc.cpp \
        $$PWD/src/caller/route/simplerouter.cpp \
        $$PWD/src/caller/route/declare.cpp \
        $$PWD/src/caller/route/router.cpp \
        $$PWD/src/caller/common/bytebuffer.cpp \
        $$PWD/src/caller/common/endian.cpp \
        $$PWD/src/caller/context/context.cpp \
        $$PWD/src/caller/route/route.cpp \
        $$PWD/src/caller/executor/executor.cpp

HEADERS += \
    $$PWD/include/caller/async/declare.hpp \
    $$PWD/include/caller/async/futureeventlistener.hpp \
    $$PWD/include/caller/async/futureinterfacebaseimpl.hpp \
    $$PWD/include/caller/async/futureinterface.hpp\
    $$PWD/include/caller/async/future.hpp \
    $$PWD/include/caller/call/asio/tcpsockethandler.hpp \
    $$PWD/include/caller/call/iohandler.hpp \
    $$PWD/include/caller/call/iopipelinecontext.hpp \
    $$PWD/include/caller/call/pipelinestage/delimiterbasedframedecoder.hpp \
    $$PWD/include/caller/call/endpoint.hpp \
    $$PWD/include/caller/call/pipeline.hpp \
    $$PWD/include/caller/call/pipelinecontext.hpp \
    $$PWD/include/caller/call/pipelinereadstage.hpp \
    $$PWD/include/caller/call/pipelinestage.hpp \
    $$PWD/include/caller/call/pipelinestage/delimiterbasedframeencoder.hpp \
    $$PWD/include/caller/call/pipelinewritereadstage.hpp \
    $$PWD/include/caller/call/pipelinewritestage.hpp \
    $$PWD/include/caller/call/socketeventrequest.hpp \
    $$PWD/include/caller/call/sockethandler.hpp \
    $$PWD/include/caller/common/error.hpp \
    $$PWD/include/caller/common/refptr.hpp \
    $$PWD/include/caller/common/signal.hpp \
    $$PWD/include/caller/context/cancelablecontext.hpp \
    $$PWD/include/caller/context/error.hpp \
    $$PWD/include/caller/context/valuecontext.hpp \
    $$PWD/include/caller/executor/asio/asioexecutor.hpp \
    $$PWD/include/caller/message/declare.hpp \
    $$PWD/include/caller/message/message.hpp \
    $$PWD/include/caller/common/any.hpp \
    $$PWD/include/caller/common/bytebuffer.hpp \
    $$PWD/include/caller/common/endian.hpp \
    $$PWD/include/caller/common/id.hpp \
    $$PWD/include/caller/common/ringbuffer.hpp \
    $$PWD/include/caller/common/serializable.hpp \
    $$PWD/include/caller/context/context.hpp \
    $$PWD/include/caller/call/declare.hpp \
    $$PWD/include/caller/context/declare.hpp \
    $$PWD/include/caller/core.hpp \
    $$PWD/include/caller/message/messagefactory.hpp \
    $$PWD/include/caller/message/typedmessage.hpp \
    $$PWD/include/caller/route/broadcastrouter.hpp \
    $$PWD/include/caller/route/declare.hpp \
    $$PWD/include/caller/route/event.hpp \
    $$PWD/include/caller/route/route.hpp \
    $$PWD/include/caller/route/routefunc.hpp \
    $$PWD/include/caller/route/router.hpp \
    $$PWD/include/caller/route/simplerouter.hpp \
    $$PWD/include/caller/common/stringview.hpp \
    $$PWD/include/caller/executor/executor.hpp \
    $$PWD/include/caller/executor/threadexecutioncontext.hpp \
    $$PWD/include/caller/route/temporaryrouter.hpp \
    $$PWD/include/caller/route/persistencerouter.hpp \
    $$PWD/include/caller/call/asio/udpsockethandler.hpp \
    $$PWD/include/caller/executor/singlethreadexecution.hpp

