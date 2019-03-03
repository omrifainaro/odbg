#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "Utils.h"
#include "Debugger.h"

#define MY_STRCMP(x, y) !strcmp(x, y, sizeof(y) - 1)
#define USER_BUFF_LEN 256
#define MAX_ARGS 5

typedef struct _PARSED_USER_LINE {
	char* command;
	char* args[MAX_ARGS];
	unsigned int amount;
} PARSED_USER_LINE;

void userInputLoop();
void printMenu();
int createProcess(char* procname);
int runProcess();
int userSingleStep();
int continueDebuggingLoop(STARTUPINFO* si, PROCESS_INFORMATION* pi);
int parseUserCommand(char* command, PARSED_USER_LINE* parsed);
int debugLoop(STARTUPINFO* si, PROCESS_INFORMATION* pi);