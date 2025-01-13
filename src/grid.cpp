#include "grid.hpp"
#include "tetromino.hpp"
#include <iostream>

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
    SDL_Rect rect = {0, 0, TILE_SIZE, TILE_SIZE};
    for(int i = 0; i < height; i++){
        for(int j = 0; j < width; j++){
            rect.x = j * rect.w;
            rect.y = i * rect.h;
            drawSquare(renderer, rect, matrix[i][j]);
        }
    }
}

// move the line lineNum down 1 block
void Grid::moveLineDown(int lineNum) {
    assert(lineNum >= 0 && lineNum < height-1);

    for (int col = 0; col < width; col++) {
        matrix[lineNum+1][col] = matrix[lineNum][col];
        matrix[lineNum][col] = Color::NONE;
    }

}

// move every lines above baseLine down 1 block
void Grid::moveLinesDown(int baseLine) {
    for (int line = baseLine-1; line > 0; line--) {
        moveLineDown(line);
    }
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
        for (int currLine = height-1; currLine >= 0; currLine--) {
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
    return nCleared;
}