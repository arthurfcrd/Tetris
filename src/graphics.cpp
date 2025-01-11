#include "graphics.hpp"

void drawSquare(SDL_Renderer* renderer, SDL_Rect rect, Color color){
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
    SDL_RenderFillRect(renderer, &rect);
}

Point operator*(int scalar, const Point& point){ // operator for scalar * Point
    return {scalar * point.x, scalar * point.y};
}