#include <asio.hpp>
#include <iostream>
#include <string>

#include <set>
#include <deque>

using asio::ip::tcp;

class ChatServer {
private:
    asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    std::set<std::shared_ptr<tcp::socket>> clients_;
    std::deque<std::string> recent_msgs_;
    enum { max_recent_msgs = 100 };

public:
    ChatServer(asio::io_context& io_context, const tcp::endpoint& endpoint)
        : io_context_(io_context), acceptor_(io_context, endpoint) {
        start_accept();
    }

private:
    void start_accept() {
        auto new_client = std::make_shared<tcp::socket>(io_context_);
        acceptor_.async_accept(*new_client,
            [this, new_client](asio::error_code ec) {
                if (!ec) {
                    clients_.insert(new_client);
                    start_read(new_client);
                }
                start_accept();
            });
    }

    void start_read(std::shared_ptr<tcp::socket> client) {
        auto buffer = std::make_shared<asio::streambuf>();
        asio::async_read_until(*client, *buffer, '\n',
            [this, client, buffer](asio::error_code ec, std::size_t) {
                if (!ec) {
                    std::string msg(asio::buffers_begin(buffer->data()),
                                    asio::buffers_end(buffer->data()));
                    buffer->consume(buffer->size());
                    broadcast_message(msg);
                    start_read(client);
                } else {
                    clients_.erase(client);
                }
            });
    }

    void broadcast_message(const std::string& msg) {
        recent_msgs_.push_back(msg);
        while (recent_msgs_.size() > max_recent_msgs) {
            recent_msgs_.pop_front();
        }

        for (auto& client : clients_) {
            asio::async_write(*client, asio::buffer(msg),
                [this, client](asio::error_code ec, std::size_t) {
                    if (ec) {
                        clients_.erase(client);
                    }
                });
        }
    }
};


int main(int argc, char* argv[]) {
    std::string port = "8080";
    if (argc == 2)
        port = argv[1];
    
    try {
        asio::io_context io_context;
        tcp::endpoint endpoint(tcp::v4(), stoi(port));
        ChatServer serv(io_context, endpoint);
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception in server: " << e.what() << "\n";
        return -1;
    }
    return 0;
}