#pragma once

#include <defines.h>
#include <platform/platform.h>


struct game;

// will grow
struct ApplicationState {
    b8 is_running;
    b8 is_suspended;
    i16 width;
    i16 height;
    Platform* platform_state;
    game* game_inst;
    f64 last_time;
};

class Application {
    public:
        Application() {}
        ~Application() {}

        MGO_API b8 create_app(game* game_inst);
        MGO_API void run();
        MGO_API void shutdown();

        Application& operator=(const Application& other) = delete;
        Application& operator=(const Application&& other) = delete;
        Application(const Application& other) = delete;
        Application(const Application&& other) = delete;

        inline const ApplicationState& get_state() {return app_state_; }

    private:
        ApplicationState app_state_;
};

Application* create_application(); // NOTE: defined by client
