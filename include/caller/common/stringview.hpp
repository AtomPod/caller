#ifndef STRINGVIEW_HPP
#define STRINGVIEW_HPP
#include <caller/core.hpp>
#include <cstdint>
#include <iostream>

CALLER_BEGIN

class CALLER_DLL_EXPORT StringView
{
public:
    static constexpr size_t npos = SIZE_MAX;
public:
    enum class Code {
      NONE,
      ASCII,
      UTF8
    };
public:
    StringView();
    StringView(const char *data, size_t size, size_t capacity = 0, const Code &code = Code::ASCII);
    ~StringView();
public:
    Code        code() const;
    void        setCode(const Code &code);
public:
    size_t       size() const { return _M_Size; }
    size_t       capacity() const { return _M_Capacity; }
    const  char* data() const { return _M_Data; }
    void         swap(StringView &other);
    bool         empty() const { return _M_Size == 0; }
    bool         startsWith(const StringView &other) const CALLER_NOEXCEPT;
    bool         endsWith(const StringView &other) const CALLER_NOEXCEPT;
    bool         contains(const StringView &other) const CALLER_NOEXCEPT;
    bool         contains(const char *str) const CALLER_NOEXCEPT;
    size_t       find(const StringView &other, size_t from = 0) const CALLER_NOEXCEPT;
    size_t       find(const char* str, size_t from = 0) const CALLER_NOEXCEPT;

    template<int N>
    size_t       contains(const char (&str)[N]) const CALLER_NOEXCEPT {
        return  contains(StringView(str, N));
    }

    template<int N>
    size_t      find(const char (&str)[N], size_t from = 0) const CALLER_NOEXCEPT {
        return  find(StringView(str, N), from);
    }
public:
    StringView   operator()(uint32_t from, uint32_t to) const;
    const char&  operator[](size_t  pos) const;
    bool         operator==(const StringView &other) const;
    bool         operator!=(const StringView &other) const { return !(*this == other); }
    bool         operator<(const StringView &other)  const;
    bool         operator>(const StringView &other)  const { return !(*this < other || *this == other); }
    bool         operator>=(const StringView &other) const { return *this == other || *this > other; }
    bool         operator<=(const StringView &other) const { return *this == other || *this < other; }
private:
    const char *_M_Data;
    size_t      _M_Size;
    size_t      _M_Capacity;
    Code        _M_Code;
};

std::ostream &operator<<(std::ostream &os, const StringView &view);

CALLER_END

#endif // STRINGVIEW_HPP
