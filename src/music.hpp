#ifdef __linux__
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#elif __APPLE__
#include <SDL.h>
#include <SDL_mixer.h>
#endif

#include <string>

class Music {
    private:
        Mix_Music* musicFile;
        int volume;
    public:
        Music(std::string path, int volume);
        Music(std::string path);
        ~Music();

        void playOnLoop();
        void stop();
};
