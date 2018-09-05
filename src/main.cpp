#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <sstream>
#include "sdl2pp.h"
#include "Game.h"

using namespace sdl2pp;


EMSCRIPTEN_KEEPALIVE
extern "C" void fireUserEvent(Game *game, int code, int data) {
    game->fireUserEvent(code, data);
}

void mainLoop(Game *game) {
    try {
        game->runOnce();
    } catch (const Exception &e) {
        SDL_Log("%s\n", e.what());
        emscripten_cancel_main_loop();
    }
}

extern "C" int main() {
    Game *game;

    try {
        Init initSdl(SDL_INIT_VIDEO);
        InitImage initImage(IMG_INIT_PNG);

        game = new Game;

#ifdef EMSCRIPTEN
        emscripten_set_main_loop_arg(reinterpret_cast<void (*)(void *)>(mainLoop), game, -1, 1);
#else
        // native application

        delete game;
#endif

    } catch (const Exception &e) {
        SDL_Log("%s\n", e.what());
        return -1;
    }

    return 0;
}