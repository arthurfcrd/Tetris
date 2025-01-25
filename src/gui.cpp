#include "gui.hpp"
#include "tetromino.hpp"
#include <iostream>

void printRect(SDL_Rect* rect) {
    std::cout << "x=" << rect->x << ", y=" << rect->y << ", w=" << rect->w <<", h=" << rect->h << std::endl;
}

SDL_Texture* createTextureFromIMG(SDL_Renderer* renderer, std::string path) {
    SDL_Surface* surf = IMG_Load(path.c_str());
    if (surf == NULL) {
        SDL_Log("Unable to load the image: %s", SDL_GetError());
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
    if (texture== NULL) {
        SDL_Log("Unable to create texture: %s", SDL_GetError());
        return NULL;
    }
    return texture;
}

TTF_Font* loadFont(std::string path, int ptsize) {
    TTF_Font* textFont = TTF_OpenFont(path.c_str(), ptsize);
    if (textFont == NULL) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
    }
    return textFont;
    
}

// draw a centered text at position yDelta and returns the y coordinate of the bottom
int drawCenteredText(SDL_Renderer* renderer, int yDelta, std::string message, int ptsize) {
    TTF_Font* textFont = loadFont("../assets/fonts/mightysouly.ttf", ptsize);

    SDL_Color white = {255,255,255,255};
    SDL_Surface* textSurf = TTF_RenderText_Blended(textFont, message.c_str(), white);
    if (!textSurf) {
        SDL_Log("Failed to create text surface: %s", TTF_GetError());
        return 0;
    }
    SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textSurf);
    SDL_FreeSurface(textSurf);

    SDL_Rect textRect = {0, yDelta, textSurf->w, textSurf->h};
    SDL_GetRendererOutputSize(renderer, &textRect.x, NULL);
    textRect.x  = (textRect.x - textRect.w) / 2;
    //std::cout << textRect.h << std::endl;
    SDL_RenderCopy(renderer, textTex, NULL, &textRect);
    SDL_DestroyTexture(textTex);

    TTF_CloseFont(textFont);

    return textRect.y + textRect.h;
}


// Button methods
Button::Button(SDL_Renderer* r, std::string textVal, std::string normalBtnFile, std::string hgBtnFile) {
    renderer = r;
    btnText = createTextureFromIMG(r, normalBtnFile);
    hgBtnText = createTextureFromIMG(r, hgBtnFile);
    isHighlighted = false;
    text = textVal;
}

bool Button::getHighlighted() const {
    return isHighlighted;
}

void Button::setHighlighted(bool newVal) {
    isHighlighted = newVal;
}

void Button::draw(int y, int width) {
    SDL_Texture* texture = btnText;
    if (isHighlighted)
        texture = hgBtnText;

    btnRect.x = 0;
    btnRect.y = y;
    btnRect.w = width;
    SDL_QueryTexture(texture, NULL, NULL, NULL, &btnRect.h);

    SDL_RenderCopy(renderer, texture, NULL, &btnRect);
    
    drawCenteredText(renderer, btnRect.y+30, text, 60);
}

// BaseUI methods

BaseUI::BaseUI(SDL_Renderer* r, std::string firstTitle, std::vector<std::string> buttonsText) {
    renderer = r;
    SDL_GetRendererOutputSize(r, &windowWidth, &windowHeight);
    title = firstTitle;
    buttons.clear();
    for (std::string text : buttonsText) {
        buttons.push_back(Button(renderer, text));
    }

    // Load the background texture once
    background = createTextureFromIMG(renderer, "../assets/img/background/bg_dullstars5.png");
    bgRect = {0, 0, windowWidth, windowWidth};
    SDL_QueryTexture(background, NULL, NULL, &bgRect.x, &bgRect.y);
    bgRect.x = (bgRect.x - windowWidth) / 2;
    bgRect.y = (bgRect.y - windowHeight) / 2;
}

void BaseUI::drawTitle() {
    titleBottom = drawCenteredText(renderer, titlePaddding, title, 100);
}

void BaseUI::drawBackground() {
    SDL_RenderCopy(renderer, background, &bgRect, NULL);
}

void BaseUI::drawButtons() {
    int buttonSpace = (windowHeight-titleBottom) / buttons.size();
    //std::cout << windowHeight << " " << titleBottom << " " << buttonPadding << std::endl;
    int y = titleBottom;
    for (int i = 0; i < (int)buttons.size(); i++) {
        buttons[i].draw(y, windowWidth);
        y += buttonSpace;
    }
}

void BaseUI::drawUI() {
    drawBackground();
    drawTitle();
    drawButtons();
    SDL_RenderPresent(renderer);
}

std::string BaseUI::getChoice(SDL_Event& event) {
    if (event.type == SDL_MOUSEMOTION) {
        SDL_Point curPos = {event.motion.x, event.motion.y};
        for (int i = 0; i < (int)buttons.size(); i++) {
            if (SDL_PointInRect(&curPos, &buttons[i].btnRect)) {
                buttons[i].setHighlighted(true);
            } else {
                buttons[i].setHighlighted(false);
            }
        }
    }

    if (event.type == SDL_MOUSEBUTTONUP) {
        SDL_Point curPos = {event.button.x, event.button.y};
        for (int i = 0; i < (int)buttons.size(); i++) {
            if (SDL_PointInRect(&curPos, &buttons[i].btnRect))
                return buttons[i].text;
        }
    }
    return "NONE";
}

BaseUI::~BaseUI() {
    SDL_DestroyTexture(background);
}




// HUD methods definition
HUD::HUD(GameType gt, int nltc, int ttc){
    score = 0;
    nLinesCleared = 0;
    gameType = gt;
    nLinesToClear = nltc;
    timeToClear = ttc;
    nextBox = new Grid(5, 4);
    holdBox = new Grid(5, 4);
    gameChrono = std::chrono::system_clock::now();
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

GameType HUD::getGameType() const {
    return gameType;
}

unsigned int HUD::getLinesToClear() const {
    return nLinesToClear;
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

double HUD::getTimeLeft() const {
    auto currentTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed = (currentTime - gameChrono);
    double timeLeft = timeToClear - elapsed.count();
    if (timeLeft < 0)
        timeLeft = 0;
    return timeLeft;
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
    std::string linesIndicator = std::to_string(nLinesCleared);
    if (gameType == GameType::LINES_BASED)
        linesIndicator += ("/" + std::to_string(nLinesToClear));
    drawText(renderer, &linesClearedRect, linesIndicator, DEFAULT_PTSIZE);

    // Draw the left pane
    SDL_Rect holdTextRect = {0, PADDING, 0, 0};
    drawText(renderer, &holdTextRect, "HOLD", 50);
    int holdStartX = (PANE_SIZE - holdBox->getWidth() * TILE_SIZE) / 2;
    int holdStartY = holdTextRect.y + holdTextRect.h + PADDING;
    if (holdTetro.getType() != TetrominoType::NONE)
        insertIntoBox(holdBox, holdTetro);
    holdBox->drawGrid(renderer, holdStartX, holdStartY);

    // Draw the timer
    if (gameType == GameType::TIME_BASED) {
        SDL_Rect timeTextRect = {0, holdStartY+TILE_SIZE*holdBox->getWidth() , 0, 0};
        drawText(renderer, &timeTextRect, "TIME LEFT", 50);

        double timeLeft = getTimeLeft();
        int minutes = timeLeft / 60;
        int seconds = (timeLeft - 60*minutes);
        std::string timeMessage = std::to_string(minutes) + ":" + std::to_string(seconds);

        SDL_Rect timeRect = {0, timeTextRect.y+timeTextRect.h+PADDING, 0, 0};
        drawText(renderer, &timeRect, timeMessage, 50);
    }
}

HUD::~HUD() {
    delete nextBox;
    delete holdBox;
}
