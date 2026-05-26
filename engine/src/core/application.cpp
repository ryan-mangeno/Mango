#include "application.h"

#include "event.h"
#include "input.h"
#include "clock.h"

#include <core/logger.h>
#include <core/assert.h>
#include <utility/type_traits.h>
#include <game_types.h>
#include <core/mgmemory.h>
#include <containers/darray.h>


static b8 s_initialized = false;

b8 Application::create_app(game* game_inst) {
    MGO_ASSERT_MSG(!s_initialized, "application already initialized!");

    Logger::get().initialize_logging();
    MGO_INFO("Mango Engine starting up ...");

    if(!game_inst) {
        MGO_ERROR("game instance is null!");
        return FALSE;
    }

    ApplicationState& app_state = get_state();

    app_state.game_inst = game_inst;
    app_state.width = game_inst->app_config.width;
    app_state.height = game_inst->app_config.height;
    app_state.is_running = FALSE;
    app_state.is_suspended = FALSE;

    if (!input_initialize()) {
        MGO_ERROR("Input system failed to initialize, application can't continue ...");
        return FALSE;
    }

    if (!event_initialize()) {
        MGO_ERROR("Event system failed to initialize, application can't continue ...");
        return FALSE;
    }

    app_state.platform_state = mg_placement_new<Platform>(MEMORY_TAG_APPLICATION);

    if (!app_state.platform_state->startup(game_inst->app_config)) {
        MGO_ERROR("Engine failed startup!");
        return FALSE;
    }

    if (!app_state.game_inst->initialize(app_state.game_inst)) {
        MGO_ERROR("game failed to initialize!");
        return FALSE;
    }

    // app_state.game_inst->on_resize(app_state.width, app_state.height); TODO: 

    app_state.is_running = TRUE;
    app_state.is_suspended = FALSE;
    s_initialized = TRUE;

    return TRUE;
}

void Application::run() {
    MGO_INFO("Running ...");
    
    ApplicationState& app_state = get_state();
    Platform& state = *(app_state.platform_state);
    Clock& clock = app_state.clock;

    MGO_INFO(mg_get_memory_usage_str());

    clock.start();
    clock.update();
    clock.set_last_time(clock.get_elapsed_time());
    f64 running_time = 0.0;
    u8 frame_count = 0;
    f64 target_fps = 1.0 / 60.0; // NOTE: should be adjustible but hardcoding for now

    while (app_state.is_running) {
        if (state.pump_message()) {
            app_state.is_running = FALSE;
        }

        if (!app_state.is_suspended) {
            clock.update();
            f64 current_time = clock.get_elapsed_time();
            f64 delta_time = current_time - clock.get_last_time();
            f64 frame_start_time = Platform::get_absolute_time();

            if (!app_state.game_inst->update(app_state.game_inst, delta_time)) {
                MGO_ERROR("game update failed!");
                break;
            }

            if (!app_state.game_inst->render(app_state.game_inst, delta_time)) {
                MGO_ERROR("game render failed!");
                break;
            }

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

    app_state.is_running = FALSE;
}


void Application::shutdown() {

    MGO_INFO("Shutting Down!");

    ApplicationState& app_state = get_state();

    // we could've failed before initialization, event_shutdown is safe 
    // but platform_state shutdown isn't if initialized isnt true
    if (s_initialized) {
        event_shutdown();
        input_shutdown();
        app_state.platform_state->shutdown();
    }

    if (app_state.platform_state) {
        mg_placement_delete(app_state.platform_state, MEMORY_TAG_APPLICATION);
        app_state.platform_state = nullptr;
    }

    MGO_INFO(mg_get_memory_usage_str());
}

