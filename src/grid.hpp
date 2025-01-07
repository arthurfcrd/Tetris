#pragma once
#include "graphics.hpp"

class Point; // forward declaration

class Grid{
private:
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
    Grid() : Grid(10, 22){}
    ~Grid(){
        for(int i = 0; i < height; i++){
            delete[] grid[i];
        }
        delete[] grid;
    }
    friend class Tetromino;
    friend bool isValidTile(Point point, Grid g);
};