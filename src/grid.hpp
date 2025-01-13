#pragma once
#include "graphics.hpp"

class Grid{
private:
    int width;
    int height;
    Color** matrix;
public:
    explicit Grid(int width, int height) : width(width), height(height){
        matrix = new Color*[height];
        for(int i = 0; i < height; i++){
            matrix[i] = new Color[width];
            for(int j = 0; j < width; j++){
                matrix[i][j] = Color::NONE;
            }
        }
    }
    Grid() : Grid(GRID_WIDTH, GRID_HEIGHT){}
    ~Grid(){
        for(int i = 0; i < height; i++){
            delete[] matrix[i];
        }
        delete[] matrix;
    }
    // checks if a tile is within the grid and is not occupied
    bool isValidTile(Point) const; 
    // draws the grid on the screen
    void drawGrid(SDL_Renderer* renderer) const; 
    friend class Tetromino;
};