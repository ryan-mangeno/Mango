// INCLUDED ONCE BY CLIENT
#ifdef ENTRY_H 
    static_assert(false, "Entry point defined multiple times!")
#else 
    #define ENTRY_H

#include "core/logger.h"
#include "core/application.h"
#include "game_types.h"

// defined by client
extern Mango::b8 create_game(Mango::game* out_game);

int main(int argc, char** argv) {

    Mango::game game_inst;
    if (!create_game(&game_inst)) {
        MGO_FATAL("failed to create game!");
        return -1;
    }

    if (! (game_inst.render && game_inst.initialize && game_inst.update && game_inst.on_resize) ) {
        MGO_FATAL("missing one or more game functions <render, update, on_resize>");
        return -2;
    }
    
    Mango::Application app;
    app.run();

    return 0;
}

#endif