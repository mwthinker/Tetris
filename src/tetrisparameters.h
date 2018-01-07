#ifndef TETRISPARAMETERS_H
#define TETRISPARAMETERS_H

// New record if these settings is used in non ai local single game.
const int TETRIS_MAX_LEVEL = 40;
const int TETRIS_WIDTH = 10;
const int TETRIS_HEIGHT = 24;
const int ROWS_TO_LEVEL_UP = 10;
const int COUNT_DOWN_TIME = 3;

enum TetrisMenu {
	MENU,
	GAME,
	CUSTOM_GAME,
	NETWORK_GAME, 
	HIGHSCORE,
	QUIT,
	ENUM_SIZE
};

#endif // TETRISPARAMETERS_H
