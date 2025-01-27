#include "server.hpp"

// --- OnlineGame Implementation ---

OnlineGame::OnlineGame() : game(GameType::MULTIPLAYER, 10, 3 * 60), otherGame(), garbageToSend(0) {}

bool OnlineGame::isRunning() const {
    return game.isRunning() && otherGame.isRunning();
}

void OnlineGame::draw(SDL_Renderer* renderer) const {
    game.hud.drawHUD(renderer, game.tetroBag.nextTetromino, game.tetroBag.heldTetromino);
    game.draw(renderer);
    int startX = PANE_SIZE + GRID_WIDTH * TILE_SIZE + SPACE_BETWEEN_GRIDS;
    int startY = 0;
    otherGame.grid.drawGrid(renderer, startX, startY);
    otherGame.curTetromino.drawTetromino(renderer, startX, startY);
}

void OnlineGame::updateFromServer(std::string serializedData) {
    // Cut the string into 3 parts separated by ':'
    std::istringstream iss(serializedData);
    std::string serializedGrid, serializedTetromino, score, garbage;
    std::getline(iss, serializedGrid, ':');
    std::getline(iss, serializedTetromino, ':');
    std::getline(iss, score, ':');

    // Update the other player's game
    otherGame.grid = Grid(serializedGrid);
    otherGame.curTetromino = BaseTetromino(serializedTetromino);
    game.hud.setScore(std::stoi(score));
    if (std::getline(iss, garbage)) {
        int nGarbageLines = std::stoi(garbage);
        game.grid.addGarbageLines(nGarbageLines);
    }
}

std::string OnlineGame::serialize() {
    std::string res = game.grid.serialize() + ":" + game.curTetromino.serialize()
                      + ":" + std::to_string(game.hud.getScore());
    if (garbageToSend > 0) {
        res += ":" + std::to_string(garbageToSend);
        garbageToSend = 0;
    }
    return res;
}

// --- TetrisSession Implementation ---

TetrisSession::TetrisSession(asio::ip::tcp::socket socketVal, int playerIdVal, TetrisServer& serverVal)
    : socket(std::move(socketVal)), playerId(playerIdVal), server(serverVal) {}

void TetrisSession::start() {
    doRead();
}

int TetrisSession::getPlayerId() const {
    return playerId;
}

void TetrisSession::sendGameState(const std::string& gameState) {
    auto self(shared_from_this());
    asio::async_write(socket, asio::buffer(gameState),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                doRead();
            }
            else{
                std::cerr << "Error sending game state: " << ec.message() << std::endl;
            }
        });
}

void TetrisSession::doRead() {
    auto self(shared_from_this());
    asio::async_read(socket, asio::buffer(data, max_length),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::string gameState = std::string(data, length);
                server.broadcastGameState(gameState, playerId);
                // empty the data 
                std::fill(data, data + max_length, 0);
                doWrite(length);
            }
            else{
                std::cerr << "Error reading data: " << ec.message() << std::endl;
            }
        });
}

void TetrisSession::doWrite(std::size_t length) {
    auto self(shared_from_this());
    asio::async_write(socket, asio::buffer(data, length),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                doRead();
            }
            else{
                std::cerr << "Error sending data: " << ec.message() << std::endl;
            }
        });
}

/*void TetrisSession::handleData(std::size_t length) {
    // Handle incoming data from the client
    std::string receivedData(data, length);
    std::cout << "reading in handleData" << std::endl;
    std::cout << "\t" << receivedData << std::endl;

    // Update the game state based on received data
    onlineGame.updateFromServer(receivedData);

    // Serialize and broadcast the game state
    std::string gameState = onlineGame.serialize();
    std::copy(gameState.begin(), gameState.end(), data);
    server.broadcastGameState(gameState, playerId);
}*/

// --- TetrisServer Implementation ---

TetrisServer::TetrisServer(asio::io_context& io_context, short port)
    : controlAcceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), 
    socket(io_context), playerCount(0), sessions() {
    startAccept();
}

void TetrisServer::broadcastGameState(const std::string& gameState, int senderId) {
    for (auto& session : sessions) {
        if (session->getPlayerId() != senderId) {
            session->sendGameState(gameState);
        }
    }
}

void TetrisServer::startAccept() {
    controlAcceptor.async_accept(socket,
            [this](std::error_code ec) {
                if (!ec) {
                    std::cout << "Accepting connecting (playerId=)" << playerCount << std::endl;
                    auto session = std::make_shared<TetrisSession>(std::move(socket), playerCount++, *this);
                    sessions.push_back(session);
                    session->start();
                } else {
                    std::cerr << "Error accepting data connection: " << ec.message() << std::endl;
                }
                startAccept();
            });
}
