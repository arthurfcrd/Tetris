#include "game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include <iostream>

#define ERROR 1


void soloGame(SDL_Renderer* renderer) {
    Game game;
    SDL_Event event;
    while (game.isRunning()) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game.setRunning(false);
            }
            else{
                game.updateHandler(event);
            }
        }
        game.update();
        game.draw(renderer);

        SDL_RenderPresent(renderer);
    }
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return ERROR;
    }
    if (TTF_Init() < 0 ) {
	    SDL_Log("Error initializing SDL_ttf: %s", TTF_GetError());
        return ERROR;
    }

    int flags = IMG_INIT_PNG;
    if ((IMG_Init(flags) & flags) != flags) {
        SDL_Log("Failed to init png support: %s", IMG_GetError());
        return ERROR;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width = TILE_SIZE * GRID_WIDTH + PANE_SIZE*2; 
    int height = TILE_SIZE * GRID_HEIGHT;
    //std::cout << width << "x" << height << std::endl;
    if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0) {
        SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
        return 1;
    }

    BaseUI mainUI(renderer, "TETRIS", {"Solo", "Multiplayer", "Controls", "Quit"});
    
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;
            else {
                switch(mainUI.getChoice(event)) {
                    case MainOption::SOLO:
                        soloGame(renderer);
                        break;
                    case MainOption::MULTIPLAYER:
                        break;
                    case MainOption::CONTROLS:
                        break;
                    case MainOption::QUIT:
                        isRunning = false;
                        break;
                    case MainOption::NONE:
                        break;
                }
            }
            mainUI.drawUI();
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    TTF_Quit();

    return 0;
}