#include "game.h"

#include <entry.h>

#include <platform/platform.h> // TODO: remove this

using namespace Mango;

b8 create_game(game* out_game) {

    out_game->app_config_ = AppConfig("Mango Engine - Dev Test", 100, 100, 1280, 720);

    out_game->initialize = initialize;
    out_game->render = render;
    out_game->on_resize = on_resize;
    out_game->update = update;

    out_game->state_ = Platform::allocate(sizeof(game_state), FALSE);

    return TRUE;
}