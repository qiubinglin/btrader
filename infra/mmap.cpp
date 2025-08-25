#include "mmap.h"

#ifdef _WINDOWS
#include <fcntl.h>
#include <windows.h>
#else

#include <sys/fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#endif // _WINDOWS

#include <stdexcept>

namespace infra {

/**
 * @brief Loads a file into memory using memory mapping
 * 
 * This function provides cross-platform memory mapping functionality:
 * - On Windows: Uses CreateFile, CreateFileMapping, and MapViewOfFile
 * - On Unix: Uses open, mmap, and optional mlock/madvise
 * 
 * The function handles both read-only and read-write mappings, with support
 * for lazy loading (on-demand page loading) vs. eager loading (pre-loading).
 */
uintptr_t load_mmap_buffer(const std::string &path, size_t size, bool is_writing, bool lazy) {
#ifdef _WINDOWS
    // Windows implementation using Win32 API
    
    // Determine if this is a master (writer) or slave (reader) mapping
    // Master mode is used when writing or when lazy loading is disabled
    bool master = is_writing || !lazy;
    
    // Create or open the file with appropriate access permissions
    HANDLE dumpFileDescriptor = CreateFileA(path.c_str(), (master) ? (GENERIC_READ | GENERIC_WRITE) : GENERIC_READ,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                            (master) ? OPEN_ALWAYS : OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (dumpFileDescriptor == INVALID_HANDLE_VALUE) {
        throw journal_error("unable to mmap for page " + path);
    }

    // Create a file mapping object that represents the file in memory
    // The mapping object can be shared between processes
    HANDLE fileMappingObject =
        CreateFileMapping(dumpFileDescriptor, NULL, (master) ? PAGE_READWRITE : PAGE_READONLY, 0, size, NULL);

    if (fileMappingObject == NULL) {
        int nRet = GetLastError();
        SPDLOG_ERROR("{} CreateFileMapping Error = {}, {}\n", master ? "writer" : "reader", nRet, path);
        throw journal_error("unable to mmap for page " + path);
    }

    // Map the file mapping object into the process address space
    // This gives us a pointer to the file contents in memory
    void *buffer = MapViewOfFile(fileMappingObject, (master) ? FILE_MAP_ALL_ACCESS : FILE_MAP_READ, 0, 0, size);

    if (buffer == nullptr) {
        int nRet = GetLastError();
        throw journal_error("failed to load page " + path + ", MapViewOfFile Error " + std::to_string(nRet));
    }
    
    // Clean up handles - the mapping remains valid until UnmapViewOfFile is called
    CloseHandle(fileMappingObject);
    CloseHandle(dumpFileDescriptor);
#else
    // Unix/Linux implementation using POSIX APIs
    
    // Determine if this is a master (writer) or slave (reader) mapping
    bool master = is_writing || !lazy;
    
    // Open the file with appropriate permissions
    // O_CREAT creates the file if it doesn't exist (for writers)
    // Mode 0600 gives read/write permissions to owner only
    int fd = open(path.c_str(), (master ? O_RDWR : O_RDONLY) | O_CREAT, (mode_t)0600);
    if (fd < 0) {
        throw std::runtime_error("failed to open file for page " + path);
    }

    if (master) {
        // For writers, ensure the file is at least 'size' bytes long
        // This is necessary because mmap requires the file to be large enough
        
        // Seek to size-1 bytes from the beginning
        if (lseek(fd, size - 1, SEEK_SET) == -1) {
            close(fd);
            throw std::runtime_error("failed to stretch for page " + path);
        }
        
        // Write a single byte to extend the file to the required size
        if (write(fd, "", 1) == -1) {
            close(fd);
            throw std::runtime_error("unable to write for page " + path);
        }
    }

    /**
     * MAP_FIXED is dup2 for memory mappings, and it's useful in exactly the same situations where dup2 is useful for
     * file descriptors: when you want to perform a replace operation that atomically reassigns a resource identifier
     * (memory range in the case of MAP_FIXED, or fd in the case of dup2) to refer to a new resource without the
     * possibility of races where it might get reassigned to something else if you first released the old resource then
     * attempted to regain it for the new resource.
     */
    
    // Create the memory mapping
    // MAP_SHARED allows the mapping to be shared between processes
    // PROT_READ | PROT_WRITE gives read/write access for writers, PROT_READ only for readers
    void *buffer = mmap(0, size, master ? (PROT_READ | PROT_WRITE) : PROT_READ, MAP_SHARED, fd, 0);

    if (buffer == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Error mapping file to buffer");
    }

    // If not using lazy loading, optimize memory access and lock pages in memory
    if (!lazy && madvise(buffer, size, MADV_RANDOM) != 0 && mlock(buffer, size) != 0) {
        // MADV_RANDOM advises the kernel that page access will be random
        // mlock locks the pages in memory, preventing them from being swapped out
        
        // Clean up on failure
        munmap(buffer, size);
        close(fd);
        throw std::runtime_error("failed to lock memory for page " + path);
    }

    // Close the file descriptor - the mapping remains valid
    close(fd);
#endif // _WINDOWS

    // Return the memory address as an integer for cross-platform compatibility
    return reinterpret_cast<uintptr_t>(buffer);
}

/**
 * @brief Releases a memory-mapped buffer
 * 
 * This function handles cleanup of memory mappings on both platforms:
 * - Windows: Flushes any pending writes and unmaps the view
 * - Unix: Unlocks memory (if previously locked) and unmaps the region
 */
bool release_mmap_buffer(uintptr_t address, [[maybe_unused]] size_t size, bool lazy) {
    void *buffer = reinterpret_cast<void *>(address);
    
#ifdef _WINDOWS
    // Windows cleanup: flush any pending writes to disk before unmapping
    FlushViewOfFile(buffer, 0);
    UnmapViewOfFile(buffer);
#else
    // Unix cleanup: unlock memory (if it was locked) and unmap the region
    
    // Unlock memory if it was previously locked (non-lazy mode)
    if (!lazy && munlock(buffer, size) != 0) {
        return false;
    }

    // Unmap the memory region
    if (munmap(buffer, size) != 0) {
        return false;
    }
#endif // _WINDOWS
    
    return true;
}

} // namespace infra