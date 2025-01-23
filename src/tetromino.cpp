#include "tetromino.hpp"
#include "grid.hpp"
#include <random>
#include <cassert>

const Point wallkicksJLSTZ[5][4] = {
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 0}, {-1, 0}, {0, 0}, {1, 0}},
    {{0, 0}, {-1, -1}, {0, 0}, {1, -1}},
    {{0, 0}, {0, 2}, {0, 0}, {0, 2}},
    {{0, 0}, {-1, 2}, {0, 0}, {1, 2}}
};

const Point wallkicksI[5][4] = {
    {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    {{1, 0}, {0, 0}, {-1, 1}, {0, 1}},
    {{-2, 0}, {0, 0}, {2, 1}, {0, 1}},
    {{1, 0}, {0, 1}, {-1, 0}, {0, -1}},
    {{-2, 0}, {0, -2}, {2, 0}, {0, 2}}
};

std::mt19937 rng(std::random_device{}());

TetrominoType Tetromino::getType() const {
    return type;
}

int Tetromino::getPosX() const {
    return pos.x;
}

int Tetromino::getPosY() const {
    return pos.y;
}

void Tetromino::setPos(int x, int y) {
        pos.x = x;
        pos.y = y;
}

bool Tetromino::isLocked() const {
    return locked;
}

bool Tetromino::hasTouchedGround() const {
    return touchedGround;
}

void Tetromino::setLocked(bool newVal) {
    locked = newVal;
}

void Tetromino::setTouchedGround(bool newVal) {
    touchedGround = newVal;
}

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

Tetromino::Tetromino(const Tetromino& other) {
    type = other.type;
    pos.x = other.pos.x; 
    pos.y = other.pos.y;
    for (int i = 0; i < 4; i++)
        blocks[i] = other.blocks[i];
    color = other.color;
    rotationIndex = other.rotationIndex;
}

void Tetromino::move(const Grid& g, int dx, int dy){
    for (const auto& point : blocks){
        Point dstTile = pos + point + Point{dx, dy};
        if (!g.isInbounds(dstTile) || !g.isUnoccupied(dstTile)){
            return;
        }
    }
    pos.x += dx;
    pos.y += dy;
}

void Tetromino::rotate(const Grid& g, int direction){ // direction = 1 for clockwise, -1 for counterclockwise
    if (this->type == TetrominoType::O){
        return;
    }
    int newRotationIndex = (this->rotationIndex + direction + 4) % 4;
    Point newblocks[4];
    for (int i = 0; i < 4; i++){
        int newX = -direction * blocks[i].y;
        int newY = direction * blocks[i].x;
        newblocks[i] = {newX, newY};
    }
    for (int offsetIndex = 0; offsetIndex < 5; offsetIndex++){
        Point newPos = getOffsetPos(offsetIndex, this->type, this->rotationIndex, newRotationIndex);
        int i;
        for (i = 0; i < 4; i++){
            Point dstTilei = newblocks[i] + newPos;
            if (!g.isInbounds(dstTilei) || !g.isUnoccupied(dstTilei)){
                break;
            }
        }
        if (i == 4){
            for (int k = 0; k < 4; k++){
                this->blocks[k] = newblocks[k];
            }
            this->pos = newPos;
            this->rotationIndex = newRotationIndex;
            break;
        }
    }
}

Point Tetromino::getOffsetPos(int offsetIndex, TetrominoType tetroType, int rotationIndex, int newRotationIndex){
    assert(tetroType != TetrominoType::O);
    assert(offsetIndex >= 0 && offsetIndex < 5);
    assert(rotationIndex >= 0 && rotationIndex < 4);
    const Point* offsetTable = (tetroType == TetrominoType::I) ? wallkicksI[offsetIndex] : wallkicksJLSTZ[offsetIndex];
    Point offset = offsetTable[newRotationIndex] - offsetTable[rotationIndex];
    return this->pos + offset;
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
        rect.x = PANE_SIZE + (pos.x + block.x) * rect.w;
        rect.y = (pos.y + block.y) * rect.h;
        drawSquare(renderer, rect, color);
    }
    //drawCenter(renderer); // for debugging 
}

void Tetromino::drawCenter(SDL_Renderer* renderer) const{
    SDL_Rect rect = {PANE_SIZE + pos.x * TILE_SIZE + TILE_SIZE * 3 / 8, pos.y * TILE_SIZE + TILE_SIZE * 3 / 8, TILE_SIZE / 4, TILE_SIZE / 4};
    drawSquare(renderer, rect, Color::NONE);
}

void TetrominoBag::createBag() {
    tetroList.clear();
    tetroList.push_back(TetrominoType::I);
    tetroList.push_back(TetrominoType::O);
    tetroList.push_back(TetrominoType::T);
    tetroList.push_back(TetrominoType::S);
    tetroList.push_back(TetrominoType::Z);
    tetroList.push_back(TetrominoType::J);
    tetroList.push_back(TetrominoType::L);

    std::shuffle(tetroList.begin(), tetroList.end(), rng);
}

void TetrominoBag::pickTetromino() {
    // fill up the bag again if it is empty
    if (tetroList.empty()) {
        createBag();
    }
    currentTetromino = Tetromino(tetroList.back());
    tetroList.pop_back();
}

void TetrominoBag::pickNextTetromino() {
    if (tetroList.empty()) {
        createBag();
    }
    nextTetromino = Tetromino(tetroList.back());
    tetroList.pop_back();
}

void TetrominoBag::switchTetromino() {
    // delete currentTetromino;
    currentTetromino = nextTetromino;
    pickNextTetromino();
}