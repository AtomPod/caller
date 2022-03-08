#ifndef CALLER_CONTEXT_ERROR_HPP
#define CALLER_CONTEXT_ERROR_HPP
#include <caller/core.hpp>
#include <system_error>

CALLER_BEGIN

enum ContextError {
    ContextCancel = 1
};

class ContextErrorCategory : public std::error_category {
public:
    ContextErrorCategory() = default;
    virtual ~ContextErrorCategory() = default;

    virtual const char* name() const noexcept {
        return "caller.context";
    }

    virtual std::string message(int e) const {
        if (e == ContextCancel) {
            return "context canceled";
        }
        return "";
    }
};

inline ContextErrorCategory& getContextErrorCategory() {
    static ContextErrorCategory category;
    return category;
}

inline std::error_code makeContextError(ContextError e) {
    return std::error_code(int(e), getContextErrorCategory());
}

CALLER_END


#endif // CALLER_CONTEXT_ERROR_HPP
