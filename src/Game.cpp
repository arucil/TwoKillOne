#include <cstdlib>
#include <stdexcept>
#include "events.h"
#include "AI_worker.h"
#include "Game.h"

using std::abs;
using namespace std::experimental;


Game::Game() :
        window("二打一棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN),
        renderer(window, -1, SDL_RENDERER_ACCELERATED),
        whiteChessmanTexture(renderer, "assets/white.png"),
        blackChessmanTexture(renderer, "assets/black.png"),
        boardTexture(renderer, "assets/board.png"),
        playerStatus(PlayerStatus::None),
#ifndef EMSCRIPTEN
        quit(false),
#endif
        isDirty(true) {
    initEvents();

    workerHandle = emscripten_create_worker("worker.js");
}

Game::~Game() {
    emscripten_destroy_worker(workerHandle);
}

void Game::initEvents() {
    eventDispatcher.quitEvent = [this](const auto &) {
#ifdef EMSCRIPTEN
        emscripten_cancel_main_loop();
#else
        quit = true;
#endif
    };

    eventDispatcher.userEvent = [this](const auto &e) {
        handleUserEvents(e);
    };

    eventDispatcher.mouseButtonDown = [this](const SDL_MouseButtonEvent &e) {
        if (e.button == SDL_BUTTON_LEFT) {
            if (playerStatus == PlayerStatus::CanMove) {
                auto pos = getBoardPos(e.x, e.y);

                if (pos && board(*pos) == curPlayer) {
                    playerOrgPos = *pos;
                    playerCurCoord.x = e.x;
                    playerCurCoord.y = e.y;
                    playerStatus = PlayerStatus::IsMoving;
                    isMouse = true;
                    isDirty = true;
                }
            } else if (playerStatus == PlayerStatus::IsMoving && playerCurPos.x >= 0) {
                move();
            }
        } else if (e.button == SDL_BUTTON_RIGHT && playerStatus == PlayerStatus::IsMoving) {
            playerStatus = PlayerStatus::CanMove;
            isDirty = true;
        }
    };

    eventDispatcher.mouseMove = [this](const SDL_MouseMotionEvent &e) {
        if (playerStatus == PlayerStatus::IsMoving) {
            auto pos = getBoardPos(e.x, e.y);

            if (pos && abs(pos->x - playerOrgPos.x) + abs(pos->y - playerOrgPos.y) == 1
                    && board(*pos) == Player::None) {
                playerCurPos = *pos;
            } else {
                playerCurPos.x = -1;
            }

            playerCurCoord.x = e.x;
            playerCurCoord.y = e.y;
            isDirty = true;
        }
    };

    ////// Move chessman with touch screen

    eventDispatcher.fingerDown = [this](const SDL_TouchFingerEvent &e) {
        if (PlayerStatus::CanMove == playerStatus) {
            auto pos = getBoardPos(static_cast<int>(e.x * WINDOW_WIDTH), static_cast<int>(e.y * WINDOW_HEIGHT));

            if (pos && curPlayer == board(*pos)) {
                playerOrgPos = *pos;
                playerStatus = PlayerStatus::IsMoving;
                fingerId = e.fingerId;
                isMouse = false;
                isDirty = true;
            }
        }
    };

    eventDispatcher.fingerUp = [this](const SDL_TouchFingerEvent &e) {
        if (PlayerStatus::IsMoving == playerStatus && e.fingerId == fingerId) {
            auto pos = getBoardPos(static_cast<int>(e.x * WINDOW_WIDTH), static_cast<int>(e.y * WINDOW_HEIGHT));

            if (pos &&
                    ((pos->x == playerOrgPos.x && pos->y != playerOrgPos.y)
                    || (pos->y == playerOrgPos.y && pos->x != playerOrgPos.x))) {
                if (pos->x == playerOrgPos.x) {
                    pos->y = playerOrgPos.y + (pos->y > playerOrgPos.y ? 1 : -1);
                } else if (pos->y == playerOrgPos.y) {
                    pos->x = playerOrgPos.x + (pos->x > playerOrgPos.x ? 1 : -1);
                }

                if (Player::None == board(*pos)) {
                    playerCurPos = *pos;
                    startMoveAnimation();
                } else {
                    playerStatus = PlayerStatus::CanMove;
                }
            } else {
                playerStatus = PlayerStatus::CanMove;
            }

            isDirty = true;
        }
    };
}

void Game::move() {
    board(playerOrgPos) = Player::None;
    board(playerCurPos) = curPlayer;
    isDirty = true;

    board.move(playerOrgPos, playerCurPos, curPlayer);

    if (!checkOver()) {
        curPlayer = nextPlayer(curPlayer);
        takeTurn();
    }
}

optional<Board::Position> Game::getBoardPos(int coordX, int coordY) {
    int xx = coordX - CHESSMAN_OFFSET_X;
    int yy = coordY - CHESSMAN_OFFSET_Y;
    int gridx = xx / GRID_W;
    int gridy = yy / GRID_H;

    if (xx >= 0 && yy >= 0 && gridx < Board::SIZE && gridy < Board::SIZE) {
        return make_optional(Board::Position { gridx, gridy });
    } else {
        return nullopt;
    }
}

bool Game::fireUserEvent(int code, int data) {
    SDL_Event event;
    event.type = SDL_USEREVENT;
    event.user.code = code;
    event.user.data1 = reinterpret_cast<void *>(data);

    return Event::pushEvent(event);
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
            black = PlayerType::Computer; break;
        default:
            throw std::logic_error("unreachable");
        }
        break;
    case SetWhite:
        switch (reinterpret_cast<uintptr_t>(e.data1)) {
        case 0:
            white = PlayerType::Human; break;
        case 1:
            white = PlayerType::Computer; break;
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
    isDirty = true;
}

void Game::takeTurn() {
    if (curPlayer == Player::Black) {
        if (black == PlayerType::Human) {
#ifdef EMSCRIPTEN
            EM_ASM(
                showMessage("黑棋移动");
            );
#endif
            playerStatus = PlayerStatus::CanMove;
        } else {
            playerStatus = PlayerStatus::None;
#ifdef EMSCRIPTEN
            EM_ASM(
                showMessage("电脑（黑棋）思考中...");
            );
#endif
            computerMove();
        }
    } else {
        if (white == PlayerType::Human) {
#ifdef EMSCRIPTEN
            EM_ASM(
                showMessage("白棋移动");
            );
#endif
            playerStatus = PlayerStatus::CanMove;
        } else {
            playerStatus = PlayerStatus::None;
#ifdef EMSCRIPTEN
            EM_ASM(
                showMessage("电脑（白棋）思考中...");
            );
#endif
            computerMove();
        }
    }
}

void Game::startMoveAnimation() {
    playerStatus = PlayerStatus::MoveAnimation;
    moveAnimationDelta = 0;
    moveAnimationTime = emscripten_get_now();
}

void Game::computerMoveCallback(char *data, int size, void *arg) {
    auto response = reinterpret_cast<ResponseData *>(data);
    Game *game = reinterpret_cast<Game *>(arg);

    game->playerOrgPos = response->srcPos;
    game->playerCurPos = response->destPos;

#ifdef EMSCRIPTEN
    if (Player::Black == game->curPlayer) {
        EM_ASM(
                showMessage("黑棋移动...");
        );
    } else {
        EM_ASM(
                showMessage("白棋移动...");
        );
    }
#endif

    game->startMoveAnimation();
}

void Game::computerMove() {
    RequestData request { board, curPlayer, AI_TIME_LIMIT };

    emscripten_call_worker(workerHandle, "runAI", reinterpret_cast<char *>(&request), sizeof request, computerMoveCallback, this);
}

bool Game::checkOver() {
    if (board.isWinning(curPlayer)) {
#ifdef EMSCRIPTEN
        EM_ASM_({
            receiveEvent($0);
            showMessage(UTF8ToString($1) + "方胜利");
        }, GameOver, curPlayer == Player::Black ? "黑" : "白");
#endif
        playerStatus = PlayerStatus::None;
        return true;
    }
    return false;
}

void Game::update() {
    if (PlayerStatus::MoveAnimation == playerStatus) {
        // Play chessman move animation
        auto time1 = emscripten_get_now();
        moveAnimationDelta += (time1 - moveAnimationTime) / MOVE_ANIMATION_DURATION;
        moveAnimationTime = time1;
        isDirty = true;

        if (moveAnimationDelta > 1) {
            move();
        }
    }
}

void Game::render() {
    renderer.setDrawColor(0xff, 0x80, 0x80, 0xff);
    renderer.fill();

    renderer.copy(boardTexture, nullopt, make_optional(Rect { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT }));

    auto blendMode = renderer.getDrawBlendMode();
    renderer.setDrawBlendMode(SDL_BLENDMODE_BLEND);

    for (int y = 0; y < Board::SIZE; ++y) {
        for (int x = 0; x < Board::SIZE; ++x) {
            if (playerStatus == PlayerStatus::MoveAnimation && x == playerOrgPos.x && y == playerOrgPos.y) {
                continue;
            }

            if (playerStatus == PlayerStatus::IsMoving) {
                if (isMouse && x == playerOrgPos.x && y == playerOrgPos.y) {
                    continue;
                }

                if (std::abs(x - playerOrgPos.x) + std::abs(y - playerOrgPos.y) == 1 && board(x, y) == Player::None) {
                    if (playerCurPos.x == x && playerCurPos.y == y) {
                        renderer.setDrawColor(0xff, 0xcc, 0x00, 0xa0);
                    } else {
                        renderer.setDrawColor(0xf7, 0xf7, 0x09, 0xa0);
                    }

                    renderer.fillRect(Rect {
                        GRID_OFFSET_X + x * GRID_W, GRID_OFFSET_Y + y * GRID_H,
                        GRID_INNER_W, GRID_INNER_H
                    });
                }
            }

            switch (board(x, y)) {
            case Player::Black:
                renderer.copy(blackChessmanTexture, nullopt, make_optional(Rect {
                    CHESSMAN_OFFSET_X + x * GRID_W, CHESSMAN_OFFSET_Y + y * GRID_H,
                    CHESSMAN_W, CHESSMAN_H
                }));
                break;
            case Player::White:
                renderer.copy(whiteChessmanTexture, nullopt, make_optional(Rect {
                    CHESSMAN_OFFSET_X + x * GRID_W, CHESSMAN_OFFSET_Y + y * GRID_H,
                    CHESSMAN_W, CHESSMAN_H
                }));
                break;
            default:
                break;
            }
        }
    }

    renderer.setDrawBlendMode(blendMode);

    if (playerStatus == PlayerStatus::IsMoving && isMouse) {
        renderer.copy(Player::Black == curPlayer ? blackChessmanTexture : whiteChessmanTexture, nullopt, make_optional(Rect {
            playerCurCoord.x - CHESSMAN_ACTUAL_W / 2, playerCurCoord.y - CHESSMAN_ACTUAL_H / 2,
            CHESSMAN_W, CHESSMAN_H
        }));
    } else if (playerStatus == PlayerStatus::MoveAnimation) {
        renderer.copy(Player::Black == curPlayer ? blackChessmanTexture : whiteChessmanTexture, nullopt, make_optional(Rect {
            static_cast<int>((playerOrgPos.x + (playerCurPos.x - playerOrgPos.x) * moveAnimationDelta) * GRID_W) + CHESSMAN_OFFSET_X,
            static_cast<int>((playerOrgPos.y + (playerCurPos.y - playerOrgPos.y) * moveAnimationDelta) * GRID_H) + CHESSMAN_OFFSET_Y,
            CHESSMAN_W, CHESSMAN_H
        }));
    }

    renderer.present();
}
