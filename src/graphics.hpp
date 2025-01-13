#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif __APPLE__
#include <SDL.h>
#include <SDL_image.h>
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

struct Point{
    int x;
    int y;
    // operator for adding points
    Point operator+(const Point& other) const;
    // operator for subtracting points
    Point operator-(const Point& other) const;
};
Point operator*(int scalar, const Point& point); // operator for scalar * Point

// draws a square of a given color at a given position
void drawSquare(SDL_Renderer* renderer, const SDL_Rect& rect, Color color); 
