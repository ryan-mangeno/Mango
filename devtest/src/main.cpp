#include <core/logger.h>
#include <core/assert.h>

#include <platform/platform.h>

using namespace Mango;

int main(int argc, char** argv) {
    MGO_INFO("Mango Engine starting up...");
    MGO_DEBUG("This is a debug message.");
    MGO_WARN("This is a warning message.");
    MGO_WARN("Sizes: Float - %zu bytes, Int - %zu bytes, long - %zu bytes, char - %zu bytes, double - %zu bytes", sizeof(float), sizeof(int), sizeof(long), sizeof(char), sizeof(double));
    //MGO_ASSERT_MSG(false, "Test Assert");
    MGO_ERROR("This is an error message.");
    
    PlatformState& p_state = PlatformState::get();
    p_state.startup();

    bool is_running = true;
    while (is_running) {
        if (p_state.pump_message()) {
            is_running = false;
        }
        p_state.sleep(5000);
        MGO_INFO("%lf", p_state.get_absolute_time());
    }

    MGO_INFO("Shutting Down!");
    p_state.shutdown();

    return 0;
}