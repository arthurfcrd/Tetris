#pragma once
#include <chrono>
#include "grid.hpp"
#include "tetromino.hpp"
#include "keyboard.hpp"
#include "graphics.hpp"
#include "gui.hpp"

const double FALL_RATE = 0.7; // seconds between falls
const double VERTICAL_MOVE_RATE = 0.03; // seconds between moves
const double HORIZONTAL_MOVE_RATE = 0.08; // seconds between moves
const double ROTATE_RATE = 0.12; // seconds between rotations

const double LOCK_DELAY = 0.5; // seconds before a locking a tetromino that has touched the ground

class BaseGame{
protected:
    Grid grid;
    bool running;
    TetrominoBag tetroBag;
public:

    BaseGame(bool isRunning) : grid(), running(isRunning), tetroBag() {}
    BaseGame() : BaseGame(true) {}
    bool isRunning() const;
    friend class OnlineGame;
};

class Game : public BaseGame {
private:
    HUD hud;
    int garbageToSend; // only used online
    friend class OnlineGame;
protected:
    bool gameOver;
    // after holding a tetromino the player must place the next tetromino before holding again
    bool canHold_ = true; 
    std::chrono::time_point<std::chrono::system_clock> lastFallTime;
    std::chrono::time_point<std::chrono::system_clock> lastHorizontalMoveTime;
    std::chrono::time_point<std::chrono::system_clock> lastVerticalMoveTime;
    std::chrono::time_point<std::chrono::system_clock> touchedGroundTime;
    KeyboardHandler keyboardHandler;

    Mix_Chunk* lineClearedSound;
    Mix_Chunk* hardDropSound;
    Mix_Chunk* holdLockedSound;
    Mix_Chunk* levelUpSound;
public:
    explicit Game(GameType gt, int nltc, int ttc, bool isRunning);
    explicit Game(GameType gt, int nLinesToClear, int timeToClear) : 
        Game(gt, nLinesToClear, timeToClear, true) {}
    // default gamemode
    explicit Game() : Game(GameType::LINES_BASED, 10, 0) {}
    ~Game();

    void update();
    void updateHandler(const SDL_Event& event);
    void updateHandler(Key key);
    void draw(SDL_Renderer* renderer);
    bool hasWon() const;

    void setRunning(bool);
    void setHoldLock();
    void releaseHoldLock();
    bool canHold() const;
};