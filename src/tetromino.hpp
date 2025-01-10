#pragma once
#include "graphics.hpp"

enum class TetrominoType{
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

class Grid; // forward declaration

class Tetromino{
private:
    TetrominoType type;
    Point pos; // x, y position of the center of the tetromino 
    Point blocks[4]; // position of the 4 blocks of the tetromino relative to the center
    Color color;
    int rotationIndex = 0;
public:
    explicit Tetromino(TetrominoType type);
    void move(const Grid&, int dx, int dy);
    void rotate(const Grid&, int direction); // direction = 1 for clockwise, -1 for counterclockwise
    void drawTetromino(SDL_Renderer* renderer) const;
    void drawCenter(SDL_Renderer* renderer) const;
};

void applyOffsetI(Point& pos, int rotationIndex, int newRotationIndex);