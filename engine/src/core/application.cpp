#include "application.h"

#include <core/logger.h>
#include <core/assert.h>
#include <utility/type_traits.h>
#include <game_types.h>
#include <core/mgmemory.h>

#include <containers/darray.h>
#include "event.h"
#include "input.h"

static b8 s_initialized = false;

b8 Application::create_app(game* game_inst) {
    MGO_ASSERT_MSG(!s_initialized, "application already initialized!");

    Logger::get().initialize_logging();
    MGO_INFO("Mango Engine starting up ...");

    if(!game_inst) {
        MGO_ERROR("game instance is null!");
        return FALSE;
    }

    app_state_.game_inst = game_inst;
    app_state_.width = game_inst->app_config_.width;
    app_state_.height = game_inst->app_config_.height;
    app_state_.is_running = FALSE;
    app_state_.is_suspended = FALSE;

    if (!input_initialize()) {
        MGO_ERROR("Input system failed to initialize, application can't continue ...");
        return FALSE;
    }

    if (!event_initialize()) {
        MGO_ERROR("Event system failed to initialize, application can't continue ...");
        return FALSE;
    }

    app_state_.platform_state = mg_placement_new<Platform>(MEMORY_TAG_APPLICATION);

    if (!app_state_.platform_state->startup(game_inst->app_config_)) {
        MGO_ERROR("Engine failed startup!");
        return FALSE;
    }

    if (!app_state_.game_inst->initialize(app_state_.game_inst)) {
        MGO_ERROR("game failed to initialize!");
        return FALSE;
    }

    // app_state.game_inst->on_resize(app_state_.width_, app_state_.height_); TODO: 

    app_state_.is_running = TRUE;
    app_state_.is_suspended = FALSE;
    s_initialized = TRUE;

    return TRUE;
}

void Application::run() {
    MGO_INFO("Running ...");
    Platform& state = *(app_state_.platform_state);

    MGO_INFO(mg_get_memory_usage_str());

    while (app_state_.is_running) {
        if (state.pump_message()) {
            app_state_.is_running = FALSE;
        }

        if (!app_state_.is_suspended) {
            f64 current_time = state.get_absolute_time();
            f64 delta_time = current_time - app_state_.last_time;
            app_state_.last_time = current_time;

            if (!app_state_.game_inst->update(app_state_.game_inst, static_cast<f32>(delta_time))) {
                MGO_ERROR("game update failed!");
                break;
            }

            if (!app_state_.game_inst->render(app_state_.game_inst, static_cast<f32>(delta_time))) {
                MGO_ERROR("game render failed!");
                break;
            }
        }
    }  

    app_state_.is_running = FALSE;
}


void Application::shutdown() {

    MGO_INFO("Shutting Down!");

    // we could've failed before initialization, event_shutdown is safe 
    // but platform_state shutdown isn't if initialized isnt true
    if (s_initialized) {
        input_shutdown();
        event_shutdown();
        app_state_.platform_state->shutdown();
    }

    if( app_state_.platform_state ) {
        mg_placement_delete(app_state_.platform_state, MEMORY_TAG_APPLICATION);
        app_state_.platform_state = nullptr;
    }
}

