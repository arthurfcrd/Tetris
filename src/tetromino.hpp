enum class TetrominoType{
    I,
    O,
    T,
    S,
    Z,
    J,
    L
};

enum class Color{
    RED,
    GREEN,
    BLUE,
    YELLOW,
    CYAN,
    MAGENTA,
    ORANGE
};

class Tetromino{
    TetrominoType type;
    int shape[4][2];
    int pos[2];
    Color color;
};