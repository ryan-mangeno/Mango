#include <core/logger.hpp>

int main(int argc, char** argv) {
    MGO_INFO("Mango Engine starting up...");
    MGO_DEBUG("This is a debug message.");
    MGO_WARN("This is a warning message.");
    MGO_ERROR("This is an error message.");
    MGO_FATAL("This is a fatal message. %f", 3.14159);
    return 0;
}