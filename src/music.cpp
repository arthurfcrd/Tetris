#include "music.hpp"

int main() {
    Mix_Music* menuMusic = Mix_LoadMUS("../assets/audio/musics/bp_MUS_SpaceCruise.ogg");
    Mix_Music* tetrisMusic = Mix_LoadMUS("../assets/audio/musics/original-theme.mp3");

    if (menuMusic == NULL || tetrisMusic == NULL) {
        SDL_Log("Could not load the musics: %s", Mix_GetError());
    
    
    }

    Mix_FreeMusic(menuMusic);
    Mix_FreeMusic(tetrisMusic);
    return 0;
}