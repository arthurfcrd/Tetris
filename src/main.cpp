#include "graphics.hpp"
#include "grid.hpp"
#include "tetromino.hpp"


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

    Grid grid;
    Tetromino tetromino(TetrominoType::I);
    grid.drawGrid(renderer);
    tetromino.drawTetromino(renderer);

    SDL_RenderPresent(renderer);

    SDL_Event event;
    bool done = false;
    while (!done) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = true;
            }
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    return 0;
}