#include <cstdio>
#include <sstream>
#include <SDL2/SDL_image.h>
#include "Game.h"

using std::printf;
using namespace std::experimental;


Game::Game() :
        window("二打一棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        whiteChessmanTexture(renderer, "assets/white.png"),
        blackChessmanTexture(renderer, "assets/black.png"),
        boardTexture(renderer, "assets/board.png") {
    initEvents();
}

void Game::initEvents() {
    eventDispatcher.quitEvent = [](const auto &) {
        emscripten_cancel_main_loop();
    };

    eventDispatcher.userEvent = [this](const auto &e) {
        handleUserEvents(e);
    };
}

bool Game::fireUserEvent(int code, int data) {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = reinterpret_cast<void *>(data);

    int ret = SDL_PushEvent(&event);
    if (ret > 0) {
        return true;
    } else if (ret == 0) {
        return false;
    } else {
        std::ostringstream strm;
        strm << "Game::fireUserEvent(): " << SDL_GetError();
        throw Exception(strm.str());
    }
}

void Game::handleUserEvents(const SDL_UserEvent &e) {
    printf("%d\n", e.code);
}

void Game::update() {
}

void Game::render() {
    renderer.setDrawColor(0xff, 0x80, 0x80, 0xff);
    renderer.fill();

    renderer.copy(boardTexture, optional(Rect { 0, 0, 300, 300 }), optional(Rect { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }));

    renderer.present();
}
