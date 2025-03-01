#pragma once

#ifdef _WINDOWS
#define GETPID _getpid
#else
#include <unistd.h>
#define GETPID getpid
#endif

#include <string>

namespace infra {

[[nodiscard]] uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing = false,
                                         bool lazy = true);

bool release_mmap_buffer(uintptr_t address, [[maybe_unused]] size_t size, bool lazy);

}  // namespace infra