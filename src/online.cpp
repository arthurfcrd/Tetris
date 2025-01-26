#include <iostream>
#include <asio.hpp>
#include "game.hpp"

void clientGame(SDL_Renderer* renderer, const std::string& host, const std::string& port) {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        Game game;
        SDL_Event event;
        while (game.isRunning()) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    game.setRunning(false);
                } else {
                    game.updateHandler(event);
                }
            }
            game.update();
            game.draw(renderer);
            SDL_RenderPresent(renderer);

            // Send game state to server
            std::string gameState = "game state data"; // Replace with actual game state data
            asio::write(socket, asio::buffer(gameState));

            // Receive game state from server
            char reply[1024];
            size_t reply_length = asio::read(socket, asio::buffer(reply, 1024));
            std::cout << "Reply is: ";
            std::cout.write(reply, reply_length);
            std::cout << "\n";
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}