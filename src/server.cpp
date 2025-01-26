#include "server.hpp"

void TetrisSession::handleData(std::size_t length) {
    // handle incoming data from the client
    std::string receivedData(data, length);
    std::cout << "Received data: " << receivedData << std::endl;

    // TODO: handle the received data
    // update this->otherGame (drawing) and this->game (add garbage lines)

    // send the serialized game to the server

    // Préparez les données à envoyer aux clients
    std::string gameState = "game state data"; // Remplacez par l'état réel du jeu
    std::copy(gameState.begin(), gameState.end(), data);
}


int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: server <port>\n";
            return 1;
        }

        asio::io_context io_context;
        TetrisServer server(io_context, std::atoi(argv[1]));
        io_context.run();
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}