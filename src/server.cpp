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
    std::getline(iss, garbage);

    // Update the other player's game
    otherGame.grid = Grid(serializedGrid);
    otherGame.curTetromino = BaseTetromino(serializedTetromino);
    game.hud.setEnemyScore(std::stoi(score));

    // Add garbage lines to the grid if necessary
    int nGarbageLines = std::stoi(garbage);
    if (nGarbageLines > 0) game.grid.addGarbageLines(nGarbageLines);
}

std::string OnlineGame::serialize() {
    std::string res = game.grid.serialize() + ":" + game.curTetromino.serialize()
                      + ":" + std::to_string(game.hud.getScore()) 
                      + ":" + std::to_string(garbageToSend);
    garbageToSend = 0;  // Reset the garbage to send
    return res;
}

// --- TetrisSession Implementation ---

TetrisSession::TetrisSession(asio::ip::tcp::socket socketVal, int playerIdVal, TetrisServer& serverVal)
    : socket(std::move(socketVal)), playerId(playerIdVal), server(serverVal) {}

void TetrisSession::start() {
    std::cout << "Session started for playerId = " << playerId << std::endl;
    doReadHeader();
}

int TetrisSession::getPlayerId() const {
    return playerId;
}

void TetrisSession::sendGameState(const std::string& gameState) {
    auto self(shared_from_this());
    asio::async_write(socket, asio::buffer(gameState),
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                doReadHeader();
            }
            else{
                std::cerr << "Error sending game state: " << ec.message() << std::endl;
            }
        });
}

void TetrisSession::doReadHeader() {
    auto self(shared_from_this());
    asio::async_read(socket, asio::buffer(header, header_length),  // Read the fixed-length header
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                message_length = std::stoi(std::string(header, header_length));  // Extract message length
                data.resize(message_length);  // Resize buffer for the payload
                doReadBody();  // Read the actual payload
            } else {
                handleError(ec);
            }
        });
}

void TetrisSession::doReadBody() {
    auto self(shared_from_this());
    asio::async_read(socket, asio::buffer(data.data(), data.size()),  // Read the payload
        [this, self](std::error_code ec, std::size_t /*length*/) {
            if (!ec) {
                std::string gameState(data.begin(), data.end());  // Convert payload to string
                server.broadcastGameState(gameState, playerId);  // Relay to other clients
                doReadHeader();  // Prepare for the next message
            } else {
                handleError(ec);
            }
        });
}

void TetrisSession::handleError(const std::error_code& ec) {
    std::cerr << "Session error (playerId = " << playerId << "): " << ec.message() << std::endl;
    server.removeSession(playerId);  // Ensure this session is removed
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
    : acceptor(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), 
    socket(io_context), playerCount(0), sessions() {
    startAccept();
}

void TetrisServer::broadcastGameState(const std::string& gameState, int senderId) {
    // add header to the message
    std::string header = std::to_string(gameState.size());
    header = std::string(4 - header.size(), '0') + header;
    std::string framedMessage = header + gameState;
    std::cout << "Broadcasting from player " << senderId << ": " << framedMessage << std::endl;
    for (auto& session : sessions) {
        if (session->getPlayerId() != senderId) {
            session->sendGameState(framedMessage);
        }
    }
}

void TetrisServer::startAccept() {
    acceptor.async_accept(socket,
            [this](std::error_code ec) {
                if (!ec) {
                    std::cout << "Accepting connecting, playerId = " << playerCount << std::endl;
                    auto session = std::make_shared<TetrisSession>(std::move(socket), playerCount++, *this);
                    sessions.push_back(session);
                    session->start();
                } else {
                    std::cerr << "Error accepting data connection: " << ec.message() << std::endl;
                }
                startAccept();
            });
}

void TetrisServer::removeSession(int playerId) {
    sessions.erase(std::remove_if(sessions.begin(), sessions.end(),
        [playerId](const std::shared_ptr<TetrisSession>& session) {
            return session->getPlayerId() == playerId;
        }),
        sessions.end());
}