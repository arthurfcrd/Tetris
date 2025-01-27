#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>

#include <string>

class Music {
    private:
        Mix_Music* musicFile;
        int volume;
    public:
        Music(std::string path, int volume);
        Music(std::string path);
        ~Music();

        void playOnce() const;
        void playOnLoop() const;
        void stop() const;
};
