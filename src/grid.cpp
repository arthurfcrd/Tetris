#include "grid.hpp"
#include "tetromino.hpp"

bool Grid::isValidTile(Point point) const{ // checks if a tile is within the grid and is not occupied
    return point.x >= 0 && point.x < width && point.y >= 0 && point.y < height && grid[point.x][point.y] == Color::NONE;
}