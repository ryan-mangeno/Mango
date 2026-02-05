#include "logger.h"
#include <platform/platform.h>

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

namespace Mango {
    b8 Logger::initialize_logging() {
        // TODO: create log files
        return true;
    }

    void Logger::shutdown_logging() {
        // TODO: cleanup logging/write queued entries
    }

    void Logger::log_message(log_level level, const char* message, ...) {
        
        const char* level_strs[6] = {
            "[FATAL]: ",
            "[ERROR]: ",
            "[WARN]:  ",
            "[INFO]:  ",
            "[DEBUG]: ",
            "[TRACE]: "
        };

        const char* level_str = level_strs[static_cast<int>(level)];

        b8 is_err = (level == log_level::LOG_LEVEL_FATAL || level == log_level::LOG_LEVEL_ERROR);
        const i32 msg_len = 4096;
        char message_buffer[msg_len];
        char final_buffer[msg_len];

        memset(message_buffer, 0, sizeof(message_buffer));
        memset(final_buffer, 0, sizeof(final_buffer));

         // NOTE: Using __builtin_va_list for compatibility across different compilers for
        __builtin_va_list args;
        va_start(args, message);
        vsnprintf(message_buffer, sizeof(message_buffer), message, args);
        va_end(args);
        snprintf(final_buffer, sizeof(final_buffer), "%s%s\n", level_str, message_buffer);
        
        if (is_err) {
            PlatformState::get().console_write(final_buffer, level);
        } else {
            PlatformState::get().console_write_error(final_buffer, level);
        }
    }
} // namespace Mango

