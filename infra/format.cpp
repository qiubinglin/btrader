#include "format.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <stdexcept>

namespace infra {

std::string cformat(const char *format, ...) {
    if (strlen(format) >= 256) {
        throw std::runtime_error("Over cformat length");
    }
    char buf[256];
    va_list args;
    va_start(args, format);
    snprintf(buf, 128, format, args);
    va_end(args);
    return buf;
}

} // namespace infra