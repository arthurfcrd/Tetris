#include "graphics.hpp"
#include "grid.hpp"
#include "tetromino.hpp"

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

// create a centered text in a rect whose upper left corner is located at (textRect.x, textRect.y)
// the given width and height attributes of textRect are ignored and filled
// automatically according to the text
void drawText(SDL_Renderer* renderer, SDL_Rect* textRect, std::string message, int ptsize) {
    // load the font if not already done
    if (textFont != NULL) {
        TTF_CloseFont(textFont);
    }
    textFont = TTF_OpenFont("../assets/mightysouly.ttf", ptsize);
    if (!textFont) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return;
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


// HUD methods definition

HUD::HUD() : score(0), nLinesCleared(0) {
    nextBox = new Grid(5, 4);
    holdBox = new Grid(5, 4);
}

unsigned int HUD::getScore() const {
    return score;
}

void HUD::setScore(int newScore) {
    score = newScore;
}

unsigned int HUD::getLinesCleared() const {
    return nLinesCleared;
}

void HUD::setLinesCleared(int newVal) {
    nLinesCleared = newVal;
}

void HUD::insertIntoNextBox(Tetromino& tetro) {
    nextBox->clearGrid();
    if (tetro.getType() == TetrominoType::I)
        tetro.setPos(1,1);
    else if (tetro.getType() == TetrominoType::O)
        tetro.setPos(1,2);
    else
        tetro.setPos(2,2);
    nextBox->insertTetromino(tetro);
}

void HUD::drawHUD(SDL_Renderer* renderer, Tetromino nextTetro, Tetromino holdTetro) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 200);
    SDL_RenderClear(renderer);
    

    // Draw the right pane
    int rightPaneX = PANE_SIZE + TILE_SIZE * GRID_WIDTH;
    
    // "Next" text
    SDL_Rect nextTextRect = {rightPaneX, PADDING, 0, 0};
    drawText(renderer, &nextTextRect, "NEXT", DEFAULT_PTSIZE);

    // Box holding the next tetromino
    int startX = rightPaneX+(PANE_SIZE - nextBox->getWidth()*TILE_SIZE)/2;
    int startY = nextTextRect.y+nextTextRect.h+PADDING;
    insertIntoNextBox(nextTetro);
    nextBox->drawGrid(renderer, startX, startY);

    // Display the score
    SDL_Rect scoreTextRect = { rightPaneX, 
        startY + nextBox->getHeight()*TILE_SIZE + 2*PADDING, 0, 0};
    drawText(renderer, &scoreTextRect, "SCORE", DEFAULT_PTSIZE);
    SDL_Rect scoreRect = {rightPaneX, scoreTextRect.y+scoreTextRect.h+PADDING, 0, 0};
    drawText(renderer, &scoreRect, std::to_string(score), DEFAULT_PTSIZE);

    // Display number of cleared lines
    SDL_Rect linesTextRect = {rightPaneX, scoreRect.y+scoreRect.h+3*PADDING, 0, 0};
    drawText(renderer, &linesTextRect, "Lines cleared", 30);
    SDL_Rect linesClearedRect = {rightPaneX, linesTextRect.y+linesTextRect.h+PADDING/2, 0, 0};
    drawText(renderer, &linesClearedRect, std::to_string(nLinesCleared), DEFAULT_PTSIZE);


    // Draw the left pane
    SDL_Rect holdTextRect = {0, PADDING, 0, 0};
    drawText(renderer, &holdTextRect, "HOLD", 50);
    int holdStartX = (PANE_SIZE - holdBox->getWidth()*TILE_SIZE) / 2;
    int holdStartY = holdTextRect.y + holdTextRect.h + PADDING;
    holdBox->drawGrid(renderer, holdStartX, holdStartY);
}


HUD::~HUD() {
    delete nextBox;
    delete holdBox;
}
