/*++
	File: game.h

	Copyright (C) gabijaba
	2023-12-08

	Main header file for the game

++*/

#ifndef GAME_H
#define GAME_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <time.h>
#include <SDL.h>
#include <SDL_ttf.h>

#include "strings.h"

/* tetromino orientation data IDs */
#define I_HORIZ		0
#define I_VERT 		1
#define O_	   		2
#define L_DOWN 		3
#define L_RIGHT		4
#define L_LEFT 		5
#define L_UP   		6
#define T_DOWN 		7
#define T_UP   		8
#define T_RIGHT		9
#define T_LEFT 		10
#define J_DOWN 		11
#define J_LEFT 		12
#define J_UP   		13
#define J_RIGHT		14
#define Z_HORIZ		15
#define Z_VERT 		16
#define S_HORIZ		17
#define S_VERT 		18

/* Offsets to default piece spawn orientation data */
#define T_SPAWN		7
#define J_SPAWN		11
#define Z_SPAWN		15
#define O_SPAWN		2
#define S_SPAWN		17
#define L_SPAWN		3
#define I_SPAWN		0

#define BOARD_HEIGHT 20
#define BOARD_WIDTH 10

#define SAVE_FILE "save.bin"
#define LOG_FILE "log.txt"

#if defined(_WIN64) || defined(_WIN32) 
#define FONT_FILE					"C:\\Windows\\Fonts\\Arial.ttf"
#endif

typedef struct _Score {
	char *name;
	unsigned int score;
	struct _Score* next;
	struct _Score* prev;
} Score;

typedef struct _GameState {
	int x;
	int y;
	int dropCounter;
	int currentTetrominoOrientation;
	uint32_t currentTetrominoColor;
	int points;
	uint32_t boardState[20][10];
} GameState;

#include "save.h"

extern char orientationTable[19][4][2];

extern SDL_Window* gWindow;

extern Score scoreListHead;
extern int scoreCount;

void Game_Main();
void Game_Error(const char* msg);
bool Game_ValidateOrientationAtPosition(const unsigned int x, const unsigned int y, const int orientationID);

void R_Init();
void R_DrawFrame(const GameState* state, bool *drawSaveNotice, bool *drawLoadNotice);
void R_DrawRedText(const char *msg);
void R_DrawYellowText(const char* msg);
void R_DrawNameBuffer(const char *name, const int len);
void R_DrawHighscores();

void Log_Init();
void Log_Write(const char* str, ...);
void Log_Close();

void Game_AddScore(const char* name, const unsigned int score);
void Game_SortScores();

void test1();
void test2();
void test3();

#endif
