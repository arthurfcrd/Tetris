#include "game.hpp"
#include <iostream>

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
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    Tetromino* curTetro = &(tetroBag.currentTetromino); // useful shorthand

    // fall of the tetromino
    std::chrono::duration<double> elapsedSecondsFromFall = currentTime - lastFallTime;
    if (!curTetro->hasTouchedGround() && elapsedSecondsFromFall.count() > FALL_RATE){
        // falls only if the tetromino has not yet touched the ground
        curTetro->move(grid, 0, 1);
        lastFallTime = currentTime;
    }

    // handle moves (even when the tetromino has touched the ground)
    std::chrono::duration<double> elapsedSecondsFromMove = currentTime - lastMoveTime;
    if (elapsedSecondsFromMove.count() > MOVE_RATE){
        
        if (keyboardHandler.getKeyState(Key::LEFT)){
            curTetro->move(grid, -1, 0);
        }
        if (keyboardHandler.getKeyState(Key::RIGHT)){
            curTetro->move(grid, 1, 0);
        }
        if (keyboardHandler.getKeyState(Key::DOWN)){
            curTetro->move(grid, 0, 1);
            // pressing the Down key while touching the ground activates the locking
            if (curTetro->hasTouchedGround())
                curTetro->setLocked(true);
        }
        lastMoveTime = currentTime;
    }

    // handle hard drop
    if (keyboardHandler.getKeyState(Key::SPACE)) {
        keyboardHandler.setKeyState(Key::SPACE, false);
        int hardDropPos = curTetro->getPosY();
        while (!curTetro->checkCollision(grid))
            curTetro->move(grid, 0, 1);
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
        }
    }

    // handle rotations
    std::chrono::duration<double> elapsedSecondsFromRotation = currentTime - lastRotationTime;
    if (!curTetro->hasTouchedGround() && elapsedSecondsFromRotation.count() > ROTATE_RATE){
        // only rotates if the tetrominio has not yet touched the ground
        if (keyboardHandler.getKeyState(Key::Z)){
            curTetro->rotate(grid, -1);
        }
        if (keyboardHandler.getKeyState(Key::X) || keyboardHandler.getKeyState(Key::UP)){
            curTetro->rotate(grid, 1);
        }
        lastRotationTime = currentTime;
    }

    // check for collision
    if (curTetro->checkCollision(grid)){
        if (!curTetro->hasTouchedGround()) {
            curTetro->setTouchedGround(true);
            touchedGroundTime = std::chrono::system_clock::now();
        }
    } else {
        // a tetromino can be moved laterally before locking so it can continue the falling
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
        hud.setScore(hud.getScore() + scoreTable[n]);
        hud.setLinesCleared(hud.getLinesCleared() + n);
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

