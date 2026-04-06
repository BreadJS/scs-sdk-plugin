#ifndef SHAREDMEMORY_PLATFORM_HPP
#define SHAREDMEMORY_PLATFORM_HPP

#include "platform.hpp"
#include <cstddef>

class SharedMemoryPlatform {
public:
    // Type definitions for cross-platform compatibility
    typedef shm_handle_t handle_type;
    typedef shm_name_t name_type;

    // Platform-specific error handling
    static int get_last_error();

    // Shared memory operations
    static handle_type create_or_open(name_type name, size_t size);
    static void* map(handle_type handle, size_t size);
    static bool unmap(void* address, size_t size);
    static bool close(handle_type handle);
};

#endif // SHAREDMEMORY_PLATFORM_HPP
