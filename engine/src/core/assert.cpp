#include "assert.h"
#include "logger.h"

namespace Mango {

    void report_assertion_failure(const char* expression, const char* message, const char* file, i32 line) {
        MGO_FATAL("Assertion failed: %s\nFile: %s\nLine: %d", expression, file, line);
    }   

}