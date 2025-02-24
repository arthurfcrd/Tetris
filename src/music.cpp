#include "music.hpp"

Music::Music(std::string path, int volumeVal) {
    musicFile = Mix_LoadMUS(path.c_str());
    if (musicFile == NULL)
        SDL_Log("Could not load the musics: %s", Mix_GetError());
    volume = volumeVal;
}

Music::Music(std::string path) : Music(path, MIX_MAX_VOLUME) {}

Music::~Music() {
    Mix_FreeMusic(musicFile);
}

void Music::playOnLoop() const {
    Mix_VolumeMusic(volume);
    Mix_PlayMusic(musicFile, -1);
}

void Music::playOnce() const {
    Mix_VolumeMusic(volume);
    Mix_PlayMusic(musicFile, 0);
}

void Music::stop() const {
    Mix_PauseMusic();
    Mix_VolumeMusic(MIX_MAX_VOLUME);
}
