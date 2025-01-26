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
    HUD hud;                     // Handle HUD for online games
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
    TetrisSession(asio::ip::tcp::socket socket, int playerId, TetrisServer& server);

    void start();
    int getPlayerId() const;
    void sendGameState(const std::string& gameState);

private:
    void doRead();
    void doWrite(std::size_t length);
    void handleData(std::size_t length);

    asio::ip::tcp::socket socket;
    enum { max_length = 1024 };
    char data[max_length];
    int playerId;
    TetrisServer& server;
    OnlineGame onlineGame;
};

class TetrisServer {
public:
    TetrisServer(asio::io_context& io_context, short port);

    void broadcastGameState(const std::string& gameState, int senderId);

private:
    void startAccept();

    asio::ip::tcp::acceptor acceptor;
    int playerCount;
    std::vector<std::shared_ptr<TetrisSession>> sessions;
};

