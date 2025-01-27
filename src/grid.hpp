#pragma once
#include "graphics.hpp"
#include <string>
#include <sstream>
#include <iostream>
#include <cassert> 

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
        std::istringstream iss(serializedGrid);
        //std::cout << "serialized data\n\t" << serializedGrid << std::endl;
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                int color;
                iss >> color;
                //std::cout << y << " " << x << color << std::endl;
                matrix[y][x] = static_cast<Color>(color);
            }
        }
    }
    Grid() : Grid(GRID_WIDTH, GRID_HEIGHT){}

    Grid(const Grid& other){
        assert(other.width == width && other.height == height);
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                matrix[y][x] = other.matrix[y][x];
            }
        }
    }

    void fromSerialized(std::string serializedGrid) {
        std::istringstream iss(serializedGrid);
        //std::cout << "serialized data\n\t" << serializedGrid << std::endl;
        for(int y = 0; y < height; y++){
            for(int x = 0; x < width; x++){
                int color;
                iss >> color;
                //std::cout << y << " " << x << color << std::endl;
                matrix[y][x] = static_cast<Color>(color);
            }
        }
    }

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
    bool addGarbageLines(int nLines); // add nLines garbage lines at the bottom of the grid
    void drawGrid(SDL_Renderer* renderer) const; // draws the grid on the screen
    // draws the grid on the screen starting at position (startX, startY)
    void drawGrid(SDL_Renderer* renderer, int startX, int startY) const;
    std::string serialize() const; // serialize the grid data
private:
    bool lineIsFull(int lineNum) const; // Check if the line has to be cleared
    void moveLineDown(int lineNum); 
    void moveLinesDown(int baseLine); // move every lines above (when seeing in game) lineNum down 1 block
    bool moveLineUp(int lineNum); // move the line up 1 block
    bool moveLinesUp(int nbOfLines); // move every lines up (when seeing in game) nbOfLines
    
    friend class Tetromino;
};