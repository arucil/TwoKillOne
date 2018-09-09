#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include "sdl2pp.h"
#include "Game.h"

using namespace sdl2pp;


#ifdef EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE
extern "C" void fireUserEvent(int code, int data) {
    Game::fireUserEvent(code, data);
}
#endif

void mainLoop(Game *game) {
    try {
        game->runOnce();
    } catch (const Exception &e) {
        SDL_Log("%s\n", e.what());
#ifdef EMSCRIPTEN
        emscripten_cancel_main_loop();
#else
        game->quit = true;
#endif
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
        while (!game->quit) {
            game->runOnce();
            SDL_Delay(20);
        }

        delete game;
#endif

    } catch (const Exception &e) {
        SDL_Log("%s\n", e.what());
        return -1;
    }

    return 0;
}