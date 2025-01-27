#pragma once
#include "point.hpp"
#include "graphics.hpp"

#include <vector>
#include <algorithm>
#include <random>
#include <string>

enum class TetrominoType : int {
    I,
    O,
    T,
    S,
    Z,
    J,
    L,
    NONE
};

class Grid;

class BaseTetromino{
protected:
    TetrominoType type;
    Point pos; // x, y position of the center of the tetromino (in grid coordinates)
    Point blocks[4]; // position of the 4 blocks of the tetromino relative to the center
    Color color;
public:
    explicit BaseTetromino(TetrominoType tetroType); 
    explicit BaseTetromino(); // creates a random tetromino
    explicit BaseTetromino(std::string serializedTetromino);
    BaseTetromino(const BaseTetromino& other);

    void drawTetromino(SDL_Renderer* renderer) const;
    void drawTetromino(SDL_Renderer* renderer, int startX, int startY) const;
    std::string serialize() const;
    void fromSerialized(std::string serializedTetromino);
    TetrominoType getType() const;
};

class Tetromino : public BaseTetromino{
private:
    int rotationIndex = 0;
    bool touchedGround = false;
    bool locked = false; // there is a delay between the tetromino touching the ground/other tetrominos and the tetromino locking
public:
    // Constructors
    explicit Tetromino(TetrominoType tetroType) : BaseTetromino(tetroType) {}; 
    explicit Tetromino() : BaseTetromino() {}; // creates a random tetromino
    Tetromino(const Tetromino& other) : BaseTetromino(other) {
        rotationIndex = other.rotationIndex;
        touchedGround = other.touchedGround;
        locked = other.locked;
    }
    Tetromino(const BaseTetromino& other) : BaseTetromino(other) {}

    // Getter and setter methods
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
    bool checkCollision(const Grid& g) const;
private:
    Point getOffsetPos(int offsetIndex, TetrominoType tetroType, int rotationIndex, int newRotationIndex);
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
        BaseTetromino nextTetromino;
        BaseTetromino heldTetromino;

        explicit TetrominoBag(): currentTetromino(TetrominoType::NONE), 
        nextTetromino(TetrominoType::NONE), heldTetromino(TetrominoType::NONE){
            createBag();
            pickTetromino();
            pickNextTetromino();
        };

        // create the bag by shuffling the seven different tetrominoes in tetroList
        void createBag();

        // pick a tetromino from the bag and stores it in currentTetromino
        void pickTetromino();

        // pick a tetromino from the bag and stores it in nextTetromino
        void pickNextTetromino();

        // place the next tetromino into current tetromino and draw the next one
        void switchTetromino();

        // hold the current tetromino
        void hold();
};
