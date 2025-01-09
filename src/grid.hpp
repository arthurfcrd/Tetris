#pragma once
#include "graphics.hpp"

class Grid{
public:
    int width;
    int height;
    Color** grid;
public:
    explicit Grid(int width, int height) : width(width), height(height){
        grid = new Color*[height];
        for(int i = 0; i < height; i++){
            grid[i] = new Color[width];
            for(int j = 0; j < width; j++){
                grid[i][j] = Color::NONE;
            }
        }
    }
    Grid() : Grid(GRID_WIDTH, GRID_HEIGHT){}
    ~Grid(){
        for(int i = 0; i < height; i++){
            delete[] grid[i];
        }
        delete[] grid;
    }
    bool isValidTile(Point) const; // checks if a tile is within the grid and is not occupied
    void drawGrid(SDL_Renderer* renderer) const; // draws the grid on the screen
    friend class Tetromino;
};