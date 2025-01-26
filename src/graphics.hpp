#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
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

// draws a square of a given color at a given position
void drawSquare(SDL_Renderer* renderer, const SDL_Rect& rect, Color color); 
void drawText(SDL_Renderer* renderer, SDL_Rect* textRect, std::string message, int ptsize);
