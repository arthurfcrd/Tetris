#pragma once
#include "graphics.hpp"
#include <string>

class Tetromino; // forward declaration
struct Point; // forward declaration

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
    Grid(std::string serializedGrid) : Grid(GRID_WIDTH, GRID_HEIGHT){
        int i = 0;
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                matrix[y][x] = static_cast<Color>(serializedGrid[i] - '0');
                i++;
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

    int getWidth() const;
    int getHeight() const;

    void clearGrid();

    bool isInbounds(Point) const; // checks if a tile is within the grid
    bool isUnoccupied(Point) const; // checks if a tile is not occupied
    void insertTetromino(const Tetromino&); // inserts a tetromino into the grid
    bool isTopOut() const; // checks if the tetromino has been insert at least partially in the spawn zone
    int clearLines(); // clear full lines and return their number
    void drawGrid(SDL_Renderer* renderer) const; // draws the grid on the screen
    // draws the grid on the screen starting at position (startX, startY)
    void drawGrid(SDL_Renderer* renderer, int startX, int startY) const;
    std::string serialize() const; // serialize the grid data
private:
    bool lineIsFull(int lineNum) const; // Check if the line has to be cleared
    void moveLineDown(int lineNum); 
    void moveLinesDown(int baseLine); // move every lines above (when seeing in game) lineNum down 1 block
    
    friend class Tetromino;
};