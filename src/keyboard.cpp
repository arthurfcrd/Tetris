#include "keyboard.hpp"

void KeyboardHandler::handleEvent(const SDL_KeyboardEvent& event){ // updates the state of the keys according to the event received
    Key key = Key::NONE;
    switch (event.keysym.sym) {
        case (SDLK_ESCAPE):
            key = Key::ESC;
            break;
        case (SDLK_UP):
            key = Key::UP;
            break;
        case (SDLK_RIGHT):
            key = Key::RIGHT;
            break;
        case(SDLK_DOWN):
            key = Key::DOWN;
            break;
        case (SDLK_LEFT):
            key = Key::LEFT;
            break;
        case (SDLK_z):
            key = Key::Z;
            break;
        case (SDLK_x):
            key = Key::X;
            break;
        default:
            break;
    }
    if (key != Key::NONE){
        keyStates[static_cast<int>(key)] = (event.state == SDL_PRESSED);
    }
}