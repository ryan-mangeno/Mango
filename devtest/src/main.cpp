#include <core/logger.h>
#include <core/assert.h>

#include <platform/platform.h>


int main(int argc, char** argv) {
    MGO_INFO("Mango Engine starting up...");
    MGO_DEBUG("This is a debug message.");
    MGO_WARN("This is a warning message.");
    //MGO_ASSERT_MSG(false, "Test Assert");
    MGO_ERROR("This is an error message.");
    
    Mango::PlatformState& p_state = Mango::PlatformState::get();
    p_state.startup();

    while (p_state.is_running()) {
        p_state.pump_message();
    }

    MGO_INFO("Shutting Down!");
    p_state.shutdown();

    return 0;
}