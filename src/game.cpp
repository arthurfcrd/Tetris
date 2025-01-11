#include "game.hpp"

void Game::update(SDL_Event event){
    if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP){
        keyboardHandler.handleEvent(event.key);
    }
    else{
        return;
    }
    if (keyboardHandler.keyStates[static_cast<int>(Key::LEFT)]){
        currentTetromino.move(grid, -1, 0);
    }
    if (keyboardHandler.keyStates[static_cast<int>(Key::RIGHT)]){
        currentTetromino.move(grid, 1, 0);
    }
    if (keyboardHandler.keyStates[static_cast<int>(Key::DOWN)]){
        currentTetromino.move(grid, 0, 1);
    }
    if (keyboardHandler.keyStates[static_cast<int>(Key::Z)]){
        currentTetromino.rotate(grid, -1);
    }
    if (keyboardHandler.keyStates[static_cast<int>(Key::X)]){
        currentTetromino.rotate(grid, 1);
    }
    lastMoveTime = std::chrono::system_clock::now();
}

void Game::update(){
    std::chrono::time_point<std::chrono::system_clock> currentTime = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = currentTime - lastMoveTime;
    if (elapsed_seconds.count() > 0.7){
        currentTetromino.move(grid, 0, 1);
        lastMoveTime = currentTime;
    }
}

void Game::draw(SDL_Renderer* renderer) const{
    grid.drawGrid(renderer);
    currentTetromino.drawTetromino(renderer);
}