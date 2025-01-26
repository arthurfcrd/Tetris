#include "game.hpp"
//#include <asio.hpp>
#include <iostream>

#define ERROR 1

void playGame(SDL_Renderer* renderer, Game* game) {
    SDL_Event event;
    while (game->isRunning()) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                game->setRunning(false);
            }
            else{
                game->updateHandler(event);
            }
        }
        game->update();
        game->draw(renderer);
        SDL_RenderPresent(renderer);
    }
}

void soloGame(SDL_Renderer* renderer) {
    Game game;
    playGame(renderer, &game);
}

void soloGame(SDL_Renderer* renderer, GameType gametype, int nLinesToClear, double timeToClear) {
    Game game(gametype, nLinesToClear, timeToClear);
    playGame(renderer, &game);
}

void multiGame(SDL_Window* window, SDL_Renderer* renderer) {
    int newWidth = (TILE_SIZE * GRID_WIDTH)*2 + SPACE_BETWEEN_GRIDS + PANE_SIZE*2;
    int newHeight = TILE_SIZE * GRID_HEIGHT;
    SDL_SetWindowSize(window, newWidth, newHeight);
    SDL_Delay(5000);
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
    if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0) {
        SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
        return 1;
    }

    // Load the user inteface
    BaseUI mainUI(renderer, "TETRIS", {"SOLO", "MULTIPLAYER", "CONTROLS", "QUIT"});
    BaseUI soloUI(renderer, "SOLO", {"START", "GAMEMODE", "BACK"});
    BaseUI gamemodeUI(renderer, "GAMEMODE", {"NORMAL", "CLASSIC", "MARATHON", "ULTRA", "INFINITE", "BACK"});
    BaseUI* currentUI = &mainUI;
    
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;
            else {
                std::string choice = currentUI->getChoice(event);
                if (currentUI == &mainUI) {
                    if (choice == "SOLO") {
                        currentUI = &soloUI;
                    } else if (choice == "MULTIPLAYER") {
                        multiGame(window, renderer);
                    }else if (choice == "QUIT") {
                        isRunning = false;
                    }
                } else if (currentUI == &soloUI) {
                    if (choice == "START") {
                        soloGame(renderer);
                    } else if (choice == "GAMEMODE") {
                        currentUI = &gamemodeUI;
                    } else if (choice == "BACK") {
                        currentUI = &mainUI;
                    }
                } else if (currentUI == &gamemodeUI) {
                    if (choice == "NORMAL")
                        soloGame(renderer, GameType::LINES_BASED, 40, 0);
                    else if (choice == "CLASSIC")
                        soloGame(renderer, GameType::CLASSIC, 0, 0);
                    else if (choice == "MARATHON")
                        soloGame(renderer, GameType::LINES_BASED, 150, 0);
                    else if (choice == "ULTRA")
                        soloGame(renderer, GameType::TIME_BASED, 0, 3*60);
                    else if (choice == "INFINITE")
                        soloGame(renderer, GameType::INFINITE, 0, 0);
                    else if (choice == "BACK")
                        currentUI = &mainUI;
                }
            }
            currentUI->drawUI();
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();
    TTF_Quit();
    return 0;
}