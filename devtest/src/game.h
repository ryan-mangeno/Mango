#pragma once

#include <defines.h>
#include <game_types.h>

struct game_state {
    f64 delta_time;
};

b8 initialize(struct game* game_inst);
b8 update(struct game* game_inst, f64 delta_time);
b8 render(struct game* game_inst, f64 delta_time);
b8 on_resize(struct game* game_inst, f64 delta_time);