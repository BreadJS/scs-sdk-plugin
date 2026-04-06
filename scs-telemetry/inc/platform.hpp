#ifndef PLATFORM_HPP
#define PLATFORM_HPP

// Platform detection
#if defined(_WIN32) || defined(_WIN64)
    #define SCS_PLATFORM_WINDOWS
#elif defined(__linux__)
    #define SCS_PLATFORM_LINUX
#else
    #error "Unsupported platform"
#endif

// Platform-specific includes and types
#ifdef SCS_PLATFORM_WINDOWS
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif
    #include <windows.h>
    typedef HANDLE shm_handle_t;
    typedef LPCWSTR shm_name_t;
    #define SCS_INVALID_HANDLE nullptr
    #define SCS_MAP_FAILED nullptr
#elif defined(SCS_PLATFORM_LINUX)
    #define _GNU_SOURCE
    #include <sys/mman.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <errno.h>
    typedef int shm_handle_t;
    typedef const char* shm_name_t;
    #define SCS_INVALID_HANDLE (-1)
    #define SCS_MAP_FAILED MAP_FAILED
#endif

// Shared memory naming
#ifdef SCS_PLATFORM_WINDOWS
    #define SCS_SHM_NAME L"SCSTelemetry"
#elif defined(SCS_PLATFORM_LINUX)
    #define SCS_SHM_NAME "/SCSTelemetry"
#endif

// Safe string functions
#ifdef SCS_PLATFORM_WINDOWS
    #define SCS_VSNPRINTF(buffer, size, format, args) vsnprintf_s(buffer, size, _TRUNCATE, format, args)
#else
    #define SCS_VSNPRINTF(buffer, size, format, args) vsnprintf(buffer, size, format, args)
#endif

// Platform-specific error handling
#ifdef SCS_PLATFORM_WINDOWS
    #define SCS_GET_LAST_ERROR() static_cast<int>(GetLastError())
    #define SCS_ERROR_ALREADY_EXISTS 183
#elif defined(SCS_PLATFORM_LINUX)
    #define SCS_GET_LAST_ERROR() errno
    #define SCS_ERROR_ALREADY_EXISTS EEXIST
#endif

// DLL/SO export macros
#ifdef SCS_PLATFORM_WINDOWS
    #ifdef SCS_TELEMETRY_EXPORTS
        #define SCS_API __declspec(dllexport)
    #else
        #define SCS_API __declspec(dllimport)
    #endif
#elif defined(SCS_PLATFORM_LINUX)
    #define SCS_API __attribute__((visibility("default")))
    // Override SCSAPIFUNC to include visibility attribute for exported functions
    #ifdef SCS_TELEMETRY_EXPORTS
        #undef SCSAPIFUNC
        #define SCSAPIFUNC __attribute__((visibility("default")))
    #endif
#endif

// Log file path prefix
#ifdef SCS_PLATFORM_WINDOWS
    #define SCS_LOG_PATH_PREFIX "C:\\ets2telem_"
#elif defined(SCS_PLATFORM_LINUX)
    #define SCS_LOG_PATH_PREFIX "/tmp/ets2telem_"
#endif

#endif // PLATFORM_HPP
