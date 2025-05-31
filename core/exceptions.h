#pragma once

#include <stdexcept>

namespace btra {

class JournalError : public std::runtime_error {
public:
    JournalError(const std::string &message) : runtime_error(message) {}
};

} // namespace btra