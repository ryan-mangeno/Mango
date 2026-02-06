#include <entry.h>


using namespace Mango;

b8 initialize(struct game* game_inst);
b8 update(struct game* game_inst, f32 delta_time);
b8 render(struct game* game_inst, f32 delta_time);
b8 on_resize(struct game* game_inst, f32 delta_time);


b8 create_game(game* out_game) {
    out_game->initialize = initialize;
    out_game->render = render;
    out_game->on_resize = on_resize;
    out_game->update = update;

    return true;
}

b8 initialize(struct game* game_inst) {
    return true;
}

// function pointer to game's update function
b8 update(struct game* game_inst, f32 delta_time) {
    return true;
}

// function pointer to game's render function
b8 render(struct game* game_inst, f32 delta_time) {
    return true;
}

// function pointer to handle resizes if applicable
b8 on_resize(struct game* game_inst, f32 delta_time) {
    return true;
}