#include "tetromino.hpp"
#include "grid.hpp"

Tetromino::Tetromino(TetrominoType tetroType){ // TODO : change pos to be not hardcoded
        type = tetroType;
        pos = {5, 1};
        switch(tetroType){
            case TetrominoType::I:
                blocks[0] = {-1, 0};
                blocks[1] = {0, 0};
                blocks[2] = {1, 0};
                blocks[3] = {2, 0};
                color = Color::CYAN;
                pos.y = 0;
                break;
            case TetrominoType::O:
                blocks[0] = {0, -1};
                blocks[1] = {0, 0};
                blocks[2] = {-1, 1};
                blocks[3] = {1, 0};
                color = Color::YELLOW;
                break;
            case TetrominoType::T:
                blocks[0] = {-1, 0};
                blocks[1] = {0, 0};
                blocks[2] = {0, -1};
                blocks[3] = {1, 0};
                color = Color::MAGENTA;
                break;
            case TetrominoType::S:
                blocks[0] = {-1, 0};
                blocks[1] = {0, 0};
                blocks[2] = {0, -1};
                blocks[3] = {1, -1};
                color = Color::GREEN;
                break;
            case TetrominoType::Z:
                blocks[0] = {-1, -1};
                blocks[1] = {0, -1};
                blocks[2] = {0, 0};
                blocks[3] = {1, 0};
                color = Color::RED;
                break;
            case TetrominoType::J:
                blocks[0] = {-1, -1};
                blocks[1] = {-1, 0};
                blocks[2] = {0, 0};
                blocks[3] = {1, 0};
                color = Color::BLUE;
                break;
            case TetrominoType::L:
                blocks[0] = {-1, 0};
                blocks[1] = {0, 0};
                blocks[2] = {1, 0};
                blocks[3] = {1, -1};
                color = Color::ORANGE;
                break;
        }

    }

void Tetromino::move(const Grid& g, int dx, int dy){
    for (auto point : blocks){
        if (g.isValidTile({pos.x + point.x + dx, pos.y + point.y + dy}) == false){
            return;
        }
    }
    pos.x += dx;
    pos.y += dy;
}

void Tetromino::rotate(const Grid& g, int direction){ // direction = 1 for clockwise, -1 for counterclockwise
    if (type == TetrominoType::O){
        return;
    }
    Point newblocks[4];
    int newRotationIndex = (rotationIndex + direction + 4) % 4;
    for (int i = 0; i < 4; i++){
        Point block = blocks[i];
        int newX = -direction * block.y;
        int newY = direction * block.x;
        newblocks[i] = {newX, newY};
    }
    if (type == TetrominoType::I){
        applyOffsetI(rotationIndex, newRotationIndex);
    }
    for (int i = 0; i < 4; i++){
        if (g.isValidTile({pos.x + newblocks[i].x, pos.y + newblocks[i].y}) == false){
            if (type == TetrominoType::I){
                applyOffsetI(rotationIndex, newRotationIndex);
            }
            return;
        }
    }
    for (int i = 0; i < 4; i++){
        blocks[i] = newblocks[i];
    }
    rotationIndex = newRotationIndex;
}

void Tetromino::applyOffsetI(int rotationIndex, int newRotationIndex){
    Point offsetTable[4] = {{0, 0}, {1, 0}, {1, 1}, {0, 1}};
    Point offset = offsetTable[newRotationIndex] - offsetTable[rotationIndex];
    pos = pos + offset;
}

void Tetromino::drawTetromino(SDL_Renderer* renderer) const{
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (auto block : blocks){
        rect.x = (pos.x + block.x) * rect.w;
        rect.y = (pos.y + block.y) * rect.h;
        drawSquare(renderer, rect, color);
    }
    // drawCenter(renderer); // for debugging 
}

void Tetromino::drawCenter(SDL_Renderer* renderer) const{
    SDL_Rect rect = {pos.x * TILE_SIZE + TILE_SIZE * 3 / 8, pos.y * TILE_SIZE + TILE_SIZE * 3 / 8, TILE_SIZE / 4, TILE_SIZE / 4};
    drawSquare(renderer, rect, Color::NONE);
}