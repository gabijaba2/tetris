/*++
	File: score.c

	Copyright (C) gabijaba
	2024-01-24

	Score linked list management, sorting functionality

++*/

#include "game.h"

Score scoreListHead = {0};
int scoreCount = 0;

/* Adds an entry to the linked list */
void Game_AddScore(const char *name, const unsigned int score) {
	Score* entry = NULL;
	Score* newEntry = malloc(sizeof(Score));
	char* str = NULL;

	if (!newEntry) {
		Game_Error(STR_ERROR_MEM);
	}
	memset(newEntry, 0, sizeof(Score));

	str = malloc(strlen(name) + 1);
	if (!str) {
		Game_Error(STR_ERROR_MEM);
	}
	memset(str, 0, strlen(name) + 1);
	memcpy(str, name, strlen(name));

	newEntry->name = str;
	newEntry->score = score;	
	
	if (scoreCount)
		entry = scoreListHead.next;
	else
		entry = &scoreListHead;

	while (entry->next) {
		entry = entry->next;
	}

	entry->next = newEntry;
	newEntry->prev = entry;

	scoreCount++;

}

/* Gets a specific element from the linked list */
Score* Game_SortGetListElement(const int num) {
	Score* entry = scoreListHead.next;
	
	assert(entry);

	if (num == 0) {
		return entry;
	}

	for (int i = 1; i <= num; i++) {
		entry = entry->next;
	}

	return entry;

}

/* Swaps the data in two linked list elements */
void Game_SwapListElements(Score* a, Score* b) {
	char* nameTemp = NULL;
	int scoreTemp = 0;

	assert(a && b);

	nameTemp = a->name;
	scoreTemp = a->score;

	a->name = b->name;
	a->score = b->score;

	b->name = nameTemp;
	b->score = scoreTemp;

}

/* Sorts the linked list in descending order */
void Game_SortScores() {

	for (int i = 0; i < scoreCount - 1; i++) {
		for (int j = 0; j < scoreCount - i - 1; j++) {
			Score* A = Game_SortGetListElement(j);
			Score* B = Game_SortGetListElement(j + 1);
			if (A->score < B->score) {
				Game_SwapListElements(A, B);
			}
		}
	}

}