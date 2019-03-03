#pragma once
#include "User.h"

char userInput[USER_BUFF_LEN + 1];
STARTUPINFOA si = { 0 };
PROCESS_INFORMATION pi = { 0 };
BOOL hasProcess = FALSE, processRunning = FALSE;
DEBUG_EVENT debugEvent;
char peFileName[MAX_PATH];

void userInputLoop() {
	char* ptr = NULL, *pEnd = NULL;
	LPVOID addr;
	DWORD size;
	PARSED_USER_LINE parsedInput = { 0 };
	printMenu();
	while (strncmp(userInput, "q", sizeof("q") - 1)) {
		printf("$ odbg: ");
		fgets(userInput, USER_BUFF_LEN, stdin);
		if (ptr = strchr(userInput, '\n')) {
			*ptr = 0;
		}
		parseUserCommand(userInput, &parsedInput);
		if (MY_STRCMP(parsedInput.command, "help")) {
			printMenu();
		}
		else if (MY_STRCMP(parsedInput.command, "create")) {
			if (!hasProcess) {
				createProcess(parsedInput.args[0]);
				strncpy_s(peFileName, MAX_PATH - 1, parsedInput.args[0], MAX_PATH - 1);
			}
			else {
				printf("Process already running...\n");
			}
		}
		else if (MY_STRCMP(parsedInput.command, "run")) {
			if (!processRunning) {
				runProcess();
				processRunning = TRUE;
			}
			else {
				printf("Process already running...\n");
			}
		}
		else if (MY_STRCMP(parsedInput.command, "bp")) {
			if (!parsedInput.args[0] || !parsedInput.args[1]) {
				printf("Not enough args...\n");
				continue;
			}
			if (*parsedInput.args[0] == 'd')
				addr = strtol(parsedInput.args[1], &pEnd, 10);
			else if (*parsedInput.args[0] == 'h')
				addr = strtol(parsedInput.args[1], &pEnd, 16);
			else if (*parsedInput.args[0] == 'o')
				addr = strtol(parsedInput.args[1], &pEnd, 8);
			else if (*parsedInput.args[0] == 'b')
				addr = strtol(parsedInput.args[1], &pEnd, 2);
			else {
				printf("Not a valid address base...\n");
				continue;
			}
			insertBreakpoint(pi.hProcess, addr);
		}
		else if (MY_STRCMP(parsedInput.command, "file")) {
			if(hasProcess)
				printPEData(peFileName);
		}
		else if (MY_STRCMP(parsedInput.command, "continue")) {
			if (hasProcess)
				continueDebuggingLoop(&si, &pi);
		}
		else if (MY_STRCMP(parsedInput.command, "step")) {
			userSingleStep();
		}
		else if (MY_STRCMP(parsedInput.command, "examine")) {
			if (!parsedInput.args[0] || !parsedInput.args[1] || !parsedInput.args[2]) {
				printf("Not enough args...\n");
				continue;
			}	
			if (*parsedInput.args[0] == 'd')
				addr = strtol(parsedInput.args[1], &pEnd, 10);
			else if (*parsedInput.args[0] == 'h')
				addr = strtol(parsedInput.args[1], &pEnd, 16);
			else if (*parsedInput.args[0] == 'o')
				addr = strtol(parsedInput.args[1], &pEnd, 8);
			else if (*parsedInput.args[0] == 'b')
				addr = strtol(parsedInput.args[1], &pEnd, 2);
			else {
				printf("Not a valid address base...\n");
				continue;
			}
			size = atoi(parsedInput.args[2]);
			examineMemory(pi.hProcess, addr, size);
		}
		else if (MY_STRCMP(parsedInput.command, "registers")) {
			printRegistersData(pi.hThread);
		}
		else {
			printf("Unkown Command!\n");
		}
	}
	printf("Bye :)\n");
}

int userSingleStep() {
	singleStep(pi.hThread);
	continueDebuggingLoop(&si, &pi);
}

int parseUserCommand(char* command, PARSED_USER_LINE* parsed) {
	char* context = NULL;
	char* token = strtok_s(command, " ", &context);
	memset(parsed, 0, sizeof(PARSED_USER_LINE));
	parsed->command = token;
	for (int i = 0; i < MAX_ARGS; i++) {
		token = strtok_s(NULL, " ", &context);
		if (token == NULL)
			break;
		parsed->args[i] = token;
	}
	return parsed;
}

void printMenu() {
	printf("\nWelcome to OmriDBG:\n");
	printf("Commands are as follows: \n");
	printf("help - Display this menu\n");
	printf("create <full_path> - Create a process\n");
	printf("run - Start the process\n");
	printf("continue - Continue process running\n");
	printf("step - Single step an instruction\n");
	printf("bp <h/d/o/b> <addr> - Adding a breakpoint (must specify the base of the address i.e h - hex, d - decimal...)\n");
	printf("file - Get PE file data\n");
	printf("examine <h/d/o/b> <address> <num bytes> - Examine process memory at specified address\n");
	printf("registers - print all registers\n");
	printf("\n");
}

int createProcess(char* procname) {
	si.cb = sizeof(si);
	if (!CreateProcessA(
		procname,
		NULL,
		NULL,
		NULL,
		FALSE,
		DEBUG_PROCESS | 
		CREATE_SUSPENDED,
		NULL,
		NULL,
		&si,
		&pi)) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return -1;
	}
	hasProcess = TRUE;
	return 1;
}

int runProcess() {
	ResumeThread(pi.hThread);
	printf("Process Running...\n");
	debugLoop(&si, &pi);
}

int continueDebuggingLoop(STARTUPINFO* si, PROCESS_INFORMATION* pi) {
	ContinueDebugEvent(pi->dwProcessId, pi->dwThreadId, DBG_CONTINUE);
	debugLoop(si, pi);
}

int debugLoop(STARTUPINFO* si, PROCESS_INFORMATION* pi) {
	while (WaitForDebugEvent(&debugEvent, INFINITE)) {
		if (!handleEvent(&debugEvent, si, pi)) {
			return 1;
		}
		ContinueDebugEvent(pi->dwProcessId, pi->dwThreadId, DBG_CONTINUE);
	}
	return 0;
}