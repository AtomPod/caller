INCLUDEPATH += $$PWD/include

SOURCES += \
        $$PWD/src/common/serializable.cpp \
        $$PWD/src/call/message.cpp \
        $$PWD/src/common/ringbuffer.cpp \
        $$PWD/src/middleware/router_interceptor.cpp \
        $$PWD/src/call/call_service.cpp \
        $$PWD/src/common/stringview.cpp \
        $$PWD/src/middleware/unpack_interceptor.cpp \
        $$PWD/src/route/routefunc.cpp \
        $$PWD/src/route/simplerouter.cpp \
        $$PWD/src/route/declare.cpp \
        $$PWD/src/route/router.cpp \
        $$PWD/src/call/call_response.cpp \
        $$PWD/src/common/bytebuffer.cpp \
        $$PWD/src/common/endian.cpp \
        $$PWD/src/call/call_request.cpp \
        $$PWD/src/call/call_context.cpp \
        $$PWD/src/call/interceptor.cpp \
        $$PWD/src/context/context.cpp \
        $$PWD/src/route/route.cpp

HEADERS += \
    $$PWD/include/call/call_request.hpp \
    $$PWD/include/call/call_response.hpp \
    $$PWD/include/call/message.hpp \
    $$PWD/include/common/any.hpp \
    $$PWD/include/call/call_context.hpp \
    $$PWD/include/common/bytebuffer.hpp \
    $$PWD/include/common/endian.hpp \
    $$PWD/include/common/id.hpp \
    $$PWD/include/common/ringbuffer.hpp \
    $$PWD/include/common/serializable.hpp \
    $$PWD/include/context/context.hpp \
    $$PWD/include/call/declare.hpp \
    $$PWD/include/call/interceptor.hpp \
    $$PWD/include/context/declare.hpp \
    $$PWD/include/core.hpp \
    $$PWD/include/middleware/declare.hpp \
    $$PWD/include/middleware/router_interceptor.hpp \
    $$PWD/include/route/declare.hpp \
    $$PWD/include/route/route.hpp \
    $$PWD/include/route/routefunc.hpp \
    $$PWD/include/route/router.hpp \
    $$PWD/include/route/simplerouter.hpp \
    $$PWD/include/common/stringview.hpp \
    $$PWD/include/call/call_service.hpp \
    $$PWD/include/middleware/unpack_interceptor.hpp
