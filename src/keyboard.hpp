#pragma once

#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#elif __APPLE__
#include <SDL.h>
#include <SDL_image.h>
#endif

#define DEFAULT_N_KEYS (10)

enum class Key : int {
    Z,
    X,
    LEFT,
    RIGHT,
    DOWN,
    UP,
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
    void handleEvent(SDL_KeyboardEvent event);
    friend class Game;
};