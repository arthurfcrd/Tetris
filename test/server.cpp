#include <asio.hpp>

int main() {
    asio::io_context io_context;
    asio::ip::tcp::acceptor acceptor(io_context, "localhost");

    asio::ip::tcp::socket socket(io_context);
    acceptor.accept(socket);
}