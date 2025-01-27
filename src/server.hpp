#pragma once

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <memory>
#include <asio.hpp>
#include "game.hpp"


class OnlineGame {
public:
    Game game;               // Game of the player
    BaseGame otherGame;          // Game of the other player
    int garbageToSend;           // Number of garbage lines to send to the other player

    friend void clientGame(SDL_Renderer* renderer, const std::string& host, const std::string& port);

public:
    OnlineGame();

    bool isRunning() const;
    void run();
    void draw(SDL_Renderer* renderer);
    void updateFromServer(std::string serializedData);
    std::string serialize();
};



class TetrisClient {
    asio::io_context& io_context_;
    asio::ip::tcp::socket socket_;
    asio::streambuf streambuf_;
    SDL_Renderer* renderer_;
    int playerId;
public:
    OnlineGame onlineGame;
    TetrisClient(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, SDL_Renderer* renderer);
    void connect(const asio::ip::tcp::resolver::results_type& endpoints);
    void run();
    int getPlayerId() const;
    void sendGameState();
    void readServerInfo();
    //void readGameState(); 
    void close();
};                  

class TetrisServer {
public:
    TetrisServer(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint);

private:
    asio::io_context& io_context_;
    asio::ip::tcp::acceptor acceptor_;
    std::set<std::shared_ptr<asio::ip::tcp::socket>> clients_;
    int playerCount;

    void startAccept();
    void startRead(std::shared_ptr<asio::ip::tcp::socket> client);
    void broadcastMsg(const std::string& msg, std::shared_ptr<asio::ip::tcp::socket> sendingClient);
};

