#pragma once

#include "core/application.h"

namespace Mango {

    struct game {

        // game specific game state, created and managed by game
        void* state;
        AppAttribs app_attribs;
        
        // function pointer to game's init function
        b8 (*initialize)(struct game* game_inst);

        // function pointer to game's update function
        b8 (*update)(struct game* game_inst, f32 delta_time);

        // function pointer to game's render function
        b8 (*render)(struct game* game_inst, f32 delta_time);

        // function pointer to handle resizes if applicable
        b8 (*on_resize)(struct game* game_inst, f32 delta_time);

        game() : initialize(nullptr), update(nullptr), render(nullptr), on_resize(nullptr),
                 state(nullptr), app_attribs() {}
    };

}