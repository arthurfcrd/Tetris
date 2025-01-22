#include "game.hpp"

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
        tetroBag.currentTetromino.move(grid, 0, 1);
        lastFallTime = currentTime;
    }

    // handle moves (even when the tetromino has touched the ground)
    std::chrono::duration<double> elapsedSecondsFromMove = currentTime - lastMoveTime;
    if (elapsedSecondsFromMove.count() > MOVE_RATE){
        
        if (keyboardHandler.getKeyState(Key::LEFT)){
            tetroBag.currentTetromino.move(grid, -1, 0);
        }
        if (keyboardHandler.getKeyState(Key::RIGHT)){
            tetroBag.currentTetromino.move(grid, 1, 0);
        }
        if (keyboardHandler.getKeyState(Key::DOWN)){
            tetroBag.currentTetromino.move(grid, 0, 1);
            // pressing the Down key while touching the ground activates the locking
            if (curTetro->hasTouchedGround())
                curTetro->setLocked(true);
        }
        lastMoveTime = currentTime;
    }

    // handle rotations
    std::chrono::duration<double> elapsedSecondsFromRotation = currentTime - lastRotationTime;
    if (!curTetro->hasTouchedGround() && elapsedSecondsFromRotation.count() > ROTATE_RATE){
        // only rotates if the tetrominio has not yet touched the ground
        if (keyboardHandler.getKeyState(Key::Z)){
            tetroBag.currentTetromino.rotate(grid, -1);
        }
        if (keyboardHandler.getKeyState(Key::X) || keyboardHandler.getKeyState(Key::UP)){
            tetroBag.currentTetromino.rotate(grid, 1);
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
        int scoreTable[5] = {0, 40, 100, 300, 1200};
        int n = grid.clearLines();
        hud.setScore(hud.getScore() + scoreTable[n]);
        hud.setLinesCleared(hud.getLinesCleared() + n);
        tetroBag.switchTetromino();
        
    }
}


void Game::draw(SDL_Renderer* renderer) {
    hud.drawHUD(renderer, tetroBag.nextTetromino, tetroBag.currentTetromino);
    grid.drawGrid(renderer);
    tetroBag.currentTetromino.drawTetromino(renderer);
}


bool Game::isRunning() const{
    return running;
}

void Game::setRunning(bool newRunning){
    this->running = newRunning;
}