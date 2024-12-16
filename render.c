/*++
	File: render.c

	Copyright (C) gabijaba
	2023-12-08

	Renderer functionality

++*/

#include "game.h"

SDL_Window* gWindow = NULL;
SDL_Renderer* gRenderer = NULL;
TTF_Font* font = NULL;
TTF_Font* largerFont = NULL;
SDL_Surface* messageSurf[10] = { 0 };

void R_Init() {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		Game_Error(STR_ERROR_SDLINIT);
	}

	if (TTF_Init()) {
		Game_Error(STR_ERROR_SDLINIT);
	}

	gWindow = SDL_CreateWindow(STR_WND_TITLE, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 500, 520, 0);
	if (!gWindow) {
		Game_Error(STR_ERROR_WND);
	}

	gRenderer = SDL_CreateRenderer(gWindow, -1, 0);
	if (!gRenderer) {
		Game_Error(STR_ERROR_RENDERER);
	}

	font = TTF_OpenFont(FONT_FILE, 12);
	largerFont = TTF_OpenFont(FONT_FILE, 25);
	if (!font || !largerFont) {
		Game_Error(STR_ERROR_FONT);
	}

	SDL_Color whiteColor = { 255,255,255,255 };
	messageSurf[0] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_Z, whiteColor);
	messageSurf[1] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_X, whiteColor);
	messageSurf[2] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_LEFT, whiteColor);
	messageSurf[3] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_RIGHT, whiteColor);
	messageSurf[4] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_DOWN, whiteColor);

	messageSurf[5] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_F5, whiteColor);
	messageSurf[6] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_F6, whiteColor);
	messageSurf[7] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_ESC, whiteColor);
	messageSurf[8] = TTF_RenderUTF8_Blended(font, STR_INSTRUCTION_R, whiteColor);
	messageSurf[9] = TTF_RenderUTF8_Blended(largerFont, STR_POINTS, whiteColor);

}

/* Draws a tile on the screen in the playing field */
void R_DrawTileInField(const int x, const int y, const uint32_t color) {

	SDL_SetRenderDrawColor(gRenderer, color & 0xff, (color & 0xff00) >> 8, (color & 0xff0000) >> 16, (color & 0xff000000) >> 24);
	SDL_Rect sqr = { 0 };
	sqr.h = 25;
	sqr.w = 25;
	sqr.x = 11 + (x * 25);
	sqr.y = 11 + (y * 25);

	SDL_RenderFillRect(gRenderer, &sqr);

	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);
	SDL_Rect sqr1 = { 0 };
	sqr1.h = 25;
	sqr1.w = 25;
	sqr1.x = 11 + (x * 25);
	sqr1.y = 11 + (y * 25);

	SDL_RenderDrawRect(gRenderer, &sqr1);

}
/* Draws a text surface on screen */
void R_DrawTextTexture(const SDL_Surface* surf, const int x, const int y, const bool freeSurf) {
	SDL_Texture *tex = SDL_CreateTextureFromSurface(gRenderer,surf);

	SDL_Rect a;
	a.x = x;
	a.y = y;
	a.h = surf->h;
	a.w = surf->w;

	SDL_RenderCopy(gRenderer, tex, NULL, &a);
	if(freeSurf) 
		SDL_FreeSurface(surf);

	SDL_DestroyTexture(tex);

}

/* Draws a red notice */
void R_DrawRedText(const char *msg) {
	SDL_Color textColor = { 255,0,0,255 };

	R_DrawTextTexture(TTF_RenderUTF8_Blended(font, msg, textColor), 275, 10 + (4 * 13) + 300, true);

	SDL_RenderPresent(gRenderer);
}

/* Draws a yellow notice */
void R_DrawYellowText(const char* msg) {
	SDL_Color textColor = { 255,255,0,255 };

	R_DrawTextTexture(TTF_RenderUTF8_Blended(font, msg, textColor), 275, 10 + (4 * 13) + 275, true);

	SDL_RenderPresent(gRenderer);
}

/* Draws game over screen and text prompt */
void R_DrawNameBuffer(const char *name, const int len) {
	SDL_Color whiteColor = { 255,255,255,255 };

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, STR_ENDGAME_TITLE, whiteColor), 50, 30, true);
	R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, STR_ENDGAME_PROMPT, whiteColor), 50, 55, true);

	if(len)
		R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, name, whiteColor), 50, 80, true);

	SDL_RenderPresent(gRenderer);
}

void R_DrawHighscores() {
	SDL_Color whiteColor = { 255,255,255,255 };
	Score* entry = scoreListHead.next;

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, STR_HIGHSCORE_TITLE, whiteColor), 50, 30, true);



	for (int i = 0; i < scoreCount; i++) {
		R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, entry->name, whiteColor), 50, 55 + (i * 25), true);

		char scoreText[32];
		sprintf(scoreText, "%u", entry->score);
		R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, scoreText, whiteColor), 400, 55 + (i * 25), true);
		entry = entry->next;
	}

	R_DrawTextTexture(TTF_RenderUTF8_Blended(font, STR_HIGHSCORE_ENTER, whiteColor), 50, 500, true);

	SDL_RenderPresent(gRenderer);


}


/* Draws the frame and presents it */
void R_DrawFrame(const GameState *state, bool *drawSaveNotice, bool *drawLoadNotice) {
	int static drawNoticeCounter = 0;
	SDL_Color pointsColor = { 255,255,255,255 };

	SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 255);
	SDL_RenderClear(gRenderer);

	/* Playing field frame */
	SDL_SetRenderDrawColor(gRenderer, 255, 255, 255, 255);

	SDL_Rect frame = {0};
	frame.h = 502;
	frame.w = 252;
	frame.x = 10;
	frame.y = 10;

	SDL_RenderDrawRect(gRenderer, &frame);


	/* Playing instructions */
	for (int i = 0; i <= 8; i++) {
		R_DrawTextTexture(messageSurf[i], 275, 10 + (i*13), false);
	}

	/* Points counter */
	R_DrawTextTexture(messageSurf[9], 275, 10 + (4 * 13) + 425, false);
	char pointsText[32];
	sprintf(pointsText, "%u", state->points);
	R_DrawTextTexture(TTF_RenderUTF8_Blended(largerFont, pointsText, pointsColor), 275 + messageSurf[8]->w, 10 + (4 * 13) + 425, true);

	/* Solid blocks in board */
	for (int i = 0; i <= BOARD_WIDTH-1; i++) {
		for (int ii = 0; ii <= BOARD_HEIGHT-1; ii++) {
			if(state->boardState[ii][i] != 0)
				R_DrawTileInField(i, ii, state->boardState[ii][i]);
		}
	}
	
	/* Current falling tetromino */
	for (int i = 0; i <= 3; i++) {
		R_DrawTileInField(state->x + orientationTable[state->currentTetrominoOrientation][i][0], state->y + orientationTable[state->currentTetrominoOrientation][i][1], state->currentTetrominoColor);
	}

	/* Save or load notice*/
	if (*drawSaveNotice) {
		if (drawNoticeCounter != 10) {
			R_DrawYellowText(STR_SAVE_NOTICE);
			drawNoticeCounter++;
		}
		else {
			drawNoticeCounter = 0;
			*drawSaveNotice = false;
		}
	}

	if (*drawLoadNotice) {
		if (drawNoticeCounter != 10) {
			R_DrawYellowText(STR_LOAD_NOTICE);
			drawNoticeCounter++;
		}
		else {
			drawNoticeCounter = 0;
			*drawLoadNotice = false;
		}
	}

	SDL_RenderPresent(gRenderer);
}