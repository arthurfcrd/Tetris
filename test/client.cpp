#include <iostream>
#include <thread>
#include <asio.hpp>

using asio::ip::tcp;

class ChatClient {
private:
    asio::io_context& io_context_;
    tcp::socket socket_;

public:
    ChatClient(asio::io_context& io_context, const tcp::resolver::results_type& endpoints)
        : io_context_(io_context), socket_(io_context) {
        connect(endpoints);
    }

    void write(const std::string& msg) {
        asio::async_write(socket_, asio::buffer(msg + "\n"),
            [this](asio::error_code ec, std::size_t) {
                if (!ec) {
                    // Message sent successfully
                } else {
                    std::cout << "Error: " << ec.message() << std::endl;
                    socket_.close();
                }
            });
    }

    void close() {
        asio::post(io_context_, [this]() { socket_.close(); });
    }

private:
    void connect(const tcp::resolver::results_type& endpoints) {
        asio::async_connect(socket_, endpoints,
            [this](asio::error_code ec, tcp::endpoint) {
                if (!ec) {
                    read_message();
                } else {
                    std::cout << "Connection error: " << ec.message() << std::endl;
                }
            });
    }

    void read_message() {
        asio::async_read_until(socket_, streambuf_, '\n',
            [this](asio::error_code ec, std::size_t) {
                if (!ec) {
                    std::string msg(asio::buffers_begin(streambuf_.data()),
                                    asio::buffers_end(streambuf_.data()));
                    std::cout << msg;
                    streambuf_.consume(streambuf_.size());
                    read_message();
                } else {
                    std::cout << "Error: " << ec.message() << std::endl;
                    socket_.close();
                }
            });
    }

    asio::streambuf streambuf_;
};


int main() {
    try {
        asio::io_context io_context;
        tcp::resolver resolver(io_context);
        auto endpoints = resolver.resolve("localhost", "8080");
        ChatClient client(io_context, endpoints);

        std::thread t([&io_context]() { io_context.run(); });

        std::string line;
        while (std::getline(std::cin, line)) {
            client.write(line);
        }

        client.close();
        t.join();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    return 0;
}