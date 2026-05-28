#include "application.h"

#include "event.h"
#include "input.h"
#include "clock.h"

#include "core/logger.h"
#include "core/assert.h"
#include "utility/type_traits.h"
#include "game_types.h"
#include "core/mgmemory.h"
#include "containers/darray.h"
#include "renderer/renderer.h"
#include "renderer/renderer_types.h"



static b8 s_initialized = false;

Application::State Application::s_app_state = {};
Application* Application::s_instance = nullptr;

b8 Application::create_app(game* game_inst) {
    MGO_ASSERT_MSG(!s_initialized, "application already initialized!");

    Logger::get().initialize_logging();
    MGO_INFO("Mango Engine starting up ...");

    if(!game_inst) {
        MGO_ERROR("game instance is null!");
        return FALSE;
    }

    s_app_state.game_inst = game_inst;
    s_app_state.width = game_inst->app_config.width;
    s_app_state.height = game_inst->app_config.height;
    s_app_state.is_running = FALSE;
    s_app_state.is_suspended = FALSE;

    if (!input_initialize()) {
        MGO_FATAL("Input system failed to initialize, application can't continue ...");
        return FALSE;
    }

    if (!event_initialize()) {
        MGO_FATAL("Event system failed to initialize, application can't continue ...");
        return FALSE;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT, this, Application::on_event);
    event_register(EVENT_CODE_KEY_PRESSED, this, Application::on_key);
    event_register(EVENT_CODE_KEY_RELEASED, this, Application::on_key);

    s_app_state.platform_state = mg_new<Platform>(MEMORY_TAG_APPLICATION);

    if (!s_app_state.platform_state->startup(game_inst->app_config)) {
        MGO_FATAL("Engine failed startup!");
        return FALSE;
    }

    if (!renderer_init(game_inst->app_config.title)) {
        MGO_FATAL("Failed to initialize renderer. Aborting application.");
        return FALSE;
    }

    if (!s_app_state.game_inst->initialize(s_app_state.game_inst)) {
        MGO_FATAL("game failed to initialize!");
        return FALSE;
    }

    // s_app_state.game_inst->on_resize(s_app_state.width, s_app_state.height); TODO: 

    s_app_state.is_running = TRUE;
    s_app_state.is_suspended = FALSE;
    s_initialized = TRUE;

    return TRUE;
}

void Application::run() {
    MGO_INFO("Running ...");
    
    Platform& state = *(s_app_state.platform_state);
    Clock& clock = s_app_state.clock;

    MGO_INFO(mg_get_memory_usage_str());

    clock.start();
    clock.update();
    clock.set_last_time(clock.get_elapsed_time());
    f64 running_time = 0.0;
    u8 frame_count = 0;
    f64 target_fps = 1.0 / 60.0; // NOTE: should be adjustible but hardcoding for now

    while (s_app_state.is_running) {
        if (!state.pump_message()) {
            s_app_state.is_running = FALSE;
            break;
        }

        if (!s_app_state.is_suspended) {
            clock.update();
            f64 current_time = clock.get_elapsed_time();
            f64 delta_time = current_time - clock.get_last_time();
            f64 frame_start_time = Platform::get_absolute_time();

            if (!s_app_state.game_inst->update(s_app_state.game_inst, delta_time)) {
                MGO_ERROR("game update failed!");
                break;
            }

            if (!s_app_state.game_inst->render(s_app_state.game_inst, delta_time)) {
                MGO_ERROR("game render failed!");
                break;
            }

            RenderPacket packet;
            packet.delta_time = delta_time;
            renderer_draw_frame(&packet);

            f64 frame_end_time = Platform::get_absolute_time();
            f64 frame_elapsed_time = frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds = target_fps - frame_elapsed_time;

            if (remaining_seconds > 0) {
                u64 remaining_ms = (remaining_seconds * 1000);

                // if there is time left, give it back to os
                b8 limit_frames = FALSE; // Hardcoded for now
                if (remaining_ms > 0 && limit_frames) {
                    Platform::sleep(remaining_ms - 1);
                }

                frame_count++;
            }

            // NOTE: input update is handled after frame ends
            input_update(delta_time);
            clock.set_last_time(current_time);
        }
    }  

    s_app_state.is_running = FALSE;
}


void Application::shutdown() {

    MGO_INFO("Shutting Down!");

    // we could've failed before initialization, event_shutdown is safe 
    // but platform_state shutdown isn't if initialized isnt true
    if (s_initialized) {
        
        event_unregister(EVENT_CODE_APPLICATION_QUIT, this, Application::on_event);
        event_unregister(EVENT_CODE_KEY_PRESSED, this, Application::on_key);
        event_unregister(EVENT_CODE_KEY_RELEASED, this, Application::on_key);

        event_shutdown();
        input_shutdown();
        renderer_shutdown();
        s_app_state.platform_state->shutdown();
    }

    if (s_app_state.platform_state) {
        mg_delete(s_app_state.platform_state, MEMORY_TAG_APPLICATION);
        s_app_state.platform_state = nullptr;
    }

}


b8 Application::on_event(u16 code, void* sender, void* listener_inst, EventContext ctx) {
    Application* app = static_cast<Application*>(listener_inst);

    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT: {
            MGO_INFO("EVENT_CODE_APPLICATION_QUIT recieved, shutting down.\n");
            app->s_app_state.is_running = FALSE;
            return TRUE;
        }
    }
    return FALSE;
}

b8 Application::on_key(u16 code, void* sender, void* listener_inst, EventContext ctx) {
    if (code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = ctx.data.u16[0];
        if (key_code == KEY_ESCAPE) {
            // NOTE: Technically firing an event to itself, but there may be other listeners.
            EventContext data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT, listener_inst, data);

            // Block anything else from processing this.
            return TRUE;
        } else if (key_code == KEY_A) {
            // Example on checking for a key
            MGO_DEBUG("Explicit - A key pressed!");
        } else {
            MGO_DEBUG("'%c' key pressed in window.", key_code);
        }
    } else if (code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = ctx.data.u16[0];
        if (key_code == KEY_B) {
            // Example on checking for a key
            MGO_DEBUG("Explicit - B key released!");
        } else {
            MGO_DEBUG("'%c' key released in window.", key_code);
        }
    }
    return FALSE;
}