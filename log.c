/*++
	File: log.c

	Copyright (C) gabijaba
	2024-01-24

	Logging functionality

++*/

#include "game.h"

FILE* logFile;
clock_t timeSinceStart;

void Log_AtexitCallback() {
	timeSinceStart = clock() - timeSinceStart;
	Log_Write(STR_LOG_END, ((float)timeSinceStart / (float)CLOCKS_PER_SEC));
	Log_Close();
}

void Log_Init() {
	logFile = fopen(LOG_FILE, "a");
	if (!logFile) {
		Game_Error(STR_ERROR_LOG_FILE);
	}
	timeSinceStart = clock();

	atexit(Log_AtexitCallback);
}

void Log_Write(const char* str, ...) {
	va_list arg;
	time_t currentTime;
	char buffer[512];
	char* timeStr;

	va_start(arg, str);
	vsprintf(buffer, str, arg);
	va_end(arg);

	time(&currentTime);
	timeStr = ctime(&currentTime);
	timeStr[24] = 0;
	fprintf(logFile, "[%s] %s\n", timeStr, buffer);

}

void Log_Close() {
	fclose(logFile);

}