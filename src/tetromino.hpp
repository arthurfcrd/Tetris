#pragma once
#include "point.hpp"
#include "graphics.hpp"

#include <vector>
#include <algorithm>
#include <random>

enum class TetrominoType : int {
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

class Grid;

class Tetromino{
private:
    TetrominoType type;
    Point pos; // x, y position of the center of the tetromino (in grid coordinates)
    Point blocks[4]; // position of the 4 blocks of the tetromino relative to the center
    Color color;
    int rotationIndex = 0;
    bool touchedGround = false;
    bool locked = false; // there is a delay between the tetromino touching the ground/other tetrominos and the tetromino locking
public:
    // Constructors
    explicit Tetromino(TetrominoType type);
    explicit Tetromino(); // creates a random tetromino
    Tetromino(const Tetromino& other);

    // Getter and setter methods
    TetrominoType getType() const;
    int getPosX() const;
    int getPosY() const;
    void setPos(int x, int y);
    bool isLocked() const;
    bool hasTouchedGround() const;
    void setLocked(bool newVal);
    void setTouchedGround(bool newVal);

    // Other methods
    void move(const Grid&, int dx, int dy);
    void rotate(const Grid&, int direction); // direction = 1 for clockwise, -1 for counterclockwise
    void applyOffsetI(int rotationIndex, int newRotationIndex);
    bool checkCollision(const Grid& g) const;
    void drawTetromino(SDL_Renderer* renderer) const;
    void drawCenter(SDL_Renderer* renderer) const;
    friend class Grid;
};


// Implementation of the Random Generator used by Tetris games
// A bag containing all seven tetrominoes is shuffled and each tetromino
// is drawn one by one
class TetrominoBag {
    private:
        std::vector<TetrominoType> tetroList;
    public:
        Tetromino currentTetromino;
        Tetromino nextTetromino;
        Tetromino holdTetromino;

        explicit TetrominoBag() {
            createBag();
            drawTetromino();
            drawNextTetromino();
        };

        // create the bag by shuffling the seven different tetrominoes in tetroList
        void createBag();

        // draw a tetromino from the bag and stores it in currentTetromino
        void drawTetromino();

        // draw a tetromino from the bag and stores it in nextTetromino
        void drawNextTetromino();

        // place the next tetromino into current tetromino and draw the next one
        void switchTetromino();
};
