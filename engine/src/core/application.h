#pragma once

#include <defines.h>
#include <platform/platform.h>


struct game;

// will grow
struct ApplicationState {
    b8 is_running_;
    b8 is_suspended_;
    Platform platform_state_;
    i16 width_;
    i16 height_;
    game* game_inst_;
    f64 last_time_;

};

class Application {
    public:
        Application() {}
        ~Application() {}

        MGO_API b8 create_app(game* game_inst);
        MGO_API void run();

        Application& operator=(const Application& other) = delete;
        Application& operator=(const Application&& other) = delete;
        Application(const Application& other) = delete;
        Application(const Application&& other) = delete;


        inline const ApplicationState& get_state() {return app_state_; }

    private:
        ApplicationState app_state_;
};

Application* create_application(); // NOTE: defined by client
