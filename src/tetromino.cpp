#include "tetromino.hpp"
#include "grid.hpp"

Tetromino::Tetromino(TetrominoType tetroType){ // TODO : change pos to be not hardcoded
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

void Tetromino::move(const Grid& g, int dx, int dy){
    for (auto point : tiles){
        if (isValidTile({pos.x + point.x + dx, pos.y + point.y + dy}, g) == false){
            return;
        }
    }
    pos.x += dx;
    pos.y += dy;
}

void Tetromino::rotate(const Grid& g, int direction){ // direction = 1 for clockwise, -1 for counterclockwise
    Point newTiles[4];
    for (int i = 0; i < 4; i++){
        Point point = tiles[i];
        int newX = -direction * point.y;
        int newY = direction * point.x;
        if (isValidTile({pos.x + newX, pos.y + newY}, g) == false){
            return;
        }
        newTiles[i] = {newX, newY};
    }
    for (int i = 0; i < 4; i++){
        tiles[i] = newTiles[i];
    }
}