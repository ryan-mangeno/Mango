// INCLUDED ONCE BY CLIENT
#ifdef ENTRY_H 
    static_assert(false, "Entry point defined multiple times!")
#else 
#define ENTRY_H

#include "core/logger.h"
#include "core/application.h"
#include "game_types.h"
#include "core/mgmemory.h"

// defined by client
b8 create_game(game* out_game);

int main(int argc, char** argv) {

    mg_initialize_memory();

    game game_inst;
    if (!create_game(&game_inst)) {
        MGO_FATAL("failed to create game!");
        return -1;
    }

    if (! (game_inst.render && game_inst.initialize && game_inst.update && game_inst.on_resize) ) {
        MGO_FATAL("missing one or more game functions <render, update, on_resize>");
        return -2;
    }
    
    Application app;
    if (!app.create_app(&game_inst)) {
        MGO_FATAL("failed to create application!");
        return 1;
    }
    app.run();
    app.shutdown();

    mg_shutdown_memory();

    return 0;
}

#endif