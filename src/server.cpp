#include "server.hpp"

// --- OnlineGame Implementation ---

OnlineGame::OnlineGame() : game(GameType::MULTIPLAYER, 10, 3 * 60), otherGame(), 
                           garbageToSend(0) {}

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




// --- TetrisClient Implementation ---

TetrisClient::TetrisClient(asio::io_context& io_context, 
                const asio::ip::tcp::resolver::results_type& endpoints, 
                SDL_Renderer* renderer) 
            : io_context_(io_context), socket_(io_context), renderer_(renderer)
{
    connect(endpoints);
}

void TetrisClient::connect(const asio::ip::tcp::resolver::results_type& endpoints) {
    asio::connect(socket_, endpoints);
    //std::cout << "Connection error: " << ec.message() << std::endl;
    readServerInfo();
}

void TetrisClient::readServerInfo() {
    asio::async_read_until(socket_, streambuf_, ';',
        [this](asio::error_code ec, std::size_t) {
            if (!ec) {
                std::string msg(asio::buffers_begin(streambuf_.data()),
                                asio::buffers_end(streambuf_.data()));
                if (msg == "START;") {
                    std::cout << "ok starting the game" << std::endl;
                    run();
                } else {
                    std::cout << "received bullshit\n\t" << msg << std::endl;
                }
                streambuf_.consume(streambuf_.size());
            } else {
                std::cout << "Error: " << ec.message() << std::endl;
                socket_.close();
            }
            readServerInfo();
        });
}


void TetrisClient::run() {
    SDL_Event event;
    readGameState(); // Start reading from the server
    while (onlineGame.isRunning()) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onlineGame.game.setRunning(false);
            } else {
                onlineGame.game.updateHandler(event);
            }
            onlineGame.game.update();
            onlineGame.draw(renderer_);
            SDL_RenderPresent(renderer_);
            sendGameState();
        }
    }
}


int TetrisClient::getPlayerId() const {
    return playerId;
}

void TetrisClient::readGameState() {
    asio::async_read_until(socket_, streambuf_, ';',
            [this](asio::error_code ec, std::size_t) {
                if (!ec) {
                    std::string msg(asio::buffers_begin(streambuf_.data()),
                                    asio::buffers_end(streambuf_.data()));
                    msg.pop_back();
                    onlineGame.updateFromServer(msg);
                    streambuf_.consume(streambuf_.size());
                    readGameState();
                } else {
                    std::cout << "Error reading game state: " << ec.message() << std::endl;
                    socket_.close();
                }
            });
}

void TetrisClient::sendGameState() {
    const std::string& gameState = onlineGame.serialize();
    asio::async_write(socket_, asio::buffer(gameState + ";"),
        [this](asio::error_code ec, std::size_t) {
            if (!ec) {
                // Message sent successfully
            } else {
                std::cout << "Error: " << ec.message() << std::endl;
                socket_.close();
            }
        });
}

void TetrisClient::close() {
    asio::post(io_context_, [this]() { socket_.close(); });
}




// --- TetrisServer Implementation ---

TetrisServer::TetrisServer(asio::io_context& io_context, const asio::ip::tcp::endpoint& endpoint)
    : io_context_(io_context), acceptor_(io_context, endpoint), playerCount(0) {
    startAccept();
}

void TetrisServer::startAccept() {
    auto newClient = std::make_shared<asio::ip::tcp::socket>(io_context_);
    acceptor_.async_accept(*newClient,
            [this, newClient](std::error_code ec) {
                if (!ec) {
                    std::cout << "Accepting connecting, playerId = " << playerCount++ << std::endl;
                    clients_.insert(newClient);
                    if (playerCount == 2) {
                        std::cout << "broadcasting 'START'" << std::endl;
                        broadcastMsg("START;", NULL);
                        for (auto& client : clients_)
                            startRead(client);
                        return;
                    }
                } else {
                    std::cerr << "Error accepting data connection: " << ec.message() << std::endl;
                }
                startAccept();
            });
}

void TetrisServer::startRead(std::shared_ptr<asio::ip::tcp::socket> client) {
        auto buffer = std::make_shared<asio::streambuf>();
        asio::async_read_until(*client, *buffer, ';',
            [this, client, buffer](asio::error_code ec, std::size_t) {
                if (!ec) {
                    std::string msg(asio::buffers_begin(buffer->data()),
                                    asio::buffers_end(buffer->data()));
                    buffer->consume(buffer->size());
                    broadcastMsg(msg, client);
                    startRead(client);
                } else if (ec == asio::error::eof) {
                    std::cout << "Client disconnected normally." << std::endl;
                    clients_.erase(client);
                } else {
                    std::cout << "Error reading from client: " << ec.message() << std::endl;
                    clients_.erase(client);
                }
            });
}

void TetrisServer::broadcastMsg(const std::string& gameState, 
    std::shared_ptr<asio::ip::tcp::socket> sendingClient)
{
    for (auto& client : clients_) {
        if (client != sendingClient) {
            asio::async_write(*client, asio::buffer(gameState),
                [this, client](asio::error_code ec, std::size_t) {
                    if (ec) {
                        clients_.erase(client);
                    }
                });
        }
    }
}
