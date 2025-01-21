#pragma once
#include <chrono>
#include "grid.hpp"
#include "tetromino.hpp"
#include "keyboard.hpp"

const double FALL_RATE = 0.7; // seconds between falls
const double MOVE_RATE = 0.05; // seconds between moves
const double ROTATE_RATE = 0.15; // seconds between rotations

class Game{
private:
    Grid grid;
    Tetromino currentTetromino;
    int score;
    bool running;
    bool gameOver;
    std::chrono::time_point<std::chrono::system_clock> lastFallTime;
    std::chrono::time_point<std::chrono::system_clock> lastMoveTime;
    std::chrono::time_point<std::chrono::system_clock> lastRotationTime;
    KeyboardHandler keyboardHandler;
public:
    explicit Game() : grid(), currentTetromino(), score(0), 
                    running(true), gameOver(false),
                    lastFallTime(std::chrono::system_clock::now()), 
                    lastMoveTime(std::chrono::system_clock::now()), 
                    lastRotationTime(std::chrono::system_clock::now()), 
                    keyboardHandler(){}
    void update(const SDL_Event& event);
    void update();
    void updateHandler(const SDL_Event& event);
    void draw_hud(SDL_Renderer* renderer) const;
    void draw(SDL_Renderer* renderer) const;
    bool isRunning() const;
    void setRunning(bool);
};