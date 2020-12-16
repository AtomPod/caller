#include <caller/common/endian.hpp>

CALLER_BEGIN

Endian nativeEndian()
{
    if (ENDIAN() == LITTLE_ENDIAN) {
        return Endian::Little;
    }
    return Endian::Big;
}

CALLER_END
