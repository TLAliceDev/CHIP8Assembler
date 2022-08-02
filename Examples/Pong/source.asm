# Constants Declaration
@PADDLE_OFFSET 0
@BALL_OFFSET 12
@SAVE_OFFSET 16
@ENEMY_OFFSET 32

CALL $LOAD_GRAPHICS
# Player And Enemy Scores
SETM 13 0
SETM 14 0
CALL $READY

$LOOP
CALL $PLAYER_CONTROL
CALL $ENEMY_CONTROL
CALL $MOVE_BALL
CALL $DRAW
JUMP $LOOP

$DRAW
CLEAR
CALL $DRAW_PADDLE_PLAYER
CALL $DRAW_PADDLE_ENEMY
CALL $DRAW_BALL
CALL $DRAW_SCORE
RETN

$PLAYER_CONTROL
# Y Position is increased by 1 if Down is pressed, Decreased by 1 if Up is pressed
KEYU 6
ADDM 1 -1
KEYU 7
ADDM 1 1
JNEM 1 0
SETM 1 1
JNEM 1 20
SETM 1 19
RETN

$ENEMY_CONTROL
SETI $DATA_START/@ENEMY_OFFSET
DUMP 0x2
SETM 2 6
RGOP 1 3 ASSIGN
RGOP 1 2 ADD
RGOP 1 5 SUB
JEQM 0xF 1
RGOP 3 8 ADD
JEQM 0xF 0
RGOP 3 8 SUB
JNEM 3 0
SETM 3 1
JNEM 3 20
SETM 3 19
SETI $DATA_START/@ENEMY_OFFSET
LOAD 0x2
RETN

$READY
# Sets the Player's position
SETM 0 1
SETM 1 10
# Sets the Enemy's Position
SETM 2 60
SETM 3 10
# Sets the Ball's Position
SETM 4 30
SETM 5 12
# Sets the Player's controls ( 6 - UP, 7 - DOWN, 8 - Velocity )
SETM 6 5
SETM 7 8
SETM 8 1
# Sets the Ball's Starting Velocity ( 9 - X direction, 10 - Y direction, 11 - Speed, Direction of 0 = Negative, Direction of 1 = Positive )
RAND 9 1
RAND 10 1
SETM 11 1
SETM 12 30
RETN

$MOVE_BALL
CALL $CHECK_BALL_COLLISIONS
JEQM 9 0
RGOP 4 11 SUB
JEQM 9 1
RGOP 4 11 ADD
JEQM 10 0
RGOP 5 11 SUB
JEQM 10 1
RGOP 5 11 ADD
JEQM 9 0
ASET 12
JEQM 9 1
ASET 12
JEQM 10 0
ASET 12
JEQM 10 1
ASET 12
RETN

$PLAYER_LOSE
ADDM 13 1
CALL $READY
RETN

$PLAYER_WIN
ADDM 14 1
CALL $READY
RETN

$CHECK_BALL_COLLISIONS
# Checks for Collisions Against Screen Edges
JNEM 4 0
CALL $PLAYER_WIN
JNEM 4 60
CALL $PLAYER_LOSE
JNEM 5 0
SETM 10 0
JNEM 5 28
SETM 10 1
RETN

$LOAD_GRAPHICS
# Load Paddle Graphics
SETM 0 0b11000000
SETM 1 0b11000000
SETM 2 0b11000000
SETM 3 0b11000000
SETM 4 0b11000000
SETM 5 0b11000000
SETM 6 0b11000000
SETM 7 0b11000000
SETM 8 0b11000000
SETM 9 0b11000000
SETM 10 0b11000000
SETM 11 0b11000000
SETM 12 0b11000000
SETI $DATA_START/@PADDLE_OFFSET
DUMP 12
# Load Ball Graphics
SETM 0 0b01100000
SETM 1 0b11110000
SETM 2 0b11110000
SETM 3 0b01100000
SETI $DATA_START/@BALL_OFFSET
DUMP 3
RETN

$DRAW_PADDLE_PLAYER
SETI $DATA_START/@PADDLE_OFFSET
DRAW 0 1 12
RETN

$DRAW_PADDLE_ENEMY
SETI $DATA_START/@PADDLE_OFFSET
DRAW 2 3 12
RETN

$DRAW_BALL
SETM 15 1
SETI $DATA_START/@BALL_OFFSET
DRAW 4 5 4
JNEM 0xF 1
RGOP 9 15 NEGATE
RETN

$DRAW_SCORE
SETI $DATA_START/@SAVE_OFFSET
DUMP 0x1
SETM 0 0
SETM 1 56
FONT 13
DRAW 0 0 5
FONT 14
DRAW 1 0 5
SETI $DATA_START/@SAVE_OFFSET
LOAD 0x1
RETN

$DATA_START
