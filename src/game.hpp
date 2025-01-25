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

class Game{
private:
    HUD hud;
    Grid grid;
    TetrominoBag tetroBag;
    bool running;
    bool gameOver;
    // after holding a tetromino the player must place the next tetromino before holding again
    bool canHold_ = true; 
    std::chrono::time_point<std::chrono::system_clock> lastFallTime;
    std::chrono::time_point<std::chrono::system_clock> lastHorizontalMoveTime;
    std::chrono::time_point<std::chrono::system_clock> lastVerticalMoveTime;
    std::chrono::time_point<std::chrono::system_clock> touchedGroundTime;
    KeyboardHandler keyboardHandler;
public:
    explicit Game(GameType gt, int nltc, int ttc) : 
            hud(gt, nltc, ttc), grid(), tetroBag(), 
            running(true), gameOver(false),
            lastFallTime(std::chrono::system_clock::now()), 
            lastHorizontalMoveTime(std::chrono::system_clock::now()), 
            lastVerticalMoveTime(std::chrono::system_clock::now()), 
            keyboardHandler() {}
    // default gamemode
    explicit Game() : Game(GameType::LINES_BASED, 10, 0) {}
    void update(const SDL_Event& event);
    void update();
    void updateHandler(const SDL_Event& event);
    void draw(SDL_Renderer* renderer);
    bool hasWon() const;

    bool isRunning() const;
    void setRunning(bool);
    void setHoldLock();
    void releaseHoldLock();
    bool canHold() const;

};