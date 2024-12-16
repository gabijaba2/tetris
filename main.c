/*++
	File: main.c

	Copyright (C) gabijaba
	2023-12-08

	Platform specific entry point

++*/
#include "game.h"

#define RUN_TESTSS

#if defined(_WIN64) || defined(_WIN32) 
#include <Windows.h>

WINAPI
WinMain(
    _In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPSTR lpCmdLine,
    _In_ int nShowCmd
) {

#if defined(RUN_TESTS)
	Log_Init();
	test1();
	test2();
	test3();
#else
	Game_Main();
#endif


	return 0;
}

#endif