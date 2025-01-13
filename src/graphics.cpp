#include "graphics.hpp"

Point Point::operator+(const Point& other) const{
    return {x + other.x, y + other.y};
}

Point Point::operator-(const Point& other) const{
    return {x - other.x, y - other.y};
}

Point operator*(int scalar, const Point& point){ // operator for scalar * Point
    return {scalar * point.x, scalar * point.y};
}

void drawSquare(SDL_Renderer* renderer, const SDL_Rect& rect, Color color){
    // add a black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_Rect newRect = rect;
    newRect.x += TILE_PADDING;
    newRect.y += TILE_PADDING;
    newRect.h -= 2*TILE_PADDING;
    newRect.w -= 2*TILE_PADDING;

    switch(color){
        case Color::RED:
            SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
            break;
        case Color::GREEN:
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
            break;
        case Color::BLUE:
            SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
            break;
        case Color::YELLOW:
            SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
            break;
        case Color::CYAN:
            SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
            break;
        case Color::MAGENTA:
            SDL_SetRenderDrawColor(renderer, 255, 0, 255, 255);
            break;
        case Color::ORANGE:
            SDL_SetRenderDrawColor(renderer, 255, 165, 0, 255);
            break;
        case Color::NONE:
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            break;
    }
    SDL_RenderFillRect(renderer, &newRect);
}