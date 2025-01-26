#include "server.hpp"

std::string OnlineGame::serialize(){
    std::string res = game.grid.serialize() + ":" + game.curTetromino.serialize() 
    + ":" + std::to_string(hud.getScore());
    if (garbageToSend > 0){
        res += ":" + std::to_string(garbageToSend);
        garbageToSend = 0;
    }
    return res;
}

void OnlineGame::updateFromServer(std::string serializedData){ // update the other player's game according to the data received from the server
    // cut the string into 3 parts separated by ':'
    std::istringstream iss(serializedData);
    std::string serializedGrid, serializedTetromino, score, garbage;
    std::getline(iss, serializedGrid, ':');
    std::getline(iss, serializedTetromino, ':');
    std::getline(iss, score, ':');
    // update the other player's game
    otherGame.grid = Grid(serializedGrid);
    otherGame.curTetromino = BaseTetromino(serializedTetromino);
    hud.setScore(std::stoi(score));
    if (std::getline(iss, garbage)){
        int nGarbageLines = std::stoi(garbage);
        game.grid.addGarbageLines(nGarbageLines);
    }
}

void TetrisSession::handleData(std::size_t length) {
    // handle incoming data from the client
    std::string receivedData(data, length);
    std::cout << "Received data: " << receivedData << std::endl;

    // update this->otherGame (drawing) and this->game (add garbage lines)
    // according to the data received from the server
    onlineGame.updateFromServer(receivedData);

    // send the serialized game to the server
    std::string gameState = onlineGame.serialize();
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