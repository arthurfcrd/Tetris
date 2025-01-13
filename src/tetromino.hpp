#pragma once
#include "graphics.hpp"
#include "grid.hpp"

enum class TetrominoType : int {
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

class Tetromino{
private:
    TetrominoType type;
    Point pos; // x, y position of the center of the tetromino (in grid coordinates)
    Point blocks[4]; // position of the 4 blocks of the tetromino relative to the center
    Color color;
    int rotationIndex = 0;
public:
    explicit Tetromino(TetrominoType type);
    explicit Tetromino(); // creates a random tetromino
    void move(const Grid&, int dx, int dy);
    void rotate(const Grid&, int direction); // direction = 1 for clockwise, -1 for counterclockwise
    void applyOffsetI(int rotationIndex, int newRotationIndex);
    bool checkCollision(const Grid& g) const;
    void drawTetromino(SDL_Renderer* renderer) const;
    void drawCenter(SDL_Renderer* renderer) const;
    friend void Grid::insertTetromino(const Tetromino& tetromino);
};
