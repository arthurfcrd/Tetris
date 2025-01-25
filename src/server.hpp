#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <asio.hpp>
#include "game.hpp"

class OnlineGame {
private:
    Game game[2];
    bool running;
};

class TetrisSession : public std::enable_shared_from_this<TetrisSession> {
public:
    TetrisSession(asio::ip::tcp::socket socket, int playerId, OnlineGame& onlineGame)
        : socket_(std::move(socket)), playerId_(playerId), onlineGame_(onlineGame) {}

    void start() {
        doRead();
    }

private:
    void doRead() {
        auto self(shared_from_this());
        asio::async_read(socket_, asio::buffer(data_, max_length),
            [this, self](std::error_code ec, std::size_t length) {
                if (!ec) {
                    handleData(length);
                    doWrite(length);
                }
            });
    }

    void doWrite(std::size_t length) {
        auto self(shared_from_this());
        asio::async_write(socket_, asio::buffer(data_, length),
            [this, self](std::error_code ec, std::size_t /*length*/) {
                if (!ec) {
                    doRead();
                }
            });
    }

    void handleData(std::size_t length);

    asio::ip::tcp::socket socket_;
    enum { max_length = 1024 };
    char data_[max_length];
    int playerId_;
    OnlineGame& onlineGame_;
};

class TetrisServer {
public:
    TetrisServer(asio::io_context& io_context, short port)
        : acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount_(0), onlineGame_() {
        startAccept();
    }

private:
    void startAccept() {
        acceptor_.async_accept(
            [this](std::error_code ec, asio::ip::tcp::socket socket) {
                if (!ec) {
                    if (playerCount_ < 2){
                        std::make_shared<TetrisSession>(std::move(socket), playerCount_++, onlineGame_)->start();
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
    OnlineGame onlineGame_;
};