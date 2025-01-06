# Tetris

Implementation of the Tetris game in the context of the IN204 course

## Rules
- Matrix of 10x22
- 7 tetrominos with different colors and shapes
- One tetromino at a time falls down from the top of the matrix and can be moved horizontally or rotated 
- Once the tetromino touches the ground or another piece, you have half a second to move the piece before it locks down
- ?? Rotating or moving the block during this half second resets the counter
- There are 15 levels and the level increases by 1 when each time 10 lines are cleared
- As the level get more difficult, the falling speed of the tetrominos increases

### Features
- See what the next tetromino will be
- Do a hard drop : the current tetromino will fall immediatly
- Swap current tetromino with the next
- Hold one tetromino for later use
- Perform a T-Spin
- Add a Ghost piece to see the outline of the falling tetromino

### Scoring system
- Clear lines to score points
    - 1 line = 40
    - 2 lines = 100
    - 3 lines = 300
    - 4 lines = 1200
- Hard drop = 2 x distance
- T-spin


### Gamemodes
- Marathon: Clear 150 lines with no time limit
- Ultra: Clear as many lines as possible within 3 minutes
- 1v1 multiplayer: each player has its own grid

### Controls:
    - Left: move the tetromino left
    - Right: move the tetromino right
    - Up: rotate the tetromino clockwise
    - Down: hold to increase the falling speed
    - ?? : rotate counter-clockwise
    - Space : do a hard drop
    - H : hold a tetromino
    - ?? : swap current and next tetrominos
    - Enter: Pause


## Ideas
- Multiplayer mode with more players ?
- Mutliplayer within the same grid ?
- Dig mode: clear generated broken lines
- Battle royale mode like with Tetris Ultimate
- 3D Version like Blockout

