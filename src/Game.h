
#ifndef TWOKILLONE_GAME_H
#define TWOKILLONE_GAME_H

#ifdef EMSCRIPTEN
#include <emscripten.h>
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
    ~Game();

public:
    /**
     * run one iteration of the game loop
     */
    void runOnce() {
        update();
        handleEvents();
        if (isDirty) {
            render();
            isDirty = false;
        }
    }

    /**
     * @return true if succeed, false if the event was filtered, throw an sdl::Error if there was some other error
     */
    static bool fireUserEvent(int code, int data);

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

    void startMoveAnimation();

    void computerMove();

    static void computerMoveCallback(char *data, int size, void *arg);

    void move(); // move the chessman at playerOrgPos to playerCurPos

    bool checkOver();

private:
    enum class PlayerType {
        Human, Computer
    };

    enum class PlayerStatus {
        None,
        CanMove, // human player can move chessman
        IsMoving, // human player is moving chessman
        MoveAnimation, // playing chessman move animation
    };

    // If the coordinate is not in the area of any board grids, return nullopt
    static optional<Board::Position> getBoardPos(int coordX, int coordY);

    static const int CHESSMAN_OFFSET_X = 15;
    static const int CHESSMAN_OFFSET_Y = 15;
    static const int GRID_OFFSET_X = 12;
    static const int GRID_OFFSET_Y = 12;
    static const int GRID_W = 71;
    static const int GRID_H = 71;
    static const int CHESSMAN_W = 70;
    static const int CHESSMAN_H = 70;
    static const int GRID_INNER_W = 69;
    static const int GRID_INNER_H = 69;
    static const int CHESSMAN_ACTUAL_W = 60; // not including the shadow
    static const int CHESSMAN_ACTUAL_H = 60;

    static const int MOVE_ANIMATION_DURATION = 300; // ms

    static const int AI_TIME_LIMIT = 1000; // ms

#ifndef EMSCRIPTEN
public:
    bool quit;
#endif

private:
    EventDispatcher eventDispatcher;
    Window window;
    Renderer renderer;

    Texture whiteChessmanTexture;
    Texture blackChessmanTexture;
    Texture boardTexture;

    bool isDirty; // if need rendering

    Board board;
    PlayerType black;
    PlayerType white;

    Player curPlayer;
    PlayerStatus playerStatus;

    // The following 3 fields are used for tracking the moving chessman
    Board::Position playerOrgPos; // the original board position of the moving chessman
    Board::Position playerCurPos; // the current board position of the moving chessman, if not available, the x component is -1
    Point playerCurCoord; // the canvas coordinate of the moving chessman

    double moveAnimationDelta; // 0 start, 1 end
    double moveAnimationTime; // emscripten_get_now()

    bool isMouse;
    SDL_FingerID fingerId;

    worker_handle workerHandle;
};


#endif
