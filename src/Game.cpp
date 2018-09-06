#include <cstdio>
#include <sstream>
#include <SDL2/SDL_image.h>
#include <stdexcept>
#include "events.h"
#include "Game.h"

using std::printf;
using namespace std::experimental;


Game::Game() :
        window("二打一棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        whiteChessmanTexture(renderer, "assets/white.png"),
        blackChessmanTexture(renderer, "assets/black.png"),
        boardTexture(renderer, "assets/board.png"),
        playerStatus(PlayerStatus::None) {
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
    switch (e.code) {
    case StartGame:
        newGame();
        takeTurn();
        break;
    case SetBlack:
        switch (reinterpret_cast<uintptr_t>(e.data1)) {
        case 0:
            black = PlayerType::Human; break;
        case 1:
            black = PlayerType::AI; break;
        default:
            throw std::logic_error("unreachable");
        }
        break;
    case SetWhite:
        switch (reinterpret_cast<uintptr_t>(e.data1)) {
        case 0:
            white = PlayerType::Human; break;
        case 1:
            white = PlayerType::AI; break;
        default:
            throw std::logic_error("unreachable");
        }
        break;
    default:
        throw std::logic_error("unreachable");
    }
}

/**
 * init board and game states
 */
void Game::newGame() {
    board.initBoard();
    curPlayer = Player::Black;
    playerStatus = PlayerStatus::None;
}

void Game::takeTurn() {
    if (curPlayer == Player::Black) {
        blackChessmanTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        whiteChessmanTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        whiteChessmanTexture.setAlphaMod(0xa0);

        if (black == PlayerType::Human) {
            playerStatus = PlayerStatus::CanMove;
        } else {
            ///TODO: AI
        }
    } else {
        whiteChessmanTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        blackChessmanTexture.setBlendMode(SDL_BLENDMODE_BLEND);
        blackChessmanTexture.setAlphaMod(0xa0);

        if (white == PlayerType::Human) {
            playerStatus = PlayerStatus::CanMove;
        } else {
            ///TODO: AI
        }
    }
}

void Game::update() {
}

void Game::render() {
    renderer.setDrawColor(0xff, 0x80, 0x80, 0xff);
    renderer.fill();

    renderer.copy(boardTexture, nullopt, optional(Rect { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }));

    const int X0 = 15;
    const int Y0 = 15;
    const int GRID_W = 71;
    const int GRID_H = 71;
    const int CHESSMAN_W = 70;
    const int CHESSMAN_H = 70;

    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            switch (board(y, x)) {
            case Player::Black:
                renderer.copy(blackChessmanTexture, nullopt, optional(Rect { X0 + x * GRID_W, Y0 + y * GRID_H, CHESSMAN_W, CHESSMAN_H }));
                break;
            case Player::White:
                renderer.copy(whiteChessmanTexture, nullopt, optional(Rect { X0 + x * GRID_W, Y0 + y * GRID_H, CHESSMAN_W, CHESSMAN_H }));
                break;
            default:
                break;
            }
        }
    }

    renderer.present();
}
