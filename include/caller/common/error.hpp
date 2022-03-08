#ifndef CALLER_ERROR_HPP
#define CALLER_ERROR_HPP
#include <caller/core.hpp>
#include <system_error>

CALLER_BEGIN

typedef ::std::error_code Error;
typedef ::std::errc		  GenericError;
typedef ::std::io_errc	  IOError;

inline Error MakeError(const GenericError &e) {
    return std::make_error_code(e);
}

inline Error MakeError(const IOError &e) {
    return std::make_error_code(e);
}

CALLER_END

#endif // CALLER_ERROR_HPP
