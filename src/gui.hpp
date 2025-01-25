#include "grid.hpp"
#include "graphics.hpp"

#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>

class Tetromino; // forward declaration


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

        void draw(int y, int width);
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
    public:
        explicit BaseUI(SDL_Renderer* r, std::string firstTitle, std::vector<std::string> buttonsText);
        void drawBackground();
        void drawTitle();
        void drawButtons();
        void drawUI();
        std::string getChoice(SDL_Event& event);
        ~BaseUI();
};


class HUD {
    private:
        Grid* nextBox;
        Grid* holdBox;
        unsigned int score;
        unsigned int nLinesCleared;
    public:
        HUD();
        unsigned int getScore() const;
        void setScore(int newScore);

        unsigned int getLinesCleared() const;
        void setLinesCleared(int newVal);

        void insertIntoBox(Grid* box, Tetromino& tetro);
        void drawHUD(SDL_Renderer* renderer, Tetromino nextTetro, Tetromino holdTetro);
        ~HUD();
};