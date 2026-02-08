#pragma once 

#include <defines.h>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1
#define LOG_TRACE_ENABLED 1

// disable debug and trace logs in release builds
#if MGO_RELEASE == 1
    #define LOG_DEBUG_ENABLED 0
    #define LOG_TRACE_ENABLED 0
#endif 


enum class log_level : i8 {
    LOG_LEVEL_FATAL = 0,
    LOG_LEVEL_ERROR = 1,
    LOG_LEVEL_WARN  = 2,
    LOG_LEVEL_INFO  = 3,
    LOG_LEVEL_DEBUG = 4,
    LOG_LEVEL_TRACE = 5
};


// TODO: Implement logging backend
// colors, file output, other vars user can set ...
class Logger {
    public:
        static Logger& get() {
            static Logger instance;
            return instance;
        }

        b8 initialize_logging();
        void shutdown_logging();
        MGO_API void log_message(log_level level, const char* message, ...);
    private:
        Logger() = default;
};


#define MGO_FATAL(msg, ...) Logger::get().log_message(log_level::LOG_LEVEL_FATAL, msg, ##__VA_ARGS__)
#ifndef MGO_ERROR
    #define MGO_ERROR(msg, ...) Logger::get().log_message(log_level::LOG_LEVEL_ERROR, msg, ##__VA_ARGS__)
#endif

#if LOG_WARN_ENABLED == 1
    #define MGO_WARN(msg, ...)  Logger::get().log_message(log_level::LOG_LEVEL_WARN,  msg, ##__VA_ARGS__)
#else 
    #define MGO_WARN(msg, ...)
#endif 
#if LOG_INFO_ENABLED == 1
    #define MGO_INFO(msg, ...)  Logger::get().log_message(log_level::LOG_LEVEL_INFO,  msg, ##__VA_ARGS__)
#else
    #define MGO_INFO(msg, ...)
#endif
#if LOG_DEBUG_ENABLED == 1
    #define MGO_DEBUG(msg, ...) Logger::get().log_message(log_level::LOG_LEVEL_DEBUG, msg, ##__VA_ARGS__)
#else
    #define MGO_DEBUG(msg, ...)
#endif
#if LOG_TRACE_ENABLED == 1
    #define MGO_TRACE(msg, ...) Logger::get().log_message(log_level::LOG_LEVEL_TRACE, msg, ##__VA_ARGS__)
#else
    #define MGO_TRACE(msg, ...)
#endif

