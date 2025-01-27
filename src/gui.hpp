#include <string>
#include <vector>
#include <chrono>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>

#include "grid.hpp"
#include "graphics.hpp"
#include "grid.hpp"

class Tetromino; // forward declaration

#define BUTTON_TO_PADDING_RATIO (0.95)
#define FONTSIZE_TO_BUTTON_RATIO (0.5)

#define MAX_LEVEL (15)
#define STARTING_FALL_RATE (0.7)
#define FALL_RATE_DECREASE (0.15)
#define LINES_PER_LEVEL (4)


SDL_Texture* createTextureFromIMG(SDL_Renderer* renderer, std::string path);

class Button {
    private:
        SDL_Renderer* renderer;
        SDL_Texture* btnText;
        SDL_Texture* hgBtnText; // highlighting the button when hovering over it
        bool isHighlighted;
    public:
        SDL_Rect btnRect;
        std::string text;
        explicit Button(SDL_Renderer* r, std::string textVal, std::string normalBtnFile,
             std::string hgBtnFile);
        explicit Button(SDL_Renderer* r, std::string textVal) : 
            Button(r, textVal,
            "../assets/img/menus/main_FTL2_hyperspace_flat.png",
             "../assets/img/menus/main_FTL2_hyperspace_blank.png") {}
        bool getHighlighted() const;
        void setHighlighted(bool newVal);

        void draw(int y, int width, int height);
        void drawButtonText();
};

class BaseUI {
    private:
        SDL_Renderer* renderer;
        int windowWidth;
        int windowHeight;

        SDL_Texture* background;
        SDL_Rect bgRect;

        std::string title;
        unsigned titlePaddding = 30;
        unsigned titleBottom;
        std::vector<Button> buttons;

        Mix_Chunk* hoverSound;
        Mix_Chunk* selectSound;
    public:
        explicit BaseUI(SDL_Renderer* r, std::string firstTitle, std::vector<std::string> buttonsText);
        void drawBackground();
        void drawTitle();
        void drawButtons();
        void drawUI();
        std::string getChoice(SDL_Event& event);
        ~BaseUI();
};

class InfoUI {
    SDL_Renderer* renderer_;
    int windowWidth_;
    int windowHeight_;

    SDL_Texture* background_;
    SDL_Rect bgRect_;

    std::string title_;
    std::string text_;

    public:
        InfoUI(SDL_Renderer* renderer, std::string title, std::string text);
        void showInfo();
};

enum class GameType : int {
    TIME_BASED, LINES_BASED, CLASSIC, INFINITE, MULTIPLAYER
};


class HUD {
private:
    Grid* nextBox;
    Grid* holdBox;

    int leftGridX;
    int leftGridY;
    int rightGridX;
    int rightGridY;

    Mix_Chunk* levelUpSound;
    
    GameType gameType;
    int score;
    int enemyScore;
    int nLinesCleared;
    int nLinesToClear;
    std::chrono::time_point<std::chrono::system_clock> gameChrono;
    double timeToClear; // number of seconds the player has in time-based gamemodes

    int currentLevel;
    double fallRate;
public:
    HUD(GameType gt, int nltc, int ttc);
    HUD(GameType gt) : HUD(gt, 0, 0) {
        if (gt == GameType::LINES_BASED)
            nLinesToClear = 10;
        else if (gt == GameType::TIME_BASED)
            timeToClear = 3 * 60;
        else if (gt == GameType::MULTIPLAYER){}
            // TODO
    }
    ~HUD();

    int getScore() const;
    void setScore(int newScore);
    void setEnemyScore(int newScore);
    int getLinesCleared() const;
    void setLinesCleared(int newVal);
    int getLinesToClear() const;
    GameType getGameType() const;
    int getCurrentLevel() const;
    double getFallRate() const;
    void setFallRate(double newVal);

    void increaseLevel();
    void updateLevel();

    void insertIntoBox(Grid* box, Tetromino& tetro) const;
    double getTimeLeft() const;
    void drawHUD(SDL_Renderer* renderer, Tetromino nextTetro, Tetromino holdTetro);
};