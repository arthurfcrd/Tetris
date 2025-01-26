#include "game.hpp"
#include <iostream>


Game::Game(GameType gt, int nltc, int ttc) : 
            hud(gt, nltc, ttc), grid(), tetroBag(), 
            running(true), gameOver(false),
            lastFallTime(std::chrono::system_clock::now()), 
            lastHorizontalMoveTime(std::chrono::system_clock::now()), 
            lastVerticalMoveTime(std::chrono::system_clock::now()), 
            keyboardHandler() {
    lineClearedSound = Mix_LoadWAV("../assets/audio/sounds/line-cleared.mp3");
    holdLockedSound = Mix_LoadWAV("../assets/audio/sounds/select_b_fail1.wav");
    hardDropSound = Mix_LoadWAV("../assets/audio/sounds/harddrop.mp3");
    levelUpSound = Mix_LoadWAV("../assets/audio/sounds/levelup.mp3");
}

Game::Game() : Game(GameType::LINES_BASED, 10, 0) {}

Game::~Game() {
    Mix_FreeChunk(lineClearedSound);
    Mix_FreeChunk(holdLockedSound);
    Mix_FreeChunk(hardDropSound);
    Mix_FreeChunk(levelUpSound);
}

bool Game::hasWon() const{
    if (hud.getGameType() == GameType::LINES_BASED)
        return hud.getLinesCleared() >= hud.getLinesToClear();
    else if (hud.getGameType() == GameType::TIME_BASED)
        return hud.getTimeLeft() == 0;
    return false;
}

void Game::updateHandler(const SDL_Event& event){
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        keyboardHandler.handleEvent(event.key);
    }
}

void Game::update(){
    if (keyboardHandler.getKeyState(Key::ESC) || keyboardHandler.getKeyState(Key::Q)){
        running = false;
        return;
    }
    if (gameOver){
        return;
    }

    if (hasWon()) {
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
            // // pressing the Down key while touching the ground activates the locking; TODO : maybe change that
            // if (curTetro->hasTouchedGround())
            //     curTetro->setLocked(true);
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
        tetroBag.switchTetromino();
        releaseHoldLock();
    }
}


void Game::draw(SDL_Renderer* renderer) {
    hud.drawHUD(renderer, tetroBag.nextTetromino, tetroBag.heldTetromino);
    grid.drawGrid(renderer);
    tetroBag.currentTetromino.drawTetromino(renderer);
}


bool Game::isRunning() const{
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

