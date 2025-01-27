#include "grid.hpp"
#include "tetromino.hpp"
#include "point.hpp"
#include <cassert>

static std::mt19937 rng(std::random_device{}());

int Grid::getWidth() const {
    return width;
}

int Grid::getHeight() const {
    return height;
}

void Grid::clearGrid() {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++)
            matrix[i][j] = Color::NONE;
    }
}

bool Grid::isInbounds(Point point) const{ // checks if a tile is within the grid and is not occupied
    return point.x >= 0 && point.x < width && point.y >= 0 && point.y < height;
}

bool Grid::isUnoccupied(Point point) const{
    // assert the point is within the grid
    assert(isInbounds(point));
    return matrix[point.y][point.x] == Color::NONE;
}

void Grid::insertTetromino(const Tetromino& tetromino){
    for (const auto& block : tetromino.blocks){
        Point tile = block + tetromino.pos;
        matrix[tile.y][tile.x] = tetromino.color;
    }
}

void Grid::drawGrid(SDL_Renderer* renderer) const{
    drawGrid(renderer, PANE_SIZE, 0);
}

void Grid::drawGrid(SDL_Renderer* renderer, int startX, int startY) const {
    /*SDL_Rect wholeGrid = {PANE_SIZE, 0, width*TILE_SIZE, height*TILE_SIZE};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &wholeGrid);

    SDL_SetRenderDrawColor(renderer, 122, 120, 116, 255);
    for (int y = 1; y < height; y++) {
        SDL_RenderDrawLine(renderer, PANE_SIZE, y*TILE_SIZE, PANE_SIZE+width*TILE_SIZE, y*TILE_SIZE);
    }
    for (int x = 1; x < width; x++) {
        SDL_RenderDrawLine(renderer, PANE_SIZE+x*TILE_SIZE, 0, PANE_SIZE+x*TILE_SIZE, height*TILE_SIZE);
    }*/

    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            rect.x = startX + j * rect.w;
            rect.y = startY + i * rect.h;
            drawSquare(renderer, rect, matrix[i][j]);
        }
    }
}

// move the line lineNum down 1 block
void Grid::moveLineDown(int lineNum) {
    assert(lineNum >= 0 && lineNum < height - 1);

    for (int col = 0; col < width; col++) {
        matrix[lineNum + 1][col] = matrix[lineNum][col];
        matrix[lineNum][col] = Color::NONE;
    }
}

// move every lines above baseLine down 1 block
void Grid::moveLinesDown(int baseLine) {
    for (int line = baseLine - 1; line > 0; line--) {
        moveLineDown(line);
    }
}

bool Grid::moveLineUp(int lineNum){
    if (lineNum == 0) return false;
    assert(lineNum > 0 && lineNum < height);
    for (int col = 0; col < width; col++) {
        matrix[lineNum - 1][col] = matrix[lineNum][col];
        matrix[lineNum][col] = Color::NONE;
    }
    return true;
}

bool Grid::moveLinesUp(int nbOfLines){
    for (int i = nbOfLines; i < height; i++){
        if (!moveLineUp(i)) return false;
    }
    return true;
}

bool Grid::lineIsFull(int lineNum) const {
    for (int col = 0; col < width; col++) {
        if (matrix[lineNum][col] == Color::NONE)
            return false;
    }
    return true;
}

int Grid::clearLines() {
    int nCleared = 0;
    bool done = false;
    while (!done) {
        int nNotFull = 0;
        // check the lines starting from the bottom
        for (int currLine = height - 1; currLine >= 0; currLine--) {
            if (lineIsFull(currLine)) {
                //std::cout << currLine << " must be cleared" << std::endl;
                moveLinesDown(currLine);
                nCleared++;
            } else {
                nNotFull++;
            }
        }

        if (nNotFull == height)
            done = true;
    }
    assert(nCleared < 5);
    return nCleared;
}

bool Grid::addGarbageLines(int nLines){
    // add nLines garbage lines at the bottom of the grid
    // each garbage line will have a hole with a random position
    if (!moveLinesUp(nLines)) return false;
    for (int i = 0; i < nLines; i++){
        // decide number of holes in the line
        int holeJ = std::uniform_int_distribution<int>(0, width - 1)(rng);
        for (int j = 0; j < width; j++){
            if (j == holeJ) continue;
            matrix[height - 1 - i][j] = Color::GRAY;
        }
    }
    return true;
}

bool Grid::isTopOut() const{ // checks if the tetromino has been insert at least partially in the spawn zone
    for (int i = 0; i < 2; i++){
        for (int j = 0; j < width; j++){
            if (matrix[i][j] != Color::NONE){
                return true;
            }
        }
    }
    return false;
}

std::string Grid::serialize() const{
    std::string serializedGrid = "";
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            serializedGrid += std::to_string(static_cast<int>(matrix[i][j])) + " ";
        }
    }
    return serializedGrid;
}