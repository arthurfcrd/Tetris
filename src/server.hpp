#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <asio.hpp>
#include "game.hpp"

class TetrisServer;

class OnlineGame {
private:
    Game game;                   // Game of the player
    BaseGame otherGame;          // Game of the other player
    int garbageToSend;           // Number of garbage lines to send to the other player

    friend void clientGame(SDL_Renderer* renderer, const std::string& host, const std::string& port);

public:
    OnlineGame();

    bool isRunning() const;
    void draw(SDL_Renderer* renderer) const;
    void updateFromServer(std::string serializedData);
    std::string serialize();
};

class TetrisSession : public std::enable_shared_from_this<TetrisSession> {
public:
    TetrisSession(asio::ip::tcp::socket socketVal, int playerIdVal, TetrisServer& serverVal);

    void start();
    int getPlayerId() const;
    void sendGameState(const std::string& gameState);

private:
    void doReadHeader();
    void doReadBody();
    void handleError(const std::error_code& ec);

    asio::ip::tcp::socket socket;
    int playerId;
    TetrisServer& server;

    static constexpr std::size_t header_length = 4; // Fixed size for the header
    char header[header_length];                     // Buffer for the message header
    std::size_t message_length;                     // Length of the incoming message payload
    std::vector<char> data;                         // Buffer for the message payload
};

class TetrisServer {
public:
    TetrisServer(asio::io_context& io_context, short port);

    void broadcastGameState(const std::string& gameState, int senderId);
    void removeSession(int playerId);

private:
    void startAccept();

    asio::ip::tcp::acceptor acceptor;
    asio::ip::tcp::socket socket;
    int playerCount;
    std::vector<std::shared_ptr<TetrisSession>> sessions;
};

