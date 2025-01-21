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
        currentTetromino.move(grid, 0, 1);
        lastFallTime = currentTime;
    }

    // handle moves
    std::chrono::duration<double> elapsedSecondsFromMove = currentTime - lastMoveTime;
    if (elapsedSecondsFromMove.count() > MOVE_RATE){
        
        if (keyboardHandler.getKeyState(Key::LEFT)){
            currentTetromino.move(grid, -1, 0);
        }
        if (keyboardHandler.getKeyState(Key::RIGHT)){
            currentTetromino.move(grid, 1, 0);
        }
        if (keyboardHandler.getKeyState(Key::DOWN)){
            currentTetromino.move(grid, 0, 1);
        }
        lastMoveTime = currentTime;
    }

    // handle rotations
    std::chrono::duration<double> elapsedSecondsFromRotation = currentTime - lastRotationTime;
    if (elapsedSecondsFromRotation.count() > ROTATE_RATE){
        if (keyboardHandler.getKeyState(Key::Z)){
            currentTetromino.rotate(grid, -1);
        }
        if (keyboardHandler.getKeyState(Key::X) || keyboardHandler.getKeyState(Key::UP)){
            currentTetromino.rotate(grid, 1);
        }
        lastRotationTime = currentTime;
    }

    // check for collision
    if (currentTetromino.checkCollision(grid)){
        grid.insertTetromino(currentTetromino);
        if (grid.isTopOut()){ // check for top out
            gameOver = true;
            return;
        }
        int scoreTable[5] = {0, 40, 100, 300, 1200};
        int n = grid.clearLines();
        score += scoreTable[n];
        currentTetromino = Tetromino();
    }
}

void Game::draw_hud(SDL_Renderer* renderer) const {
    SDL_SetRenderDrawColor(renderer, 0, 0, 200, 200);
    SDL_RenderClear(renderer);
}

void Game::draw(SDL_Renderer* renderer) const{
    draw_hud(renderer);
    grid.drawGrid(renderer);
    currentTetromino.drawTetromino(renderer);
}


bool Game::isRunning() const{
    return running;
}

void Game::setRunning(bool newRunning){
    this->running = newRunning;
}