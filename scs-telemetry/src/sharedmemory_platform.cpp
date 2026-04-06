#include "sharedmemory_platform.hpp"

// Platform-specific implementations
#ifdef SCS_PLATFORM_WINDOWS

int SharedMemoryPlatform::get_last_error() {
    return static_cast<int>(GetLastError());
}

SharedMemoryPlatform::handle_type SharedMemoryPlatform::create_or_open(
    name_type name, size_t size) {
    handle_type handle = CreateFileMapping(
        INVALID_HANDLE_VALUE,
        nullptr,
        PAGE_READWRITE,
        0,
        static_cast<DWORD>(size),
        name
    );

    if (handle == nullptr && GetLastError() == SCS_ERROR_ALREADY_EXISTS) {
        handle = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, name);
    }

    return handle;
}

void* SharedMemoryPlatform::map(handle_type handle, size_t size) {
    return MapViewOfFile(handle, FILE_MAP_ALL_ACCESS, 0, 0, static_cast<SIZE_T>(size));
}

bool SharedMemoryPlatform::unmap(void* address, size_t) {
    return UnmapViewOfFile(address) != 0;
}

bool SharedMemoryPlatform::close(handle_type handle) {
    return CloseHandle(handle) != 0;
}

#elif defined(SCS_PLATFORM_LINUX)

#include <sys/stat.h>

int SharedMemoryPlatform::get_last_error() {
    return errno;
}

SharedMemoryPlatform::handle_type SharedMemoryPlatform::create_or_open(
    name_type name, size_t size) {
    // Try to create first
    handle_type handle = shm_open(name, O_CREAT | O_EXCL | O_RDWR, 0666);

    if (handle == -1 && errno == EEXIST) {
        // Already exists, open it
        handle = shm_open(name, O_RDWR, 0666);
    }

    if (handle != -1) {
        // Set size for newly created shared memory
        struct stat st;
        if (fstat(handle, &st) == 0 && st.st_size == 0) {
            ftruncate(handle, static_cast<off_t>(size));
        }
    }

    return handle;
}

void* SharedMemoryPlatform::map(handle_type handle, size_t size) {
    return mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_SHARED, handle, 0);
}

bool SharedMemoryPlatform::unmap(void* address, size_t size) {
    return munmap(address, size) == 0;
}

bool SharedMemoryPlatform::close(handle_type handle) {
    return ::close(handle) == 0;
}

#endif
