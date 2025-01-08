#include "tetromino.hpp"
#include "grid.hpp"

bool isValidTile(Point point, const Grid& g){ // checks if a tile is within the grid and is not occupied
    return point.x >= 0 && point.x < g.width && point.y >= 0 && point.y < g.height && g.grid[point.x][point.y] == Color::NONE;
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