#include <caller/common/stringview.hpp>
#include <memory.h>
#include <algorithm>
#include <cstring>

CALLER_BEGIN

static void SundayStringTable(int (&table)[256], const char *str, size_t size) {
    for (int i = 0; i < 256; i++) {
        table[i] = 0;
    }

    for (size_t i = 0; i < size; i++) {
        table[uint8_t(str[i])] = int(size - i);
    }
}

static int SundayString(const char *str, size_t size,
                        const char *substr , size_t subsize) {
    int table[256] = {0};

    SundayStringTable(table, substr, subsize);
    for (size_t pos = 0; pos < size;) {
        if (pos + subsize > size) {
            return -1;
        }

        size_t p = pos;
        size_t s = 0;
        for (; s < subsize; ++p, ++s) {
            if (str[p] != substr[s]) {
                break;
            }
        }

        if (s == subsize) {
            return int(pos);
        }

        size_t nextMatch = pos + subsize;
        if (nextMatch >= size) {
            return -1;
        }

        int off = table[ uint8_t(str[nextMatch]) ];
        pos += uint32_t(off);
    }
    return -1;
}

StringView::StringView() : _M_Data(nullptr), _M_Size(0), _M_Capacity(0), _M_Code(Code::NONE)
{

}

StringView::StringView(const char *data, size_t size, size_t capacity, const Code &code)
    : _M_Data(data), _M_Size(size), _M_Capacity(capacity == 0 ? size : capacity), _M_Code(code)
{

}

StringView::~StringView()
{

}

void StringView::swap(StringView &other)
{
    std::swap(_M_Data, other._M_Data);
    std::swap(_M_Size, other._M_Size);
}

bool StringView::startsWith(const StringView &other) const CALLER_NOEXCEPT
{
    size_t otherSize = other.size();
    if (otherSize > size()) {
        return false;
    }

    return std::strncmp(_M_Data, other._M_Data, other.size()) == 0;
}

bool StringView::endsWith(const StringView &other) const CALLER_NOEXCEPT
{
    size_t otherSize = other.size();
    if (otherSize > size()) {
        return false;
    }

    const char *lastN = _M_Data + size() - other.size();
    return std::strncmp(lastN, other.data(), other.size());
}

bool StringView::contains(const StringView &other) const CALLER_NOEXCEPT
{
    return SundayString(data(), size(), other.data(), other.size()) != -1;
}

bool StringView::contains(const char *str) const CALLER_NOEXCEPT
{
    return SundayString(data(), size(), str, std::strlen(str)) != -1;
}

size_t StringView::find(const StringView &other, size_t from) const CALLER_NOEXCEPT
{
    size_t end = from + other.size();
    if (size() < end) {
        return npos;
    }

    int pos = SundayString(data() + from, size() - from, other.data(), other.size());
    if (pos == -1) {
        return npos;
    }
    return size_t(pos);
}

size_t StringView::find(const char *str, size_t from) const CALLER_NOEXCEPT
{
    return find(StringView(str, std::strlen(str)), from);
}

StringView StringView::operator()(uint32_t from, uint32_t to) const {
    if (from >= to) {
        return StringView();
    }

    if (_M_Size <= from || (to != UINT32_MAX && _M_Size <= to)) {
        return StringView();
    }

    return StringView(_M_Data + from, to - from);
}

const char &StringView::operator[](size_t pos) const
{
    return _M_Data[pos];
}

bool StringView::operator==(const StringView &other) const
{
    if (_M_Size != other._M_Size || _M_Data == nullptr || other._M_Data == nullptr) {
        return false;
    }

    return memcmp(_M_Data, other._M_Data, _M_Size) == 0;
}

bool StringView::operator<(const StringView &other) const
{
    if (_M_Data == nullptr) {
        return true;
    } else if (other._M_Data) {
        return false;
    }

    size_t cmpn = (_M_Size > other._M_Size) ? other._M_Size : _M_Size;
    int r = memcmp(_M_Data, other._M_Data, cmpn);
    if (r == 0) {
        return (cmpn == other._M_Size && _M_Size != other._M_Size);
    }
    return r < 0;
}

StringView::Code StringView::code() const
{
    return _M_Code;
}

void StringView::setCode(const Code &code)
{
    _M_Code = code;
}

std::ostream &operator<<(std::ostream &os, const StringView &view) {
    return os.write(view.data(), view.size());
}

CALLER_END
