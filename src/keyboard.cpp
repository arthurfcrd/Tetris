#include "keyboard.hpp"
#include <cassert>

// updates the state of the keys according to the event received
void KeyboardHandler::handleEvent(const SDL_KeyboardEvent& event){ 
    Key key = Key::NONE;
    switch (event.keysym.sym) {
        case (SDLK_z):
            key = Key::Z;
            break;
        case (SDLK_x):
            key = Key::X;
            break;
        case (SDLK_LEFT):
            key = Key::LEFT;
            break;
        case (SDLK_RIGHT):
            key = Key::RIGHT;
            break;
        case(SDLK_DOWN):
            key = Key::DOWN;
            break;
        case (SDLK_UP):
            key = Key::UP;
            break;
        case (SDLK_SPACE):
            key = Key::SPACE;
            break;
        case (SDLK_h):
            key = Key::H;
            break;
        case (SDLK_ESCAPE):
            key = Key::ESC;
            break;
        case (SDLK_q):
            key = Key::Q;
            break;
        default:
            break;
    }
    if (key != Key::NONE){
        int keyIndex = static_cast<int>(key);
        keyStates[keyIndex] = (event.state == SDL_PRESSED);
    }
}

bool KeyboardHandler::getKeyState(Key key) const {
    return keyStates[static_cast<int>(key)];
}

void KeyboardHandler::setKeyState(Key key, bool val) {
    keyStates[static_cast<int>(key)] = val;
}