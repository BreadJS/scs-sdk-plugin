#if LOGGING
#include "platform.hpp"
#include <string>
#include <ctime>
#include "log.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>

// About: How to use
// This class is used to log to file.
// Normally it is not activated. That saves resources and
// the dll size is a lot smaller.
// To use this the simplest way is to use compiler preamble.
// Add the word 'LOGGING' to your compile preamble and this function
// and every code with the compiler '#if LOGGING' and at the end '#endif' will compile, too.
// This was implemented to make it hopefully easier to fix bugs.

// Function: Logger
// used to log to file, usage: add LOGGING to preamble to activate it globally in cpp, but use it with care not full tested yet and not many features
// also it rise dll size and may have impacted on the system (loading time, etc.)
namespace logger {

    namespace {

        // Function: time_stamp
        // current time to string
        auto time_stamp() -> std::string {
            const auto now = std::time(nullptr);
            char cstr[256]{};
            return std::strftime(cstr, sizeof(cstr), "%Y%m%d_%H%M%S", std::localtime(&now)) ? cstr : "";
        }

#ifdef SCS_PLATFORM_WINDOWS

        // Function: exe_path
        // get current path (Windows)
        auto exe_path() -> std::string {
            char buffer[1024];
            GetModuleFileNameA(nullptr, buffer, sizeof(buffer));
            const auto pos = std::string(buffer).find_last_of("\\/");
            return std::string(buffer).substr(0, pos);
        }

        // Function: create_directory
        // create a directory (Windows)
        bool create_directory(const std::string& path) {
            return CreateDirectoryA(path.c_str(), nullptr) ||
                   GetLastError() == ERROR_ALREADY_EXISTS;
        }

#elif defined(SCS_PLATFORM_LINUX)

        #include <unistd.h>
        #include <sys/stat.h>
        #include <sys/types.h>
        #include <errno.h>

        // Function: exe_path
        // get current path (Linux)
        auto exe_path() -> std::string {
            char buffer[1024];
            ssize_t count = readlink("/proc/self/exe", buffer, sizeof(buffer));
            if (count != -1) {
                buffer[count] = '\0';
            } else {
                buffer[0] = '\0';
            }
            const auto pos = std::string(buffer).find_last_of("/");
            return std::string(buffer).substr(0, pos);
        }

        // Function: create_directory
        // create a directory (Linux)
        bool create_directory(const std::string& path) {
            return mkdir(path.c_str(), 0755) == 0 || errno == EEXIST;
        }

#endif

        // Function: path_to_session_log_file
        // get the path to the current log file
        auto path_to_session_log_file() -> std::string {
            auto base_path = exe_path();
            auto log_dir = base_path + "/tmp/log/";

            if (create_directory(base_path + "/tmp/") &&
                create_directory(log_dir)) {
                static const std::string log_file_name = "log.txt";
                return log_dir + time_stamp() + '_' + log_file_name;
            }
            return "";
        }

    }

    // Const: path
    // the path to the current log file
    const std::string path = path_to_session_log_file();
    // handle: out
    // stream to file
    std::ofstream out = std::ofstream(path);
    // Function: flush
    // flush the memory
    void flush() { out.flush(); }
}

#endif
