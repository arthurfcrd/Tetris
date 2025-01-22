#pragma once
#include <chrono>
#include "grid.hpp"
#include "tetromino.hpp"
#include "keyboard.hpp"
#include "graphics.hpp"

const double FALL_RATE = 0.7; // seconds between falls
const double MOVE_RATE = 0.05; // seconds between moves
const double ROTATE_RATE = 0.12; // seconds between rotations

const double LOCK_DELAY = 1.2; // seconds before a locking a tetromino that has touched the ground

class Game{
private:
    Grid grid;
    HUD hud;
    TetrominoBag tetroBag;
    bool running;
    bool gameOver;
    std::chrono::time_point<std::chrono::system_clock> lastFallTime;
    std::chrono::time_point<std::chrono::system_clock> lastMoveTime;
    std::chrono::time_point<std::chrono::system_clock> lastRotationTime;
    std::chrono::time_point<std::chrono::system_clock> touchedGroundTime;
    KeyboardHandler keyboardHandler;
public:
    explicit Game() : grid(), hud(), tetroBag(), 
                    running(true), gameOver(false),
                    lastFallTime(std::chrono::system_clock::now()), 
                    lastMoveTime(std::chrono::system_clock::now()), 
                    lastRotationTime(std::chrono::system_clock::now()), 
                    keyboardHandler(){}
    void update(const SDL_Event& event);
    void update();
    void updateHandler(const SDL_Event& event);
    void draw(SDL_Renderer* renderer);
    bool isRunning() const;
    void setRunning(bool);
};