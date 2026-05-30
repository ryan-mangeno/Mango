#pragma once

#include "defines.h"
#include "platform/platform.h"

#include "clock.h"
#include "core/event.h"

struct game;



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

    private:
        struct State {
            b8 is_running;
            b8 is_suspended;
            i16 width;
            i16 height;
            Platform* platform_state;
            game* game_inst;
            Clock clock;
        };

        static b8 _on_event(u16 code, void* sender, void* listener_inst, EventContext ctx);
        static b8 _on_key(u16 code, void* sender, void* listener_inst, EventContext ctx);
    
    private:
        static State s_app_state;
        static Application* s_instance;
};

Application* create_application(); // NOTE: defined by client
