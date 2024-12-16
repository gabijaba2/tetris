/*++
	File: save.h

	Copyright (C) gabijaba
	2024-01-24

++*/

#ifndef SAVE_H
#define SAVE_H

void S_SaveState(const GameState* state);
void S_LoadState(const GameState* state);
void S_ReadScores();
void S_SaveScores();

#endif
