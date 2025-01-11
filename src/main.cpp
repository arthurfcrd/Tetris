#include "game.hpp"

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    if (SDL_CreateWindowAndRenderer(TILE_SIZE * GRID_WIDTH, TILE_SIZE * GRID_HEIGHT, 0, &window, &renderer) < 0) {
        SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
        return 1;
    }

    Game game;
    
    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            switch (event.type){
                case SDL_QUIT :
                    done = true;
                    break;
                default:
                    game.update(event);
                    break;
            }
        }
        game.update();
        game.draw(renderer);

        SDL_RenderPresent(renderer);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}