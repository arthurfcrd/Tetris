#include "grid.hpp"
#include "tetromino.hpp"

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