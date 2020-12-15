TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

INCLUDEPATH += include
INCLUDEPATH += D:\library\asio\asio\include

include(third_party/asio/AsioLibrary.pri)
include(third_party/sigslot/SigSlotLibrary.pri)

SOURCES += \
        src/caller/context/valuecontext.cpp \
        src/caller/context/cancelablecontext.cpp \
        src/caller/route/persistencerouter.cpp \
        src/caller/route/temporaryrouter.cpp \
        src/caller/route/broadcastrouter.cpp \
        src/caller/call/transport.cpp \
        src/caller/call/asio/asiotcpconnection.cpp \
        src/caller/call/asio/asiotcpservice.cpp \
        src/caller/call/underlyingservice.cpp \
        src/caller/call/connection.cpp \
        src/caller/call/asio/asioservice.cpp \
        src/caller/async/futureinterfacebaseimpl.cpp \
        src/caller/async/futureinterface.cpp\
        src/caller/call/contextresponse.cpp \
        src/caller/call/iocontext.cpp \
        src/caller/call/request.cpp \
        src/caller/call/response.cpp \
        src/caller/call/service.cpp \
        src/caller/common/error.cpp \
        src/caller/call/pipelinestagewrapper.cpp \
        src/caller/executor/asio/asioexecutor.cpp \
        src/caller/executor/threadexecutioncontext.cpp \
        src/caller/message/messagefactory.cpp \
        src/caller/call/pipeline.cpp \
        src/caller/call/pipelinestage.cpp \
        src/caller/message/messagebody.cpp \
        src/caller/message/messageheader.cpp \
        src/caller/message/message.cpp \
        src/caller/common/serializable.cpp \
        src/caller/common/ringbuffer.cpp \
        src/caller/middleware/crc_pipeline_stage.cpp \
        src/caller/middleware/message_pipeline_stage.cpp \
        src/caller/common/stringview.cpp \
        src/caller/middleware/router_pipeline_stage.cpp \
        src/caller/middleware/unpack_pipeline_stage.cpp \
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
    include/caller/async/futureinterfacebaseimpl.hpp \
    include/caller/async/futureinterface.hpp\
    include/caller/async/future.hpp \
    include/caller/call/asio/asioservice.hpp \
    include/caller/call/asio/asiotcpconnection.hpp \
    include/caller/call/asio/asiotcpservice.hpp \
    include/caller/call/connection.hpp \
    include/caller/call/contextresponse.hpp \
    include/caller/call/iocontext.hpp \
    include/caller/call/pipeline.hpp \
    include/caller/call/pipelinestage.hpp \
    include/caller/call/pipelinestagewrapper.hpp \
    include/caller/call/request.hpp \
    include/caller/call/response.hpp \
    include/caller/call/service.hpp \
    include/caller/call/transport.hpp \
    include/caller/call/underlyingservice.hpp \
    include/caller/common/error.hpp \
    include/caller/common/observerlist.hpp \
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
    include/caller/message/messagebody.hpp \
    include/caller/message/messagefactory.hpp \
    include/caller/message/messageheader.hpp \
    include/caller/middleware/crc_pipeline_stage.hpp \
    include/caller/middleware/declare.hpp \
    include/caller/middleware/message_pipeline_stage.hpp \
    include/caller/middleware/router_pipeline_stage.hpp \
    include/caller/middleware/unpack_pipeline_stage.hpp \
    include/caller/route/broadcastrouter.hpp \
    include/caller/route/declare.hpp \
    include/caller/route/route.hpp \
    include/caller/route/routefunc.hpp \
    include/caller/route/router.hpp \
    include/caller/route/simplerouter.hpp \
    include/caller/common/stringview.hpp \
    include/caller/executor/executor.hpp \
    include/caller/executor/threadexecutioncontext.hpp \
    include/caller/route/temporaryrouter.hpp \
    include/caller/route/persistencerouter.hpp

