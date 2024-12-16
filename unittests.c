/*++
	File: unittests.c

	Copyright (C) gabijaba
	2024-01-24

++*/

#include "game.h"

void test1() {
	assert(Game_ValidateOrientationAtPosition(2,2, O_) == true);
	assert(Game_ValidateOrientationAtPosition(15,25, L_DOWN) == false);
	assert(Game_ValidateOrientationAtPosition(9,4, I_HORIZ) == false);
	assert(Game_ValidateOrientationAtPosition(0,0, O_) == false);
	assert(Game_ValidateOrientationAtPosition(9,19, I_HORIZ) == false);
	for (int i = 0; i < 19; i++) {
		assert(Game_ValidateOrientationAtPosition(5, 2, i) == true);
	}
	Log_Write("1st unit test successful");
}

void test2() {
	Game_AddScore("Test", 100);

	assert(scoreListHead.next);
	assert(!strcmp(scoreListHead.next->name, "Test"));
	assert(scoreListHead.next->score == 100);
	assert(scoreListHead.next->prev == &scoreListHead);

	Game_AddScore("Test1", 77);

	assert(scoreListHead.next->next);
	assert(!strcmp(scoreListHead.next->next->name, "Test1"));
	assert(scoreListHead.next->next->score == 77);
	assert(scoreListHead.next->next->prev == scoreListHead.next);

	Log_Write("2nd unit test successful");
}

void test3() {
	Game_AddScore("Test1", 500);
	Game_AddScore("Test2", 700);
	Game_AddScore("Test3", 1000);
	Game_AddScore("Test4", 800);
	Game_AddScore("Test5", 600);

	Game_SortScores();

	assert(scoreListHead.next->score == 1000);
	assert(scoreListHead.next->next->score == 800);
	assert(scoreListHead.next->next->next->score == 700);
	assert(scoreListHead.next->next->next->next->score == 600);
	assert(scoreListHead.next->next->next->next->next->score == 500);

	Log_Write("3rd unit test successful");
}