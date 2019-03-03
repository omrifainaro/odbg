#pragma once
#include <stdio.h>
#include <Windows.h>
#include "Utils.h"

#define MAX_BREAKPOINTS 5

extern LPVOID processEntrypoint;

typedef struct _BREAKPOINT {
	LPVOID addr;
	BYTE prevMem;
} BREAKPOINT;

int insertBreakpoint(HANDLE proc, LPVOID address);
int revertBreakpoint(HANDLE proc, LPVOID address);
int stepBackBreakpoint(HANDLE thr);
int examineMemory(HANDLE proc, LPVOID address, DWORD numBytes);
int printRegistersData(HANDLE thr);
int singleStep(HANDLE thr);