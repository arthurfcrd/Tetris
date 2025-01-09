#include "grid.hpp"
#include "tetromino.hpp"

bool isValidTile(Point point, const Grid& g){ // checks if a tile is within the grid and is not occupied
    return point.x >= 0 && point.x < g.width && point.y >= 0 && point.y < g.height && g.grid[point.x][point.y] == Color::NONE;
}