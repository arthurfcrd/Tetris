#include <iostream>
#include <vector>
#include <string>
#include <asio.hpp>
#include "game.hpp"

class OnlineGame{
private:
    HUD hud; // TODO : handle HUD for online games
    Game game; // game of the player
    BaseGame otherGame; // game of the other player
    int garbageToSend; // number of garbage lines to send to the other player
public: 
    OnlineGame() : hud(GameType::MULTIPLAYER), game(), otherGame(), garbageToSend(0) {}
    // void update();
    void updateFromServer(std::string serializedData);
    std::string serialize();
};

class TetrisSession : public std::enable_shared_from_this<TetrisSession> {
public:
    TetrisSession(asio::ip::tcp::socket socket, int playerId, TetrisServer& server)
        : socket(std::move(socket)), playerId(playerId), server(server), onlineGame(){}

    void start() {
        doRead();
    }

    int getPlayerId() const {
        return playerId;
    }

    void sendGameState(const std::string& gameState) {
        auto self(shared_from_this());
        asio::async_write(socket, asio::buffer(gameState),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    doRead();
                }
            });
    }

private:
    void doRead() {
        auto self(shared_from_this());
        asio::async_read(socket, asio::buffer(data, max_length),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    handleData(length);
                    doWrite(length);
                }
            });
    }

    void doWrite(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(socket, asio::buffer(data, length),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    doRead();
                }
            });
    }

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
    TetrisServer(asio::io_context& io_context, short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount_(0){
        startAccept();
    }

    void broadcastGameState(const std::string& gameState, int senderId) {
        for (auto& session : sessions) {
            if (session->getPlayerId() != senderId) {
                session->sendGameState(gameState);
            }
        }
    }

private:
    void startAccept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    if (playerCount_ < 2){
                        auto session = std::make_shared<TetrisSession>(std::move(socket), playerCount_++, *this);
                        sessions.push_back(session);
                        session->start();
                    }
                    else {
                        // Refuse the connection if the server is full
                        socket.close();
                    }
                }
                startAccept();
            });
    }

    asio::ip::tcp::acceptor acceptor_;
    int playerCount_;
    std::vector<std::shared_ptr<TetrisSession>> sessions;
};