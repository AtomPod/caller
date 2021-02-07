TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += include
INCLUDEPATH += D:\library\asio\asio\include

include(third_party/asio/AsioLibrary.pri)
include(third_party/sigslot/SigSlotLibrary.pri)

SOURCES += \
        src/caller/call/iohandler.cpp \
        src/caller/call/iopipelinecontext.cpp \
        src/caller/call/pipelinewritereadstage.cpp \
        src/caller/async/futureeventlistener.cpp \
        src/caller/call/socketeventrequest.cpp \
        src/caller/message/typedmessage.cpp \
        src/caller/route/event.cpp \
        src/caller/call/asio/tcpsockethandler.cpp \
        src/caller/call/asio/udpsockethandler.cpp \
        src/caller/call/sockethandler.cpp \
        src/caller/call/pipelinereadstage.cpp \
        src/caller/call/pipelinewritestage.cpp \
        src/caller/call/endpoint.cpp \
        src/caller/call/pipelinecontext.cpp \
        src/caller/context/valuecontext.cpp \
        src/caller/context/cancelablecontext.cpp \
        src/caller/route/persistencerouter.cpp \
        src/caller/route/temporaryrouter.cpp \
        src/caller/route/broadcastrouter.cpp \
        src/caller/async/futureinterfacebaseimpl.cpp \
        src/caller/async/futureinterface.cpp\
        src/caller/common/error.cpp \
        src/caller/executor/asio/asioexecutor.cpp \
        src/caller/executor/threadexecutioncontext.cpp \
        src/caller/message/messagefactory.cpp \
        src/caller/call/pipeline.cpp \
        src/caller/call/pipelinestage.cpp \
        src/caller/message/message.cpp \
        src/caller/common/serializable.cpp \
        src/caller/common/ringbuffer.cpp \
        src/caller/common/stringview.cpp \
        src/caller/route/routefunc.cpp \
        src/caller/route/simplerouter.cpp \
        src/caller/route/declare.cpp \
        src/caller/route/router.cpp \
        src/caller/common/bytebuffer.cpp \
        src/caller/common/endian.cpp \
        main.cpp \
        src/caller/context/context.cpp \
        src/caller/route/route.cpp \
        src/caller/executor/executor.cpp

HEADERS += \
    include/caller/async/declare.hpp \
    include/caller/async/futureeventlistener.hpp \
    include/caller/async/futureinterfacebaseimpl.hpp \
    include/caller/async/futureinterface.hpp\
    include/caller/async/future.hpp \
    include/caller/call/asio/tcpsockethandler.hpp \
    include/caller/call/iohandler.hpp \
    include/caller/call/iopipelinecontext.hpp \
    include/caller/call/pipelinestage/delimiterbasedframedecoder.hpp \
    include/caller/call/endpoint.hpp \
    include/caller/call/pipeline.hpp \
    include/caller/call/pipelinecontext.hpp \
    include/caller/call/pipelinereadstage.hpp \
    include/caller/call/pipelinestage.hpp \
    include/caller/call/pipelinestage/delimiterbasedframeencoder.hpp \
    include/caller/call/pipelinewritereadstage.hpp \
    include/caller/call/pipelinewritestage.hpp \
    include/caller/call/socketeventrequest.hpp \
    include/caller/call/sockethandler.hpp \
    include/caller/common/error.hpp \
    include/caller/common/refptr.hpp \
    include/caller/common/signal.hpp \
    include/caller/context/cancelablecontext.hpp \
    include/caller/context/error.hpp \
    include/caller/context/valuecontext.hpp \
    include/caller/executor/asio/asioexecutor.hpp \
    include/caller/message/declare.hpp \
    include/caller/message/message.hpp \
    include/caller/common/any.hpp \
    include/caller/common/bytebuffer.hpp \
    include/caller/common/endian.hpp \
    include/caller/common/id.hpp \
    include/caller/common/ringbuffer.hpp \
    include/caller/common/serializable.hpp \
    include/caller/context/context.hpp \
    include/caller/call/declare.hpp \
    include/caller/context/declare.hpp \
    include/caller/core.hpp \
    include/caller/message/messagefactory.hpp \
    include/caller/message/typedmessage.hpp \
    include/caller/route/broadcastrouter.hpp \
    include/caller/route/declare.hpp \
    include/caller/route/event.hpp \
    include/caller/route/route.hpp \
    include/caller/route/routefunc.hpp \
    include/caller/route/router.hpp \
    include/caller/route/simplerouter.hpp \
    include/caller/common/stringview.hpp \
    include/caller/executor/executor.hpp \
    include/caller/executor/threadexecutioncontext.hpp \
    include/caller/route/temporaryrouter.hpp \
    include/caller/route/persistencerouter.hpp \
    include/caller/call/asio/udpsockethandler.hpp \
    include/caller/executor/singlethreadexecution.hpp

