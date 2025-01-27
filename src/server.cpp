#include "server.hpp"

// --- OnlineGame Implementation ---

OnlineGame::OnlineGame() : game(GameType::MULTIPLAYER, 10, 3 * 60), otherGame(), 
                           garbageToSend(0) {}

bool OnlineGame::isRunning() const {
    return game.isRunning() && otherGame.isRunning();
}

void OnlineGame::draw(SDL_Renderer* renderer) {
    game.hud.drawHUD(renderer, game.tetroBag.nextTetromino, game.tetroBag.heldTetromino);
    game.draw(renderer);
    int startX = PANE_SIZE + GRID_WIDTH * TILE_SIZE + SPACE_BETWEEN_GRIDS;
    int startY = 0;
    otherGame.grid.drawGrid(renderer, startX, startY);
    otherGame.tetroBag.currentTetromino.drawTetromino(renderer, startX, startY);
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
    otherGame.grid.fromSerialized(serializedGrid);
    otherGame.tetroBag.currentTetromino.fromSerialized(serializedTetromino);
    game.hud.setEnemyScore(std::stoi(score));

    // Add garbage lines to the grid if necessary
    int nGarbageLines = std::stoi(garbage);
    if (nGarbageLines > 0) game.grid.addGarbageLines(nGarbageLines);
}

std::string OnlineGame::serialize() {
    std::string res = game.grid.serialize() + ":" + game.tetroBag.currentTetromino.serialize()
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
    InfoUI waitingUI(renderer_,"WAITING FOR CONNECTION", "Please wait for the other player to join...");
    waitingUI.showInfo();
    asio::connect(socket_, endpoints);
    //std::cout << "Connection error: " << ec.message() << std::endl;
    readServerInfo();
}

void TetrisClient::readServerInfo() {
    asio::error_code ec;
    asio::read_until(socket_, streambuf_, ';', ec);
    if (!ec) {
        std::string msg(asio::buffers_begin(streambuf_.data()),
                        asio::buffers_end(streambuf_.data()));
        streambuf_.consume(streambuf_.size());
        if (msg == "START;") {
            run();
            close();
        } else {
            msg.pop_back();
            //std::cout << "[CLIENT] received\n\t" << msg << std::endl;
            onlineGame.updateFromServer(msg);
        }
    } else {
        std::cout << "Error: " << ec.message() << std::endl;
        socket_.close();
    }  
}


void TetrisClient::run() {
    Music tetrisMusic("../assets/audio/musics/original-theme.mp3", MIX_MAX_VOLUME/2);
    tetrisMusic.playOnLoop();
    SDL_Event event;
    while (onlineGame.isRunning()) {
        sendGameState();
        readServerInfo();
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                onlineGame.game.setRunning(false);
            } else {
                onlineGame.game.updateHandler(event);
            }
        }

        onlineGame.game.update();
        onlineGame.draw(renderer_);
        SDL_RenderPresent(renderer_);
    }
}


int TetrisClient::getPlayerId() const {
    return playerId;
}

void TetrisClient::sendGameState() {
    std::string gameState = onlineGame.serialize();
    asio::error_code ec;
    asio::write(socket_, asio::buffer(gameState + ";"), ec);
    if (!ec) {
        // Message sent successfully
        //std::cout << "\t succesfluy sent" << std::endl;
    } else {
        std::cout << "Error: " << ec.message() << std::endl;
        socket_.close();
    }
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
                        broadcastMsg("START;", NULL);
                        for (auto& client : clients_)
                            startRead(client);
                        return;
                    }
                    startAccept();
                } else {
                    std::cerr << "Error accepting data connection: " << ec.message() << std::endl;
                }
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
                    //std::cout << "[SERVER] Reading\n\t" << msg << std::endl;
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
            //std::cout << "[SERVER] broadcasting to " << client << std::endl;
            //std::cout << "\t" << gameState << std::endl;
            asio::async_write(*client, asio::buffer(gameState),
                [this, client](asio::error_code ec, std::size_t) {
                    if (ec) {
                        clients_.erase(client);
                    }
                });
        }
    }
}
