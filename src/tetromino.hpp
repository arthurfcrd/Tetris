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

struct Point{
    int x;
    int y;
};

class Grid; // forward declaration

class Tetromino{
private:
    TetrominoType type;
    Point pos; // x, y position of the center of the tetromino 
    Point tiles[4]; // position of the 4 blocks of the tetromino relative to the center
    Color color;
public:
    explicit Tetromino(TetrominoType tetroType){ // TODO : change pos to be not hardcoded
        type = tetroType;
        pos = {5, 1};
        switch(tetroType){
            case TetrominoType::I:
                tiles[0] = {-2, -1};
                tiles[1] = {-1, -1};
                tiles[2] = {1, -1};
                tiles[3] = {2, -1};
                color = Color::CYAN;
                break;
            case TetrominoType::O:
                tiles[0] = {-1, 1};
                tiles[1] = {1, -1};
                tiles[2] = {-1, -1};
                tiles[3] = {1, 1};
                color = Color::YELLOW;
                break;
            case TetrominoType::T:
                tiles[0] = {-1, 0};
                tiles[1] = {0, 0};
                tiles[2] = {0, -1};
                tiles[3] = {1, 0};
                color = Color::MAGENTA;
                break;
            case TetrominoType::S:
                tiles[0] = {-1, 0};
                tiles[1] = {0, 0};
                tiles[2] = {0, -1};
                tiles[3] = {1, -1};
                color = Color::GREEN;
                break;
            case TetrominoType::Z:
                tiles[0] = {-1, -1};
                tiles[1] = {0, -1};
                tiles[2] = {0, 0};
                tiles[3] = {1, 0};
                color = Color::RED;
                break;
            case TetrominoType::J:
                tiles[0] = {-1, -1};
                tiles[1] = {-1, 0};
                tiles[2] = {0, 0};
                tiles[3] = {1, 0};
                color = Color::BLUE;
                break;
            case TetrominoType::L:
                tiles[0] = {-1, 0};
                tiles[1] = {0, 0};
                tiles[2] = {1, 0};
                tiles[3] = {1, -1};
                color = Color::ORANGE;
                break;
        }

    }
    void move(Grid grid, int dx, int dy);
    void rotate(Grid grid, int direction); // direction = 1 for clockwise, -1 for counterclockwise
};