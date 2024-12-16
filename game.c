/*++
	File: game.c

	Copyright (C) gabijaba
	2023-12-08

	Game loop, logic functionality

++*/

#include "game.h"

enum keys {
	Key_Left,
	Key_Right,
	Key_Down,
	Key_Z,
	Key_X,
	Key_F5,
	Key_F6,
	Key_Esc,
	Key_R
};

#define RED 0xff0303e4 
#define ORANGE 0xff008cff 
#define YELLOW 0xff00edff 
#define GREEN 0xff288000
#define BLUE 0xff8E4024 
#define PURPLE 0xff822973 

/* Table of all possible orientations for all terominos, relative coordinates of every tile to the position of the tetromino, ((x,y)(x,y)(x,y)(x,y)) */
char orientationTable[19][4][2] = {
{ { -2,  0 }, { -1,  0 }, {  0,  0 }, {  1,  0 } }, /* I hs */
{ {  0, -2 }, {  0, -1 }, {  0,  0 }, {  0,  1 } }, /* I v */

{ { -1,  0 }, {  0,  0 }, { -1,  1 }, {  0,  1 } }, /* O s */

{ { -1,  0 }, {  0,  0 }, {  1,  0 }, { -1,  1 } }, /* L ds */
{ {  0, -1 }, {  0,  0 }, {  0,  1 }, {  1,  1 } }, /* L r */
{ { -1, -1 }, {  0, -1 }, {  0,  0 }, {  0,  1 } }, /* L l */
{ {  1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 } }, /* L u */

{ { -1,  0 }, {  0,  0 }, {  1,  0 }, {  0,  1 } }, /* T ds */
{ { -1,  0 }, {  0,  0 }, {  1,  0 }, {  0, -1 } }, /* T u */
{ {  0, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 } }, /* T r */
{ {  0, -1 }, { -1,  0 }, {  0,  0 }, {  0,  1 } }, /* T l */

{ { -1,  0 }, {  0,  0 }, {  1,  0 }, {  1,  1 } }, /* J ds*/
{ {  0, -1 }, {  0,  0 }, { -1,  1 }, {  0,  1 } }, /* J l*/
{ { -1, -1 }, { -1,  0 }, {  0,  0 }, {  1,  0 } }, /* J u*/
{ {  0, -1 }, {  1, -1 }, {  0,  0 }, {  0,  1 } }, /* J r*/

{ { -1,  0 }, {  0,  0 }, {  0,  1 }, {  1,  1 } }, /* Z hs */
{ {  1, -1 }, {  0,  0 }, {  1,  0 }, {  0,  1 } }, /* Z v */

{ {  0,  0 }, {  1,  0 }, { -1,  1 }, {  0,  1 } }, /* S hs */
{ {  0, -1 }, {  0,  0 }, {  1,  0 }, {  1,  1 } }  /* S v */

};

/* Maps [orientation][0] to counter clockwise rotated orientation and [orientation][1] to clockwise rotated orientation */
char rotationMap[19][2] = {
{ I_VERT	, I_VERT 	},
{ I_HORIZ	, I_HORIZ 	},
{ O_		, O_		},
{ L_RIGHT	, L_LEFT	},
{ L_UP		, L_DOWN	},
{ L_DOWN	, L_UP		},
{ L_LEFT	, L_RIGHT	},
{ T_RIGHT	, T_LEFT	},
{ T_LEFT	, T_RIGHT 	},
{ T_UP  	, T_DOWN 	},
{ T_DOWN	, T_UP		},
{ J_RIGHT	, J_LEFT	},
{ J_DOWN	, J_UP		},
{ J_LEFT	, J_RIGHT	},
{ J_UP		, J_DOWN	},
{ Z_VERT	, Z_HORIZ	},
{ Z_HORIZ	, Z_VERT	},
{ S_VERT	, S_VERT	},
{ S_HORIZ	, S_HORIZ	}
};


/* Maps 0-7 from rand() to the spawn orientation of a tetromino */
char spawnMap[7] = {
T_SPAWN,
J_SPAWN,
Z_SPAWN,
O_SPAWN,
S_SPAWN,
L_SPAWN,
I_SPAWN
};

/* Maps 0-6 from rand() to colors */
uint32_t colorMap[6] = {
RED,
ORANGE,
YELLOW,
GREEN,
BLUE,
PURPLE
};

GameState state = { 5,2,0,2,RED,0 };

bool shouldQuit = false;
bool shouldGetText = false;
bool drawHighscores = false;
bool shouldPause = false;
bool drawSaveNotice = false;
bool drawLoadNotice = false;

uint8_t keyState[9] = {0}; /* 0 - down, 1 - up*/

int nameInputLen = 0;
char nameInputBuf[15] = {0};

int drawHighscoreCounter = 0;

void Game_Error(const char* msg) {
	if (msg)
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, STR_ERROR_GAME, msg, gWindow);

	exit(1);
}

void Game_Reset() {
	state.x = 5;
	state.y = 2;
	state.dropCounter = 0;
	state.currentTetrominoOrientation = 2;
	state.currentTetrominoColor = RED;
	state.points = 0;
	memset(state.boardState, 0, BOARD_HEIGHT * BOARD_WIDTH * sizeof(uint32_t));
}

void Game_ProcessEvent(const SDL_Event* e) {
	switch (e->type) {
	case SDL_KEYDOWN:
	case SDL_KEYUP:
		switch (e->key.keysym.sym) {
		case SDLK_LEFT:
			keyState[Key_Left] = e->key.state;
			break;
		case SDLK_RIGHT:
			keyState[Key_Right] = e->key.state;
			break;
		case SDLK_DOWN:
			keyState[Key_Down] = e->key.state;
			break;
		case SDLK_z:
			keyState[Key_Z] = e->key.state;
			break;
		case SDLK_x:
			keyState[Key_X] = e->key.state;
			break;
		case SDLK_F5:
			keyState[Key_F5] = e->key.state;
			break;
		case SDLK_F6:
			keyState[Key_F6] = e->key.state;
			break;
		case SDLK_ESCAPE:
			keyState[Key_Esc] = e->key.state;
			break;
		case SDLK_r:
			keyState[Key_R] = e->key.state;
			break;
		case SDLK_RETURN:
			if (shouldGetText) {
				Game_AddScore(nameInputBuf, state.points);
				Game_SortScores();
				shouldGetText = false;
				drawHighscores = true;
				drawHighscoreCounter = 0;
			} else if (drawHighscores) {
				if (drawHighscoreCounter > 10) {
					drawHighscores = false;
					Game_Reset();
				}
			}
			break;
		case SDLK_BACKSPACE:
			if (shouldGetText) {
				if (nameInputLen) {
					nameInputBuf[nameInputLen - 1] = 0;
					nameInputLen--;
				}
			}
			break;
		default:
			break;
		}
		break;
	case SDL_TEXTINPUT:
		if (shouldGetText) {
			if (nameInputLen < 15) {
				strcat(nameInputBuf, e->text.text);
				nameInputLen += (int)strlen(e->text.text);
			}
		}
		break;
	case SDL_QUIT:
		shouldQuit = true;
		break;
	}
}

inline int rotateClockwise(const int id) {
	return rotationMap[id][1];
}

inline int rotateAntiClockwise(const int id) {
	return rotationMap[id][0];
}

/* Validates if a specific tetromino orientation can exist at coordinates */
bool Game_ValidateOrientationAtPosition(const unsigned int x, const unsigned int y, const int orientationID) {

	for (int i = 0; i <= 3; i++) {

		/* If a tile goes out of bounds of the board, orientation is invalid */
		if ((x + orientationTable[orientationID][i][0]) > BOARD_WIDTH - 1 || (y + orientationTable[orientationID][i][1]) > BOARD_HEIGHT - 1) {
			return false;
		}

		// if a tile already exists at the location that we need, the orientation is invalid
		if (state.boardState[y + orientationTable[orientationID][i][1]][x + orientationTable[orientationID][i][0]])
			return false;

	}

	// if all new pieces fit in empty places, orientation is valid
	return true;
}

void Game_WriteCurrentTetrominoToBoard() {
	for (int i = 0; i <= 3; i++) {
		state.boardState[state.y + orientationTable[state.currentTetrominoOrientation][i][1]][state.x + orientationTable[state.currentTetrominoOrientation][i][0]] = state.currentTetrominoColor;
	}
}

/* Generates next tetromino */
void Game_SpawnNewTetromino() {
	srand((unsigned int)time(0));

	state.x = 5;
	state.y = 2;
	state.dropCounter = 0;
	state.currentTetrominoOrientation = spawnMap[rand() % 7];
	state.currentTetrominoColor = colorMap[rand() % 6];
}

void Game_ClearFullRows() {
	for (int i = 0; i <= BOARD_HEIGHT - 1; i++) {
		bool isFullRow = true;
		for (int ii = 0; ii <= BOARD_WIDTH - 1; ii++) {
			if (!state.boardState[i][ii])
				isFullRow = false;
		}

		if (isFullRow) {
			/* Clear out the full row */

			state.points += 50;

			memset((void*)((uintptr_t)state.boardState + (i * (sizeof(uint32_t) * BOARD_WIDTH))), 0, sizeof(uint32_t) * BOARD_WIDTH);

			/* For every row above the destroyed row - shift it down and clear it */
			for (int ii = i - 1; ii >= 0; ii--) {
				memcpy((void*)((uintptr_t)state.boardState + ((ii + 1) * (sizeof(uint32_t) * BOARD_WIDTH))), (void*)((uintptr_t)state.boardState + ((ii) * (sizeof(uint32_t) * BOARD_WIDTH))), sizeof(uint32_t) * BOARD_WIDTH);
				memset((void*)((uintptr_t)state.boardState + (ii * (sizeof(uint32_t) * BOARD_WIDTH))), 0, sizeof(uint32_t) * BOARD_WIDTH);
			}
		}
	}
}

/* Processes all keyboard input, return value: true - action requires skipping current frame, false - don't do anything */
bool Game_ProcessKeyInput() {
	if (keyState[Key_Left]) {
		if (Game_ValidateOrientationAtPosition(state.x - 1, state.y, state.currentTetrominoOrientation))
			state.x--;
	}

	if (keyState[Key_Right]) {
		if (Game_ValidateOrientationAtPosition(state.x + 1, state.y, state.currentTetrominoOrientation))
			state.x++;
	}

	if (keyState[Key_Down]) {
		if (Game_ValidateOrientationAtPosition(state.x, state.y + 1, state.currentTetrominoOrientation))
			state.y++;
	}

	if (keyState[Key_Z]) {
		if (Game_ValidateOrientationAtPosition(state.x, state.y, rotateAntiClockwise(state.currentTetrominoOrientation)))
			state.currentTetrominoOrientation = rotateAntiClockwise(state.currentTetrominoOrientation);
	}

	if (keyState[Key_X]) {
		if (Game_ValidateOrientationAtPosition(state.x, state.y, rotateClockwise(state.currentTetrominoOrientation)))
			state.currentTetrominoOrientation = rotateClockwise(state.currentTetrominoOrientation);
	}

	if (keyState[Key_F5]) {
		S_SaveState(&state);
		drawSaveNotice = true;
	}
	if (keyState[Key_F6]) {
		S_LoadState(&state);
		drawLoadNotice = true;
		return true;
	}

	if (keyState[Key_Esc]) {
		if (shouldPause) {
			shouldPause = false;
		}
		else {
			shouldPause = true;
			R_DrawRedText(STR_PAUSE_NOTICE);
		}
	}

	if (keyState[Key_R]) {
		Game_Reset();
		return true;
	}

	return false;
}

void Game_Main() {
	uint64_t curTime = 0;
	uint64_t prevTime = 0;

	/* Initialization */
	Log_Init();
	Log_Write(STR_LOG_START);
	S_ReadScores();
	R_Init();

	SDL_StopTextInput();

	/* Game loop */
	while (!shouldQuit) {
		SDL_Event event;

		/* Limit to 60 fps */
		prevTime = curTime;
		curTime = SDL_GetTicks64();

		if ((curTime - prevTime) < 16666) {
			while (SDL_PollEvent(&event)) {
				Game_ProcessEvent(&event);
			}
			SDL_Delay((uint32_t)( (100000 - (curTime - prevTime)) / 1000) );
		}

		if (!shouldGetText && !drawHighscores) {
			/* Process all movement */
			if (Game_ProcessKeyInput())
				continue;
		}

		/* If paused do not run game logic or draw frames */
		if (!shouldPause) {
			if (!shouldGetText && !drawHighscores) {
				/* Drop 1 row down */
				if (state.dropCounter == 5) {
					if (Game_ValidateOrientationAtPosition(state.x, state.y + 1, state.currentTetrominoOrientation)) {
						state.y++;
					}
					else {
						/* Can't drop piece any lower, it's on an another piece */
						/* Make current location of game piece solid */
						Game_WriteCurrentTetrominoToBoard();

						/* Spawn new piece */
						Game_SpawnNewTetromino();

						/* If can't spawn new piece - game can't continue, board is full */
						if (!Game_ValidateOrientationAtPosition(state.x, state.y, state.currentTetrominoOrientation)) {
							SDL_StartTextInput();
							shouldGetText = true;
							continue;
						}
					}
					state.dropCounter = 0;
				}
				else {
					state.dropCounter++;
				}

				Game_ClearFullRows();
			
				/* Draw frame */
				R_DrawFrame(&state, &drawSaveNotice, &drawLoadNotice);
			}

			if (shouldGetText) {
					R_DrawNameBuffer(nameInputBuf, nameInputLen);
			}

			if (drawHighscores) {
					R_DrawHighscores();
					drawHighscoreCounter++;
			}

			/* Clear all input events for current frame */
			while (SDL_PollEvent(&event)) {
				Game_ProcessEvent(&event);
			}
		}
	}

	S_SaveScores();
}