#include "game.hpp"
#include "music.hpp"
//#include <asio.hpp>
#include <iostream>
#include "online.hpp"

#define ERROR 1

void setMultiplayerWindow(SDL_Window* window, SDL_Renderer* renderer) {
    int newWidth = (TILE_SIZE * GRID_WIDTH) * 2 + SPACE_BETWEEN_GRIDS + PANE_SIZE * 2;
    int newHeight = TILE_SIZE * GRID_HEIGHT;
    SDL_SetWindowSize(window, newWidth, newHeight);
}

void setRegularWindow(SDL_Window* window, SDL_Renderer* renderer) {
    int newWidth = TILE_SIZE * GRID_WIDTH + PANE_SIZE * 2;
    int newHeight = TILE_SIZE * GRID_HEIGHT;
    SDL_SetWindowSize(window, newWidth, newHeight);
}

void playGame(SDL_Renderer* renderer, Game* game) {
    Music tetrisMusic("../assets/audio/musics/original-theme.mp3", MIX_MAX_VOLUME/2);
    tetrisMusic.playOnLoop();

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


void clientGame(SDL_Renderer* renderer, const std::string& host, const std::string& port) {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);
        TetrisClient client(io_context, endpoints, renderer);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception in game loop: " << e.what() << "\n";
    }
}

void hostAndPlayGame(SDL_Renderer* renderer, const std::string& port) {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed\n";
        exit(1);
    } else if (pid == 0) {
        // Child process : run the server
        try {
            asio::io_context io_context;
            asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), stoi(port));
            TetrisServer server(io_context, endpoint);
            io_context.run();
        } catch (std::exception& e) {
            std::cerr << "Exception in server: " << e.what() << "\n";
        }
        exit(0); // quit the child process after the server is done
    } else {
        // Parent process : run the client
        sleep(1); // Wait so that the server is ready
        clientGame(renderer, "localhost", port);
    }
}

int main(int argc, char* argv[]) {
    std::string port = "8080";
    if (argc == 2) {
        port = argv[1];
    }
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
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

    if (Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0) {
        SDL_Log("Failed to load mixer: %s", Mix_GetError());
        return ERROR;
    }

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    int width = TILE_SIZE * GRID_WIDTH + PANE_SIZE * 2; 
    int height = TILE_SIZE * GRID_HEIGHT;
    if (SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer) < 0) {
        SDL_Log("Unable to create window and renderer: %s", SDL_GetError());
        return 1;
    }

    // Load music
    Music introMusic("../assets/audio/musics/bp_MUS_SpaceCruise.ogg");
    introMusic.playOnLoop();

    // Load the user inteface
    BaseUI mainUI(renderer, "TETRIS", {"SOLO", "MULTIPLAYER", "QUIT"});
    BaseUI soloUI(renderer, "SOLO", {"START", "GAMEMODE", "BACK"});
    BaseUI gamemodeUI(renderer, "GAMEMODE", {"NORMAL", "CLASSIC", "MARATHON", "ULTRA", "INFINITE", "BACK"});
    BaseUI multiplayerUI(renderer, "MULTIPLAYER", {"HOST", "JOIN", "BACK"});
    BaseUI* currentUI = &mainUI;
    
    bool isRunning = true;
    SDL_Event event;
    while (isRunning) {
        SDL_SetWindowSize(window, width, height);
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT)
                isRunning = false;
            else {
                std::string choice = currentUI->getChoice(event);
                if (currentUI == &mainUI) {
                    if (choice == "SOLO") {
                        currentUI = &soloUI;
                    } else if (choice == "MULTIPLAYER") {
                        currentUI = &multiplayerUI;
                    }else if (choice == "QUIT") {
                        isRunning = false;
                    }
                } else if (currentUI == &soloUI) {
                    if (choice == "START") {
                        soloGame(renderer);
                        introMusic.playOnLoop();
                    } else if (choice == "GAMEMODE") {
                        currentUI = &gamemodeUI;
                    } else if (choice == "BACK") {
                        currentUI = &mainUI;
                    }
                } else if (currentUI == &gamemodeUI) {
                    if (choice == "NORMAL") {
                        soloGame(renderer, GameType::LINES_BASED, 40, 0);
                        introMusic.playOnLoop();
                    } else if (choice == "CLASSIC") {
                        soloGame(renderer, GameType::CLASSIC, 0, 0);
                        introMusic.playOnLoop();
                    } else if (choice == "MARATHON") {
                        soloGame(renderer, GameType::LINES_BASED, 150, 0);
                        introMusic.playOnLoop();
                    } else if (choice == "ULTRA") {
                        soloGame(renderer, GameType::TIME_BASED, 0, 3*60);
                        introMusic.playOnLoop();
                    } else if (choice == "INFINITE") {
                        soloGame(renderer, GameType::INFINITE, 0, 0);
                        introMusic.playOnLoop();
                    } else if (choice == "BACK") {
                        currentUI = &mainUI;
                    }
                }
                else if (currentUI == &multiplayerUI) {
                    if (choice == "HOST") {
                        setMultiplayerWindow(window, renderer);
                        hostAndPlayGame(renderer, port);
                    } else if (choice == "JOIN") {
                        setMultiplayerWindow(window, renderer);
                        clientGame(renderer, "localhost", port);
                    } else if (choice == "BACK") {
                        currentUI = &mainUI;
                    }
                }
            }
            currentUI->drawUI();
        }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    Mix_Quit();
    TTF_Quit();
    SDL_Quit();
    return 0;
}