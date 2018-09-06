
#ifndef TWOKILLONE_GAME_H
#define TWOKILLONE_GAME_H

#ifdef EMSCRIPTEN
#include <emscripten.h>
#include <EventDispatcher.h>

#endif

#include "sdl2pp.h"
#include "Board.h"

using namespace sdl2pp;


class Game {
public:
    static const int WINDOW_WIDTH = 306;
    static const int WINDOW_HEIGHT = 306;

public:
    Game();

public:
    /**
     * run one iteration of the game loop
     */
    void runOnce() {
        update();
        handleEvents();
        render();
    }

    /**
     * @return true if succeed, false if the event was filtered, throw an sdl::Error if there was some other error
     */
    bool fireUserEvent(int code, int data);

private:
    void initEvents();

    void handleEvents() {
        while (eventDispatcher.poll()) {
        }
    }

    void handleUserEvents(const SDL_UserEvent &);

    /**
     * game logic
     */
    void update();

    /**
     * render game graphics to screen
     */
    void render();

private:
    /////////////////////////       game functions

    void newGame();

    void takeTurn();

private:
    enum class PlayerType {
        Human, AI
    };

    enum class PlayerStatus {
        None,
        CanMove, // human player can move chessman
        IsMoving, // human player is moving chessman
        MoveAnimation, // playing chessman move animation
    };

private:
    EventDispatcher eventDispatcher;
    Window window;
    Renderer renderer;

    Texture whiteChessmanTexture;
    Texture blackChessmanTexture;
    Texture boardTexture;

    Board board;
    PlayerType black;
    PlayerType white;

    Player curPlayer;
    PlayerStatus playerStatus;
};


#endif
