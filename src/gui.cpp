#include "gui.hpp"

#include "tetromino.hpp"

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

void HUD::insertIntoBox(Grid* box, Tetromino& tetro) {
    box->clearGrid();
    if (tetro.getType() == TetrominoType::I)
        tetro.setPos(1, 1);
    else if (tetro.getType() == TetrominoType::O)
        tetro.setPos(1, 2);
    else
        tetro.setPos(2, 2);
    box->insertTetromino(tetro);
}

void HUD::drawHUD(SDL_Renderer* renderer, Tetromino nextTetro, Tetromino holdTetro) {
    SDL_SetRenderDrawColor(renderer, 15, 15, 15, 0);
    SDL_RenderClear(renderer); // TODO : maybe does not belong here

    // Draw the right pane
    int rightPaneX = PANE_SIZE + TILE_SIZE * GRID_WIDTH;
    
    // "Next" text
    SDL_Rect nextTextRect = {rightPaneX, PADDING, 0, 0};
    drawText(renderer, &nextTextRect, "NEXT", DEFAULT_PTSIZE);

    // Box holding the next tetromino
    int startX = rightPaneX + (PANE_SIZE - nextBox->getWidth() * TILE_SIZE) / 2;
    int startY = nextTextRect.y + nextTextRect.h + PADDING;
     if (nextTetro.getType() != TetrominoType::NONE)
        insertIntoBox(nextBox, nextTetro);
    nextBox->drawGrid(renderer, startX, startY);

    // Display the score
    SDL_Rect scoreTextRect = {rightPaneX, startY + nextBox->getHeight() * TILE_SIZE + 2 * PADDING, 0, 0};
    drawText(renderer, &scoreTextRect, "SCORE", DEFAULT_PTSIZE);
    SDL_Rect scoreRect = {rightPaneX, scoreTextRect.y + scoreTextRect.h + PADDING, 0, 0};
    drawText(renderer, &scoreRect, std::to_string(score), DEFAULT_PTSIZE);

    // Display number of cleared lines
    SDL_Rect linesTextRect = {rightPaneX, scoreRect.y + scoreRect.h + 3 * PADDING, 0, 0};
    drawText(renderer, &linesTextRect, "Lines cleared", 30);
    SDL_Rect linesClearedRect = {rightPaneX, linesTextRect.y + linesTextRect.h + PADDING / 2, 0, 0};
    drawText(renderer, &linesClearedRect, std::to_string(nLinesCleared), DEFAULT_PTSIZE);

    // Draw the left pane
    SDL_Rect holdTextRect = {0, PADDING, 0, 0};
    drawText(renderer, &holdTextRect, "HOLD", 50);
    int holdStartX = (PANE_SIZE - holdBox->getWidth() * TILE_SIZE) / 2;
    int holdStartY = holdTextRect.y + holdTextRect.h + PADDING;
    if (holdTetro.getType() != TetrominoType::NONE)
        insertIntoBox(holdBox, holdTetro);
    holdBox->drawGrid(renderer, holdStartX, holdStartY);
}

HUD::~HUD() {
    delete nextBox;
    delete holdBox;
}
