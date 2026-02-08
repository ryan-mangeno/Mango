#include "game.h"

#include <core/logger.h>

using namespace Mango;

b8 initialize(struct game* game_inst) {
    MGO_DEBUG("game initialize called!");
    return TRUE;
}

// function pointer to game's update function
b8 update(struct game* game_inst, f32 delta_time) {
    MGO_DEBUG("game update called! delta_time: %f", delta_time);
    return TRUE;
}

// function pointer to game's render function
b8 render(struct game* game_inst, f32 delta_time) {
    MGO_DEBUG("game render called! delta_time: %f", delta_time);
    return TRUE;
}

// function pointer to handle resizes if applicable
b8 on_resize(struct game* game_inst, f32 delta_time) {
    MGO_DEBUG("game on_resize called! delta_time: %f", delta_time);
    return TRUE;
}