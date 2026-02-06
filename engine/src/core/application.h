#pragma once

#include <defines.h>
#include <platform/platform.h>

namespace Mango {

    // will grow
    struct ApplicationState {
        PlatformState* platform_state_;

        f64 last_time;
        b8 is_running;
        b8 is_suspended;

        ApplicationState() = default;
    };

    class MGO_API Application {
        public:
            Application(const AppAttribs& app_attribs = AppAttribs());
            ~Application();

            Application& operator=(const Application& other) = delete;
            Application& operator=(const Application&& other) = delete;
            Application(const Application& other) = delete;
            Application(const Application&& other) = delete;

            void run();

            inline const ApplicationState& get_state() {return app_state_; }

        private:
            ApplicationState app_state_;
    };

    Application* create_application(); // NOTE: defined by client
}