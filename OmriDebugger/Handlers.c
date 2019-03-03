#include "Handlers.h"

int handleEvent(DEBUG_EVENT* e, STARTUPINFO* si, PROCESS_INFORMATION* pi) {
	switch (e->dwDebugEventCode)
	{
	case EXIT_PROCESS_DEBUG_EVENT:
		printf("Process exit with code: %d\n", e->u.ExitProcess.dwExitCode);
		return 0;
	case CREATE_PROCESS_DEBUG_EVENT:
		handleProccesCreate(e);
		break;
	case LOAD_DLL_DEBUG_EVENT:
	case UNLOAD_DLL_DEBUG_EVENT:
		handleDllEvent(pi->hProcess, e);
		break;
	case EXCEPTION_DEBUG_EVENT:
		handleExceptionEvent(pi->hProcess, pi->hThread, e);
		return 0;
		break;
	default:
		printf("Debuggee sent event code: %d, no available handler...\n", e->dwDebugEventCode, pi->hProcess);
		break;
	}
	return 1;
}

int handleExceptionEvent(HANDLE proc, HANDLE thread, DEBUG_EVENT* e) {
	EXCEPTION_RECORD exception = e->u.Exception.ExceptionRecord;
	switch (exception.ExceptionCode)
	{
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	case EXCEPTION_BREAKPOINT:
		printf("[Exception] Breakpoint Hit at 0x%x\n", exception.ExceptionAddress);
		if (!revertBreakpoint(proc, exception.ExceptionAddress))
			return 0;
		stepBackBreakpoint(thread);
		break;
	case EXCEPTION_ACCESS_VIOLATION:
		printf("[Exception] Access Violation at 0x%x\n", exception.ExceptionAddress);
		getchar();
		break;
	case EXCEPTION_SINGLE_STEP:
		printf("[Exception] single step: 0x%08x!\n", exception.ExceptionAddress);
		break;
	default:
		printf("[EXCEPTION] Debuggee exception code: 0x%x at 0x%x\n", exception.ExceptionCode, exception.ExceptionAddress);
		break;
	}
	return 1;
}

int handleProccesCreate(DEBUG_EVENT* e) {
	printf("[CREATE] Process Created!\n");
	printf("[CREATE] Entrypoint: 0x%x\n", e->u.CreateProcessInfo.lpStartAddress);
	printf("[CREATE] Image base: 0x%x\n", e->u.CreateProcessInfo.lpBaseOfImage);
	processEntrypoint = e->u.CreateProcessInfo.lpStartAddress;
	return 1;
}

int handleDllEvent(HANDLE proc, DEBUG_EVENT* e) {
	char buf[MAX_PATH] = { 0 };

	if (e->dwDebugEventCode == LOAD_DLL_DEBUG_EVENT) {
		//if (!GetModuleFileNameExA(proc, e->u.LoadDll.hFile, &buf, sizeof(buf))) {
		//	printf("[DLL] Unknown load to 0x%x\n", e->u.LoadDll.lpBaseOfDll);
		//	PrintCSBackupAPIErrorMessage(GetLastError());
		//}
		//else
		printf("[DLL] load to 0x%x\n", e->u.LoadDll.lpBaseOfDll);
	}
	else if (e->dwDebugEventCode == UNLOAD_DLL_DEBUG_EVENT) {
		printf("[DLL] 0x%x unloaded\n", e->u.UnloadDll.lpBaseOfDll);
	}
	else {
		printf("Unkown event code: %d\n", e->dwDebugEventCode);
		return 0;
	}
	return 1;
}