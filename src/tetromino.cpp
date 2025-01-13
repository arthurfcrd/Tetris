#include "tetromino.hpp"
#include "grid.hpp"
#include <random>
#include <iostream>

std::mt19937 rng(std::random_device{}());

Tetromino::Tetromino(TetrominoType tetroType){ // TODO : change pos to be not hardcoded
        std::cout << "Creating Tetromino of type " << static_cast<int>(tetroType) << std::endl;
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
                blocks[2] = {1, -1};
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

Tetromino::Tetromino() : Tetromino(static_cast<TetrominoType>(std::uniform_int_distribution<int>(0, 6)(rng))){}

void Tetromino::move(const Grid& g, int dx, int dy){
    for (const auto& point : blocks){
        Point dstTile = point + pos + Point{dx, dy};
        if (!g.isInbounds(dstTile) || !g.isUnoccupied(dstTile)){
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
        int newX = -direction * blocks[i].y;
        int newY = direction * blocks[i].x;
        newblocks[i] = {newX, newY};
    }
    if (type == TetrominoType::I){
        applyOffsetI(rotationIndex, newRotationIndex);
    }
    for (int i = 0; i < 4; i++){
        Point dstTilei = newblocks[i] + pos;
        if (!g.isInbounds(dstTilei) || !g.isUnoccupied(dstTilei)){
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

// true if the tetromino has reached the bottom or another block
bool Tetromino::checkCollision(const Grid& g) const{ 
    for (const auto& point : blocks){
        Point downTile = point + pos + Point{0, 1};
        if (!g.isInbounds(downTile) || !g.isUnoccupied(downTile)){ // if the block is at the bottom or another block is below
            return true;
        }
    }
    return false;
}

void Tetromino::drawTetromino(SDL_Renderer* renderer) const{
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (const auto& block : blocks){
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