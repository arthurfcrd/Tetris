#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#elif __APPLE__
#include <SDL.h>
#endif

#define DEFAULT_N_KEYS (10)

enum class Key : int {
    Z, // rotate counter-clockwise
    X, // rotate clockwise
    LEFT,
    RIGHT,
    DOWN,
    UP, // rotate clockwise
    ESC,
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
    //friend class Game;
};