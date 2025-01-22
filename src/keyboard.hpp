#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#elif __APPLE__
#include <SDL.h>
#endif

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
    int nKeys;
    bool* keyStates; // false for released, true for pressed
public:
    explicit KeyboardHandler(int nKeys) : nKeys(nKeys){
        keyStates = new bool[nKeys];
        for(int i = 0; i < nKeys; i++){
            keyStates[i] = false;
        }
    }
    explicit KeyboardHandler() : KeyboardHandler(DEFAULT_N_KEYS){}
    ~KeyboardHandler(){
        delete[] keyStates;
    }
    void handleEvent(const SDL_KeyboardEvent& event);
    bool getKeyState(Key key) const;
    void setKeyState(Key key, bool val);
};