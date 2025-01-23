#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#elif __APPLE__
#include <SDL.h>
#include <SDL_ttf.h>
#endif

#include "point.hpp"
#include <string>

#define TILE_SIZE (30)
#define TILE_PADDING (2)
#define GRID_WIDTH (10)
#define GRID_HEIGHT (22)

#define PANE_SIZE (260)
#define PADDING (20)
#define DEFAULT_PTSIZE (50) // font size
#define SMALLGRID_SIZE (6)

enum class Color{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    ORANGE,
    NONE
};

// foward declaration (defined in tetromino.hpp)
class Tetromino;
// forward declaration (defined in grid.hpp)
class Grid;

// draws a square of a given color at a given position
void drawSquare(SDL_Renderer* renderer, const SDL_Rect& rect, Color color); 
void drawText(SDL_Renderer* renderer, SDL_Rect* textRect, std::string message, int ptsize);


class HUD {
    private:
        Grid* nextBox;
        Grid* holdBox;
        unsigned int score;
        unsigned int nLinesCleared;
    public:
        HUD();
        unsigned int getScore() const;
        void setScore(int newScore);

        unsigned int getLinesCleared() const;
        void setLinesCleared(int newVal);

        void insertIntoBox(Grid* box, Tetromino& tetro);
        void drawHUD(SDL_Renderer* renderer, Tetromino nextTetro, Tetromino holdTetro);
        ~HUD();
};