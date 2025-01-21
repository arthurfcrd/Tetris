#include "graphics.hpp"

TTF_Font* textFont = NULL;

SDL_Color white = {255,255,255,255};
SDL_Color red = {255, 0, 0, 255};

void drawSquare(SDL_Renderer* renderer, const SDL_Rect& rect, Color color){
    // add a black border
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &rect);
    SDL_Rect newRect = rect;
    newRect.x += TILE_PADDING;
    newRect.y += TILE_PADDING;
    newRect.h -= 2 * TILE_PADDING;
    newRect.w -= 2 * TILE_PADDING;

    // draw the actual square
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


void drawRectOutline(SDL_Renderer* renderer, SDL_Rect* rect) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderDrawRect(renderer, rect);
}

void drawText(SDL_Renderer* renderer, SDL_Rect* textRect, std::string message) {
    // load the font if not already done
    if (textFont == NULL) {
        textFont = TTF_OpenFont("../assets/mightysouly.ttf", 50);
        if (!textFont) {
            SDL_Log("Failed to load font: %s", TTF_GetError());
            return;
        }
    }

    SDL_Surface* textSurf = TTF_RenderText_Blended(textFont, message.c_str(), white);
    if (!textSurf) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        return;
    }
    
    int horiz_padding = (PANE_SIZE - textSurf->w) / 2;
    textRect->x += horiz_padding;
    textRect->w = textSurf->w;
    textRect->h = textSurf->h;

    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
    SDL_RenderCopy(renderer, textTex, NULL, textRect);
    SDL_FreeSurface(textSurf);
    SDL_DestroyTexture(textTex);

}

void drawHUD(SDL_Renderer* renderer, unsigned int score){
    int PADDING = 20;

    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 200);
    SDL_RenderClear(renderer);

    int rightPaneX = PANE_SIZE + TILE_SIZE * GRID_WIDTH;

    // "Next" text
    SDL_Rect nextTextRect = {rightPaneX, PADDING, 0, 0};
    drawText(renderer, &nextTextRect, "NEXT");
    //drawRectOutline(renderer, &nextTextRect);

    // Box holding the next tetromino
    SDL_Rect nextBox = {rightPaneX+30, nextTextRect.y+nextTextRect.h+PADDING, 
                        PANE_SIZE-2*30, PANE_SIZE-2*30};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &nextBox);

    // "Score" text
    SDL_Rect scoreTextRect = {rightPaneX, nextBox.y+nextBox.h+40, 0, 0};
    drawText(renderer, &scoreTextRect, "SCORE");
    //drawRectOutline(renderer, &scoreTextRect);

    // the score
    SDL_Rect scoreRect = {rightPaneX, scoreTextRect.y+scoreTextRect.h+20, 0, 0};
    drawText(renderer, &scoreRect, std::to_string(score));
}