#pragma once
#include <Windows.h>
#include <psapi.h>
#include <stdio.h>

#include "Debugger.h"
#include "Utils.h"
#include "User.h"

int handleEvent(DEBUG_EVENT* e);
int handleDllEvent(HANDLE proc, DEBUG_EVENT* e);
int handleProccesCreate(DEBUG_EVENT* e);
int handleExceptionEvent(HANDLE proc, HANDLE thread, DEBUG_EVENT* e);