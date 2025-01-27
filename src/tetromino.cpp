#include "tetromino.hpp"
#include "grid.hpp"
#include <random>
#include <cassert>
#include <sstream>

static const Point wallkicksJLSTZ[5][4] = { // wallkick offsets for J, L, S, T and Z
    {{0, 0}, {0, 0}, {0, 0}, {0, 0}},
    {{0, 0}, {-1, 0}, {0, 0}, {1, 0}},
    {{0, 0}, {-1, -1}, {0, 0}, {1, -1}},
    {{0, 0}, {0, 2}, {0, 0}, {0, 2}},
    {{0, 0}, {-1, 2}, {0, 0}, {1, 2}}
};

static const Point wallkicksI[5][4] = { // wallkick offsets for I
    {{0, 0}, {1, 0}, {1, 1}, {0, 1}},
    {{1, 0}, {0, 0}, {-1, 1}, {0, 1}},
    {{-2, 0}, {0, 0}, {2, 1}, {0, 1}},
    {{1, 0}, {0, 1}, {-1, 0}, {0, -1}},
    {{-2, 0}, {0, -2}, {2, 0}, {0, 2}}
};

static std::mt19937 rng(std::random_device{}());


/* Tetromino getters and setters */
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



/* Tetromino other methods*/
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



/* TetrominoBag methods */


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
    currentTetromino = BaseTetromino(tetroList.back());
    tetroList.pop_back();
}

void TetrominoBag::pickNextTetromino() {
    if (tetroList.empty()) {
        createBag();
    }
    nextTetromino = BaseTetromino(tetroList.back());
    tetroList.pop_back();
}

void TetrominoBag::switchTetromino() {
    // delete currentTetromino;
    currentTetromino = nextTetromino;
    pickNextTetromino();
}

void TetrominoBag::hold() {
    if (heldTetromino.getType() == TetrominoType::NONE) {
        heldTetromino = BaseTetromino(currentTetromino.getType());
        switchTetromino();
    } else if (heldTetromino.getType() != currentTetromino.getType()){
        // do not switch if current tetromino is the same as the held one
        TetrominoType heldType = heldTetromino.getType();
        heldTetromino = BaseTetromino(currentTetromino.getType());
        currentTetromino = BaseTetromino(heldType);
    }
}

// BaseTetromino methods

BaseTetromino::BaseTetromino(TetrominoType tetroType) : // TODO : change pos to be not hardcoded if the size of the grid changes
    type(tetroType), pos({5, 1}){
    color = Color::NONE;
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
        case TetrominoType::NONE:
            break;
        default:
            assert(false); // should never happen
            break;
        }
}

BaseTetromino::BaseTetromino() : BaseTetromino(static_cast<TetrominoType>(std::uniform_int_distribution<int>(0, 6)(rng))){}

BaseTetromino::BaseTetromino(const BaseTetromino& other) {
    type = other.type;
    pos.x = other.pos.x; 
    pos.y = other.pos.y;
    for (int i = 0; i < 4; i++)
        blocks[i] = other.blocks[i];
    color = other.color;
}

TetrominoType BaseTetromino::getType() const {
    return type;
}

void BaseTetromino::drawTetromino(SDL_Renderer* renderer) const{
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (const auto& block : blocks){
        rect.x = PANE_SIZE + (pos.x + block.x) * rect.w;
        rect.y = (pos.y + block.y) * rect.h;
        drawSquare(renderer, rect, color);
    }
}

void BaseTetromino::drawTetromino(SDL_Renderer* renderer, int startX, int startY) const{
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for (const auto& block : blocks){
        rect.x = startX + (pos.x + block.x) * rect.w;
        rect.y = startY + (pos.y + block.y) * rect.h;
        drawSquare(renderer, rect, color);
    }
}

std::string BaseTetromino::serialize() const {
    // serialize the type, pos, blocks and color 
    std::string serializedTetromino = "";
    serializedTetromino += std::to_string(static_cast<int>(type));
    serializedTetromino += " " + std::to_string(pos.x) + " " + std::to_string(pos.y);
    for (int i = 0; i < 4; i++){
        serializedTetromino += " " + std::to_string(blocks[i].x) + " " + std::to_string(blocks[i].y);
    }
    serializedTetromino += " " + std::to_string(static_cast<int>(color));
    return serializedTetromino;
}

BaseTetromino::BaseTetromino(std::string serializedTetromino) { 
    // Tetrominoes reconstructed this way will only be displayed and not interacted with
    // so rotationIndex, touchedGround and locked are not set
    std::istringstream iss(serializedTetromino);
    // set type
    int tmp;
    iss >> tmp;
    type = static_cast<TetrominoType>(tmp);
    iss >> pos.x >> pos.y;
    for (int j = 0; j < 4; j++){
        iss >> blocks[j].x >> blocks[j].y;
    }
    iss >> tmp;
    color = static_cast<Color>(tmp);
}