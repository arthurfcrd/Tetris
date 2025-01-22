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
    // fall of the tetromino
    std::chrono::duration<double> elapsedSecondsFromFall = currentTime - lastFallTime;
    if (elapsedSecondsFromFall.count() > FALL_RATE){
        tetroBag.currentTetromino.move(grid, 0, 1);
        lastFallTime = currentTime;
    }

    // handle moves
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
        }
        lastMoveTime = currentTime;
    }

    // handle rotations
    std::chrono::duration<double> elapsedSecondsFromRotation = currentTime - lastRotationTime;
    if (elapsedSecondsFromRotation.count() > ROTATE_RATE){
        if (keyboardHandler.getKeyState(Key::Z)){
            tetroBag.currentTetromino.rotate(grid, -1);
        }
        if (keyboardHandler.getKeyState(Key::X) || keyboardHandler.getKeyState(Key::UP)){
            tetroBag.currentTetromino.rotate(grid, 1);
        }
        lastRotationTime = currentTime;
    }

    // check for collision
    if (tetroBag.currentTetromino.checkCollision(grid)){
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