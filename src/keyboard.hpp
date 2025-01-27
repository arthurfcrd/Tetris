#pragma once

#include <SDL2/SDL.h>

#include <vector>

#define DEFAULT_N_KEYS (15)

enum class Key : int {
    Z,      // rotate counter-clockwise
    X,      // rotate clockwise
    LEFT,   // move to the left
    RIGHT,  // move to the right
    DOWN,   // accelerates fall
    UP,     // rotate clockwise
    SPACE,  // hard drop
    H,      // hold key
    ESC,    // quit
    Q,      // quit
    NONE
};

class KeyboardHandler{
private:
    std::vector<bool> keyStates;
public:
    explicit KeyboardHandler(int nKeys) : keyStates(nKeys, false){}
    explicit KeyboardHandler() : KeyboardHandler(DEFAULT_N_KEYS){}
    void handleEvent(const SDL_KeyboardEvent& event);
    bool getKeyState(Key key) const;
    void setKeyState(Key key, bool val);
};