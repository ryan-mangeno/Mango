#include "application.h"

#include <core/logger.h>
#include <core/assert.h>
#include <utility/type_traits.h>
#include <game_types.h>
#include <core/mgmemory.h>

#include <containers/darray.h>

static b8 initialized_ = false;


b8 Application::create_app(game* game_inst) {
    MGO_ASSERT_MSG(!initialized_, "application already initialized!");

    Logger::get().initialize_logging();
    MGO_INFO("Mango Engine starting up ...");

    if(!game_inst) {
        MGO_ERROR("game instance is null!");
        return FALSE;
    }

    app_state_.game_inst_ = game_inst;
    app_state_.width_ = game_inst->app_config_.width;
    app_state_.height_ = game_inst->app_config_.height;
    app_state_.is_running_ = FALSE;
    app_state_.is_suspended_ = FALSE;

    if (!app_state_.platform_state_.startup(game_inst->app_config_)) {
        MGO_ERROR("Engine failed startup!");
        app_state_.platform_state_.shutdown();
        return FALSE;
    }

    if (!app_state_.game_inst_->initialize(app_state_.game_inst_)) {
        MGO_ERROR("game failed to initialize!");
        app_state_.platform_state_.shutdown();
        return FALSE;
    }

    // app_state.game_inst_->on_resize(app_state_.width_, app_state_.height_); TODO: 

    app_state_.is_running_ = TRUE;
    app_state_.is_suspended_ = FALSE;
    initialized_ = TRUE;

    return TRUE;
}

void Application::run() {
    MGO_INFO("Running ...");
    Platform& state = app_state_.platform_state_;

    MGO_INFO(mg_get_memory_usage_str());

    while (app_state_.is_running_) {
        if (state.pump_message()) {
            app_state_.is_running_ = FALSE;
        }

        if (!app_state_.is_suspended_) {
            f64 current_time = state.get_absolute_time();
            f64 delta_time = current_time - app_state_.last_time_;
            app_state_.last_time_ = current_time;

            if (!app_state_.game_inst_->update(app_state_.game_inst_, static_cast<f32>(delta_time))) {
                MGO_ERROR("game update failed!");
                break;
            }

            if (!app_state_.game_inst_->render(app_state_.game_inst_, static_cast<f32>(delta_time))) {
                MGO_ERROR("game render failed!");
                break;
            }
        }
    }  

    // we might want to exit the loop for other reasons
    app_state_.is_running_ = FALSE;

    MGO_INFO("Shutting Down!");
    state.shutdown();
}

