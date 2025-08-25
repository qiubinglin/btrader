#pragma once

/**
 * @file mmap.h
 * @brief Cross-platform memory-mapped file operations
 * 
 * This header provides a unified interface for memory-mapped file operations
 * across Windows and Unix-like systems. Memory mapping allows files to be
 * accessed as if they were in memory, providing efficient I/O operations.
 */

#ifdef _WINDOWS
#define GETPID _getpid
#else
#include <unistd.h>
#define GETPID getpid
#endif

#include <cstdint>
#include <string>

namespace infra {

/**
 * @brief Loads a file into memory using memory mapping
 * 
 * This function creates a memory mapping of a file, allowing it to be accessed
 * as if it were in memory. This is particularly useful for large files that
 * need to be accessed randomly or for implementing shared memory between processes.
 * 
 * @param path The file path to memory map
 * @param size The size of the memory mapping in bytes
 * @param is_writing Whether the mapping should allow write access (default: false)
 * @param lazy Whether to use lazy loading (default: true)
 *              - If true: Pages are loaded on-demand when accessed
 *              - If false: Pages are pre-loaded and locked in memory
 * 
 * @return The memory address where the file is mapped
 * @throws std::runtime_error if the mapping operation fails
 * 
 * @note On Unix systems, when lazy=false, the memory is locked using mlock()
 *       and advised to use MADV_RANDOM for better performance
 */
[[nodiscard]] uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing = false,
                                         bool lazy = true);

/**
 * @brief Releases a memory-mapped buffer
 * 
 * This function unmaps the memory region and releases any associated resources.
 * On Unix systems, it also unlocks memory if it was previously locked.
 * 
 * @param address The memory address returned by load_mmap_buffer()
 * @param size The size of the mapped region (unused on Windows)
 * @param lazy Whether the buffer was loaded with lazy loading
 * 
 * @return true if the operation was successful, false otherwise
 * 
 * @note On Windows, the function flushes any pending writes before unmapping
 */
bool release_mmap_buffer(uintptr_t address, [[maybe_unused]] size_t size, bool lazy);

} // namespace infra