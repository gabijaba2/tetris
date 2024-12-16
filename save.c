/*++
	File: save.c

	Copyright (C) gabijaba
	2023-12-08

	Game state/score saving, loading functionality

++*/


#include "game.h"

/*
	Save file format:

	4 bytes - x
	4 bytes - y
	4 bytes - dropCounter
	4 bytes - currentTetrominoOrientation
	4 bytes - currentTetrominoColor
	4 bytes - points
	800 bytes - boardState
	1 byte - scoreCount
	scoreCount*sizeof(ScoreEntry) bytes - score entries

	score entry:
	4 bytes - nameLenght
	nameLenght bytes - name
	4 bytes - score


*/

#define FILE_SCORE_OFFSET 0x338

/* Saves game state to save file */
void S_SaveState(const GameState *state) 
{
	FILE* saveFile = NULL;
	int fileSize = 0;

	saveFile = fopen(SAVE_FILE, "rb");
	if (saveFile) {
		fseek(saveFile, 0, SEEK_END);
		fileSize = ftell(saveFile);

		void* fileTemp = malloc(fileSize);
		if (!fileTemp) {
			Game_Error(STR_ERROR_MEM);
		}
		memset(fileTemp, 0x0, fileSize);

		fseek(saveFile, 0, SEEK_SET);
		fread(fileTemp, fileSize, 1, saveFile);

		fclose(saveFile);
		saveFile = fopen(SAVE_FILE, "wb");
		if (!saveFile) {
			Game_Error(STR_ERROR_DATA_FILE);
		}

		fwrite(fileTemp, fileSize, 1, saveFile);
	}
	else {
		saveFile = fopen(SAVE_FILE, "wb");
		if (!saveFile) {
			Game_Error(STR_ERROR_DATA_FILE);
		}
	}

	fseek(saveFile, 0, SEEK_SET);
	fwrite(&state->x, 1, 4, saveFile);
	fwrite(&state->y, 1, 4, saveFile);
	fwrite(&state->dropCounter, 1, 4, saveFile);
	fwrite(&state->currentTetrominoOrientation, 1, 4, saveFile);
	fwrite(&state->currentTetrominoColor, 1, 4, saveFile);
	fwrite(&state->points, 1, 4, saveFile);

	fwrite(&state->boardState, 1, 800, saveFile);
	

	fclose(saveFile);

}

/* Loads game state from save file */
void S_LoadState(const GameState* state)
{
	FILE* saveFile = fopen(SAVE_FILE, "rb");

	if (!saveFile) {
		return;
	}

	fread(&state->x, 1, 4, saveFile);
	fread(&state->y, 1, 4, saveFile);
	fread(&state->dropCounter, 1, 4, saveFile);
	fread(&state->currentTetrominoOrientation, 1, 4, saveFile);
	fread(&state->currentTetrominoColor, 1, 4, saveFile);
	fread(&state->points, 1, 4, saveFile);

	fread(&state->boardState, 1, 800, saveFile);

	fclose(saveFile);
	
}

/* Reads scores from save file */
void S_ReadScores() {
	FILE* saveFile = fopen(SAVE_FILE, "rb");
	int count = 0;

	if (!saveFile) {
		return;
	}

	fseek(saveFile, FILE_SCORE_OFFSET, SEEK_SET);
	fread(&count, 1, 1, saveFile);

	for (int i = 0; i < count; i++) {;
		int score = 0;
		int nameLen = 0;
		char* name = NULL;

		fread(&nameLen, 1, 4, saveFile);
		assert(nameLen > 0 || nameLen < 16);

		name = malloc(nameLen + 1);
		if (!name) {
			Game_Error(STR_ERROR_MEM);
		}
		memset(name, 0, nameLen + 1);

		fread(name, 1, nameLen, saveFile);
		fread(&score, 1, 4, saveFile);

		Game_AddScore(name, score);
	}
	
	fclose(saveFile);
}

/* Writes scores to save file */
void S_SaveScores() {
	FILE* saveFile = NULL;
	Score* entry = scoreListHead.next;
	int fileSize = 0;

	if (!scoreCount)
		return;

	saveFile = fopen(SAVE_FILE, "rb");
	if (saveFile) {
		fseek(saveFile, 0, SEEK_END);

		fileSize = ftell(saveFile);
		void* fileTemp = malloc(FILE_SCORE_OFFSET);
		if (!fileTemp) {
			Game_Error(STR_ERROR_MEM);
		}
		memset(fileTemp, 0x0, FILE_SCORE_OFFSET);

		fseek(saveFile, 0, SEEK_SET);
		fread(fileTemp, 1, FILE_SCORE_OFFSET, saveFile);

		fclose(saveFile);
		saveFile = fopen(SAVE_FILE, "wb");
	
		fwrite(fileTemp, FILE_SCORE_OFFSET, 1, saveFile);

	}
	else {
		saveFile = fopen(SAVE_FILE, "wb");
		if (!saveFile) {
			Game_Error(STR_ERROR_DATA_FILE);
		}
	}

	fseek(saveFile, FILE_SCORE_OFFSET, SEEK_SET);

	fwrite(&scoreCount, 1, 1, saveFile);

	for (int i = 0; i < scoreCount; i++) {
		uint32_t strLen = strlen(entry->name);
		fwrite(&strLen, 1, 4, saveFile);
		fwrite(entry->name, 1, strLen, saveFile);
		fwrite(&entry->score, 1, 4, saveFile);
		entry = entry->next;
	}

	fclose(saveFile);
	
}