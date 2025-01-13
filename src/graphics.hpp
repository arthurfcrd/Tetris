#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#elif __APPLE__
#include <SDL.h>
#endif

#define TILE_SIZE (30)
#define TILE_PADDING (2)
#define GRID_WIDTH (10)
#define GRID_HEIGHT (22)

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
