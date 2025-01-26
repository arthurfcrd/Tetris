#include "server.hpp"

// --- OnlineGame Implementation ---

OnlineGame::OnlineGame() : hud(GameType::MULTIPLAYER), game(), otherGame(), garbageToSend(0) {}

bool OnlineGame::isRunning() const {
    return game.isRunning() && otherGame.isRunning();
}

void OnlineGame::draw(SDL_Renderer* renderer) const {
    hud.drawHUD(renderer, game.tetroBag.nextTetromino, game.tetroBag.heldTetromino);
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
    hud.setScore(std::stoi(score));
    if (std::getline(iss, garbage)) {
        int nGarbageLines = std::stoi(garbage);
        game.grid.addGarbageLines(nGarbageLines);
    }
}

std::string OnlineGame::serialize() {
    std::string res = game.grid.serialize() + ":" + game.curTetromino.serialize()
                      + ":" + std::to_string(hud.getScore());
    if (garbageToSend > 0) {
        res += ":" + std::to_string(garbageToSend);
        garbageToSend = 0;
    }
    return res;
}

// --- TetrisSession Implementation ---

TetrisSession::TetrisSession(asio::ip::tcp::socket socket, int playerId, TetrisServer& server)
    : socket(std::move(socket)), playerId(playerId), server(server), onlineGame() {}

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
        });
}

void TetrisSession::doRead() {
    auto self(shared_from_this());
    asio::async_read(socket, asio::buffer(data, max_length),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                handleData(length);
                doWrite(length);
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
        });
}

void TetrisSession::handleData(std::size_t length) {
    // Handle incoming data from the client
    std::string receivedData(data, length);

    // Update the game state based on received data
    onlineGame.updateFromServer(receivedData);

    // Serialize and broadcast the game state
    std::string gameState = onlineGame.serialize();
    std::copy(gameState.begin(), gameState.end(), data);
    server.broadcastGameState(gameState, playerId);
}

// --- TetrisServer Implementation ---

TetrisServer::TetrisServer(asio::io_context& io_context, short port)
    : acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), playerCount(0) {
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
    acceptor.async_accept(
        [this](std::error_code ec, asio::ip::tcp::socket socket) {
            if (!ec) {
                if (playerCount < 2) {
                    auto session = std::make_shared<TetrisSession>(std::move(socket), playerCount++, *this);
                    sessions.push_back(session);
                    session->start();
                } else {
                    // Refuse the connection if the server is full
                    socket.close();
                }
            }
            startAccept();
        });
}
