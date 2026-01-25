#include "logger.hpp"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace Mango {
    b8 initialize_logging() {
        // TODO: create log files
        return TRUE;
    }

    void shutdown_logging() {
        // TODO: cleanup logging/write queued entries
    }

    void log_message(log_level level, const char* message, ...) {
        const char* level_str = "";
        switch (level) {
            case log_level::LOG_LEVEL_FATAL: level_str = "FATAL"; break;
            case log_level::LOG_LEVEL_ERROR: level_str = "ERROR"; break;
            case log_level::LOG_LEVEL_WARN:  level_str = "WARN";  break;
            case log_level::LOG_LEVEL_INFO:  level_str = "INFO";  break;
            case log_level::LOG_LEVEL_DEBUG: level_str = "DEBUG"; break;
            case log_level::LOG_LEVEL_TRACE: level_str = "TRACE"; break;
        }

        // TODO: setup output color based on level, and send to stderr for errors potentially
        // b8 is_err = (level == log_level::LOG_LEVEL_FATAL || level == log_level::LOG_LEVEL_ERROR);
        char message_buffer[4096];
        char final_buffer[8192];

        memset(message_buffer, 0, sizeof(message_buffer));
        memset(final_buffer, 0, sizeof(final_buffer));

         // NOTE: Using __builtin_va_list for compatibility across different compilers for
        __builtin_va_list args;
        va_start(args, message);
        vsnprintf(message_buffer, sizeof(message_buffer), message, args);
        va_end(args);
        sprintf(final_buffer, "[%s]: %s\n", level_str, message_buffer);

        // TODO: platform specific output (e.g. OutputDebugString on Windows)
        printf("%s", final_buffer);
    }
} // namespace Mango

