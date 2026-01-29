#include <core/logger.hpp>
#include <core/assert.hpp>

using namespace Mango;

int main(int argc, char** argv) {
    MGO_INFO("Mango Engine starting up...");
    MGO_DEBUG("This is a debug message.");
    MGO_WARN("This is a warning message.");
    MGO_ASSERT_MSG(false, "Test Assert");
    MGO_ERROR("This is an error message.");
    return 0;
}