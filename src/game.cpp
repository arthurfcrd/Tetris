#include "game.hpp"

void Game::update(const SDL_Event& event){
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        keyboardHandler.handleEvent(event.key);
    }
    else{
        return;
    }

    if (keyboardHandler.getKeyState(Key::LEFT)){
        currentTetromino.move(grid, -1, 0);
    }
    if (keyboardHandler.getKeyState(Key::RIGHT)){
        currentTetromino.move(grid, 1, 0);
    }
    if (keyboardHandler.getKeyState(Key::DOWN)){
        currentTetromino.move(grid, 0, 1);
    }
    if (keyboardHandler.getKeyState(Key::Z)){
        currentTetromino.rotate(grid, -1);
    }
    if (keyboardHandler.getKeyState(Key::X) || keyboardHandler.getKeyState(Key::UP)){
        currentTetromino.rotate(grid, 1);
    }
}

void Game::update(){
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = currentTime - lastMoveTime;
    if (elapsed_seconds.count() > FALL_SPEED){
        currentTetromino.move(grid, 0, 1);
        lastMoveTime = currentTime;
    }
}

void Game::draw(SDL_Renderer* renderer) const{
    grid.drawGrid(renderer);
    currentTetromino.drawTetromino(renderer);
}