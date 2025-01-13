#pragma once
#include <chrono>
#include "grid.hpp"
#include "tetromino.hpp"
#include "keyboard.hpp"

const double FALL_SPEED = 0.7;

class Game{
private:
    Grid grid;
    Tetromino currentTetromino;
    int score;
    std::chrono::time_point<std::chrono::system_clock> lastMoveTime;
    KeyboardHandler keyboardHandler;
public:
    explicit Game() : grid(), currentTetromino(), score(0), lastMoveTime(std::chrono::system_clock::now()), keyboardHandler(){}
    void update(const SDL_Event& event);
    void update();
    void draw(SDL_Renderer* renderer) const;
};