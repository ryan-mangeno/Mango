#include "application.h"

#include <core/logger.h>
#include <core/assert.h>

namespace Mango {

    static b8 initialized_ = false;

    Application::Application(const AppAttribs& app_attribs) {
        MGO_ASSERT_MSG(!initialized_, "application already initialized!");

        Logger::get().initialize_logging();
        MGO_INFO("Test Message %f", 3.14);
        MGO_WARN("Test Message %f", 3.14);
        MGO_DEBUG("Test Message %f", 3.14);
        MGO_TRACE("Test Message %f", 3.14);
        MGO_ERROR("Test Message %f", 3.14);
        MGO_FATAL("Test Message %f", 3.14);

        MGO_INFO("Mango Engine starting up ...");
        
        app_state_.platform_state_ = new PlatformState;
        bool startup_success = app_state_.platform_state_->startup(app_attribs);
        if (!startup_success) {
            MGO_ERROR("Engine failed startup!");
            app_state_.platform_state_->shutdown();
            return;
        }

        app_state_.is_running = true;
        app_state_.is_suspended = false;
        initialized_ = true;
    }

    Application::~Application() {
        delete app_state_.platform_state_;
        app_state_.platform_state_ = nullptr;
        
        initialized_ = false;
    }

    void Application::run() {
        MGO_INFO("Running ...");
        PlatformState& state = *(app_state_.platform_state_);

        while (app_state_.is_running) {
            if (state.pump_message()) {
                app_state_.is_running = false;
            }
        }  

        // we might want to exit the loop for other reasons
        app_state_.is_running = false;

        MGO_INFO("Shutting Down!");
        state.shutdown();
    }

}