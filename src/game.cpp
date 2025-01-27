#include "game.hpp"
#include <iostream>


Game::Game(GameType gt, int nltc, int ttc, bool isRunning) : 
            BaseGame(isRunning), hud(gt, nltc, ttc), 
            gameOver(false),
            lastFallTime(std::chrono::system_clock::now()), 
            lastHorizontalMoveTime(std::chrono::system_clock::now()), 
            lastVerticalMoveTime(std::chrono::system_clock::now()), 
            keyboardHandler() {
    lineClearedSound = Mix_LoadWAV("../assets/audio/sounds/line-cleared.mp3");
    holdLockedSound = Mix_LoadWAV("../assets/audio/sounds/select_b_fail1.wav");
    hardDropSound = Mix_LoadWAV("../assets/audio/sounds/harddrop.mp3");
    levelUpSound = Mix_LoadWAV("../assets/audio/sounds/levelup.mp3");
}

Game::~Game() {
    Mix_FreeChunk(lineClearedSound);
    Mix_FreeChunk(holdLockedSound);
    Mix_FreeChunk(hardDropSound);
    Mix_FreeChunk(levelUpSound);
}

bool Game::hasWon() const{
    GameType gt = hud.getGameType();
    if (gt == GameType::LINES_BASED)
        return hud.getLinesCleared() >= hud.getLinesToClear();
    else if (gt == GameType::TIME_BASED)
        return hud.getTimeLeft() == 0;
    else if (gt == GameType::CLASSIC)
        return hud.getCurrentLevel() == 15;
    return false;
}

void Game::updateHandler(const SDL_Event& event){
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        keyboardHandler.handleEvent(event.key);
    }
}

void Game::updateHandler(Key key){
    keyboardHandler.setKeyState(key, true);
}

void Game::update(){
    if (keyboardHandler.getKeyState(Key::ESC) || keyboardHandler.getKeyState(Key::Q)){
        running = false;
        return;
    }

    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    Tetromino* curTetro = &(tetroBag.currentTetromino); // useful shorthand

    // fall of the tetromino
    double fallRate = (hud.getGameType() == GameType::CLASSIC) ? hud.getFallRate() : FALL_RATE;
    std::chrono::duration<double> elapsedSecondsFromFall = currentTime - lastFallTime;
    if (!curTetro->hasTouchedGround() && elapsedSecondsFromFall.count() > fallRate){
        // falls only if the tetromino has not yet touched the ground
        curTetro->move(grid, 0, 1);
        lastFallTime = currentTime;
    }

    // handle moves (even when the tetromino has touched the ground)
    std::chrono::duration<double> elapsedSecondsFromVerticalMove = currentTime - lastVerticalMoveTime;
    if (elapsedSecondsFromVerticalMove.count() > VERTICAL_MOVE_RATE){
        if (keyboardHandler.getKeyState(Key::DOWN)){
            curTetro->move(grid, 0, 1);
        }
        lastVerticalMoveTime = currentTime;
    }

    std::chrono::duration<double> elapsedSecondsFromHorizontalMove = currentTime - lastHorizontalMoveTime;
    if (elapsedSecondsFromHorizontalMove.count() > HORIZONTAL_MOVE_RATE){
        if (keyboardHandler.getKeyState(Key::LEFT)){
            curTetro->move(grid, -1, 0);
        }
        if (keyboardHandler.getKeyState(Key::RIGHT)){
            curTetro->move(grid, 1, 0);
        }
        lastHorizontalMoveTime = currentTime;
    }

    // handle hard drop
    if (keyboardHandler.getKeyState(Key::SPACE)) {
        keyboardHandler.setKeyState(Key::SPACE, false);
        int hardDropPos = curTetro->getPosY();
        while (!curTetro->checkCollision(grid))
            curTetro->move(grid, 0, 1);
        Mix_PlayChannel(-1, hardDropSound, 0);
        // adds two times the hard drop distance to the score
        hud.setScore(hud.getScore() + 2*(curTetro->getPosY()-hardDropPos)); 
        curTetro->setTouchedGround(true);
        curTetro->setLocked(true);
    }

    // handle piece holding
    if (keyboardHandler.getKeyState(Key::H)) {
        keyboardHandler.setKeyState(Key::H, false);

        if (canHold()) {
            tetroBag.hold();
            setHoldLock();
        } else {
            Mix_Volume(0, MIX_MAX_VOLUME);
            Mix_PlayChannel(0, holdLockedSound, 0);
        }
    }

    // handle rotations
    if (keyboardHandler.getKeyState(Key::Z)){
        curTetro->rotate(grid, -1);
        keyboardHandler.setKeyState(Key::Z, false);
    }
    if (keyboardHandler.getKeyState(Key::X)){
        curTetro->rotate(grid, 1);
        keyboardHandler.setKeyState(Key::X, false);
    }
    if (keyboardHandler.getKeyState(Key::UP)){
        curTetro->rotate(grid, 1);
        keyboardHandler.setKeyState(Key::UP, false);
    }

    // check for collision
    if (curTetro->checkCollision(grid)){
        if (!curTetro->hasTouchedGround()) {
            curTetro->setTouchedGround(true);
            touchedGroundTime = std::chrono::system_clock::now();
        }
    } else {
        // a tetromino can be moved laterally before locking so it can keep falling
        curTetro->setTouchedGround(false);
    }

    if (curTetro->hasTouchedGround()) {
        std::chrono::duration<double> elapsedGround = currentTime - touchedGroundTime;
        if (elapsedGround.count() > LOCK_DELAY) {
            curTetro->setLocked(true);
        }
    }

    if (curTetro->isLocked()) {
        grid.insertTetromino(tetroBag.currentTetromino);
        if (grid.isTopOut()){ // check for top out
            gameOver = true;
            return;
        }
        int scoreTable[5] = {0, 100, 300, 500, 800};
        int n = grid.clearLines();
        if (n > 0) {
            Mix_PlayChannel(-1, lineClearedSound, 0);
        }
        hud.setScore(hud.getScore() + scoreTable[n]);
        hud.setLinesCleared(hud.getLinesCleared() + n);
        hud.updateLevel();
        if (hasWon()) {
            gameOver = true;
            return;
        }
        tetroBag.switchTetromino();
        releaseHoldLock();
    }
}


void Game::draw(SDL_Renderer* renderer) {
    if (gameOver) {
        showGameOverInfos(renderer);
        running = false;
    } else {
        hud.drawHUD(renderer, tetroBag.nextTetromino, tetroBag.heldTetromino);
        grid.drawGrid(renderer);
        tetroBag.currentTetromino.drawTetromino(renderer);
        tetroBag.currentTetromino.drawGhost(renderer, grid);
    }
}

void Game::showGameOverInfos(SDL_Renderer* renderer) {
    std::string text;
    std::string musicFile;
    if (hasWon()) {
        musicFile = "../assets/audio/sounds/victory.mp3";
        text = "You won!\n";
        if (hud.getGameType() == GameType::CLASSIC)
            text += "You reached level 15\n";
        text += ("You cleared " + std::to_string(hud.getLinesCleared()) + " lines\n");
        text += "Your score is " + std::to_string(hud.getScore()) + "\n";
    } else {
        musicFile = "../assets/audio/sounds/gameover.mp3";
        text = "You lost!\n";
        if (hud.getGameType() == GameType::CLASSIC)
            text += "You were at level " + std::to_string(hud.getCurrentLevel()) + "\n";
        else if (hud.getGameType() != GameType::INFINITE)
            text += "You only cleared " + std::to_string(hud.getLinesCleared()) + " lines over "
                + std::to_string(hud.getLinesToClear()) + "\n";
        text += "Your score is " + std::to_string(hud.getScore()) + "\n";

    }
    text += "Press Escape to continue...";

    InfoUI infoUI(renderer, "GAME OVER", text);
    SDL_Event event;
    Music infoMusic(musicFile);
    infoMusic.playOnce();
    while (true) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) {
                return;
            }
        }
        infoUI.showInfo();
    }
}


bool BaseGame::isRunning() const{
    return running;
}

void Game::setRunning(bool newRunning){
    this->running = newRunning;
}

void Game::setHoldLock() {
    canHold_ = false;
}

void Game::releaseHoldLock() {
    canHold_ = true;
}

bool Game::canHold() const {
    return canHold_;
}

