#include <iostream>
#include <asio.hpp>
#include "server.hpp"

void clientGame(SDL_Renderer* renderer, const std::string& host, const std::string& port) {
    try {
        asio::io_context io_context;
        asio::ip::tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve(host, port);
        asio::ip::tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        OnlineGame onlineGame;
        SDL_Event event;
        while (onlineGame.isRunning()) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    onlineGame.game.setRunning(false);
                } else {
                    onlineGame.game.updateHandler(event);
                }
            }
            onlineGame.game.update();
            onlineGame.draw(renderer);
            SDL_RenderPresent(renderer);

            // Send game state to server
            std::string gameState = onlineGame.serialize();
            asio::write(socket, asio::buffer(gameState));

            // Receive game state from server
            char reply[1024];
            size_t reply_length = asio::read(socket, asio::buffer(reply, 1024));
            std::string receivedData(reply, reply_length);
            onlineGame.updateFromServer(receivedData);
        }
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }
}