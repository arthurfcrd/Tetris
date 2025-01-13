#pragma once
#include "graphics.hpp"
#include <cassert>

class Tetromino; // forward declaration

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
    bool isInbounds(Point) const; // checks if a tile is within the grid
    bool isUnoccupied(Point) const; // checks if a tile is not occupied
    void insertTetromino(const Tetromino&); // inserts a tetromino into the grid
    void drawGrid(SDL_Renderer* renderer) const; // draws the grid on the screen
    friend class Tetromino;
};