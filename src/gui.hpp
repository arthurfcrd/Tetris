#include "grid.hpp"
#include "graphics.hpp"

class Tetromino; // forward declaration

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