# Tetris

Implementation of the Tetris game in the context of the IN204 course

## Rules
- 7 tetrominoes with different colors and shapes
- Matrix of 10x22
- One tetromino at a time falls down from the top of the matrix and can be moved horizontally or rotated 
- Once the tetromino touches the ground or another piece, you have half a brief daly to move the piece before it locks down
- [TODO?] Rotating or moving the block during this half second resets the counter

### Features
- See what the next tetromino will be
- Do a hard drop : the current tetromino will fall immediatly
- Hold one tetromino for later use
- Ghost piece to see the outline of the falling tetromino

### Scoring system
- Clear lines to score points
    - 1 line = 100
    - 2 lines = 300
    - 3 lines = 500
    - 4 lines = 800
- Hard drop = 2 x distance


### Gamemodes
- Normal: Clear 40 lines with no time limit
- Classic mode: There are 15 levels and the level increases by 1 each time 10 lines are cleared, falling speed increases
- Marathon: Clear 150 lines with no time limit
- Ultra: Clear as many lines as possible within 3 minutes
- Infinite: Play as long as you want in constant speed
- Multiplayer: each player has its own grid

### Controls:
    - Left: move the tetromino left
    - Right: move the tetromino right
    - Down: hold to increase the falling speed
    - X (or Up): rotate the tetromino clockwise
    - Z : rotate counter-clockwise
    - Space : do a hard drop
    - H : hold a tetromino
    - [TODO] Enter: Pause
    - Escape (or Q): Quit the game


## Ideas
- T-Spin implementation
- Multiplayer mode with more players ?
- Mutliplayer within the same grid ?
- Dig mode: clear generated broken lines
- Battle royale mode like with Tetris Ultimate
- 3D Version like Blockout

