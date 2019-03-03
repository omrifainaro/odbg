#include "Debugger.h"

static const BYTE INT_3 = 0xCC;
BREAKPOINT breaks[] = {
	{NULL, 0},
	{NULL, 0},
	{NULL, 0},
	{NULL, 0},
	{NULL, 0},
};

LPVOID processEntrypoint = NULL;

BREAKPOINT* getSlotByAddr(LPVOID addr) {
	for (int i = 0; i < MAX_BREAKPOINTS; i++)
	{
		if (breaks[i].addr == addr)
			return &breaks[i];
	}
	return NULL;
}

int insertBreakpoint(HANDLE proc, LPVOID address) {
	SIZE_T bytesRead = 0;
	BREAKPOINT* bp = getSlotByAddr(NULL);
	if (!bp) {
		printf("Exceeded amount of breakpoints\n");
		return 0;
	}
	bp->addr = address;
	if (!ReadProcessMemory(proc, address, &bp->prevMem, sizeof(BYTE), &bytesRead)) {
		goto error_cleanup;
	}
	if (!WriteProcessMemory(proc, address, &INT_3, sizeof(BYTE), &bytesRead)) {
		goto error_cleanup;
	}
	return 1;
error_cleanup:
	PrintCSBackupAPIErrorMessage(GetLastError());
	return 0;
}

int revertBreakpoint(HANDLE proc, LPVOID address) {
	SIZE_T bytesRead = 0;
	BREAKPOINT* bp = getSlotByAddr(address);
	if (!bp) {
		printf("Not a user created breakpoint 0x%x\n", address);
		return 0;
	}

	if (!WriteProcessMemory(proc, address, &bp->prevMem, sizeof(BYTE), &bytesRead)) {
		goto error_cleanup;
	}
	bp->addr = NULL;
	bp->prevMem = 0;
	return 1;
error_cleanup:
	PrintCSBackupAPIErrorMessage(GetLastError());
	return 0;
}

int stepBackBreakpoint(HANDLE thr) {
	CONTEXT context = {0};
	context.ContextFlags = CONTEXT_ALL;
	if (SuspendThread(thr) == (DWORD)-1) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	if (!GetThreadContext(thr, &context)) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	context.Eip -= sizeof(BYTE);
	SetThreadContext(thr, &context);
	ResumeThread(thr);
}

int examineMemory(HANDLE proc, LPVOID address, DWORD numBytes) {
	int i;
	PUCHAR mem = (PUCHAR) malloc(numBytes);
	DWORD bytesRead = 0;
	ReadProcessMemory(proc, address, mem, numBytes, &bytesRead);
	for (i = 0; i < bytesRead; i++)
	{
		printf("%02x ", mem[i]);
		if ((i + 1) % 16 == 0)
			printf("\n");
	}
	printf("\n");
	return 1;
}

void printContext(CONTEXT* context) {
	printf("\n#########Registers#########\n");
	printf("Eax: 0x%08x\n", context->Eax);
	printf("Ebx: 0x%08x\n", context->Ebx);
	printf("Ecx: 0x%08x\n", context->Ecx);
	printf("Edx: 0x%08x\n", context->Edx);
	printf("Esi: 0x%08x\n", context->Esi);
	printf("Edi: 0x%08x\n", context->Edi);
	
	printf("Ebp: 0x%08x\n", context->Ebp);
	printf("Esp: 0x%08x\n", context->Esp);
	printf("Eip: 0x%08x\n", context->Eip);
	printf("EFlags: 0x%08x\n", context->EFlags);
	
	printf("\n#####Segment Registers######\n");
	printf("SegCs: 0x%08x\n", context->SegCs);
	printf("SegDs: 0x%08x\n", context->SegDs);
	printf("SegGs: 0x%08x\n", context->SegGs);
	printf("SegFs: 0x%08x\n", context->SegFs);
	printf("SegEs: 0x%08x\n", context->SegEs);
	printf("SegSs: 0x%08x\n\n", context->SegSs);
}

int printRegistersData(HANDLE thr) {
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_ALL;
	if (SuspendThread(thr) == (DWORD)-1) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	if (!GetThreadContext(thr, &context)) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	printContext(&context);
	ResumeThread(thr);
}

int singleStep(HANDLE thr) {
	CONTEXT context = { 0 };
	context.ContextFlags = CONTEXT_ALL;
	if (SuspendThread(thr) == (DWORD)-1) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	if (!GetThreadContext(thr, &context)) {
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	context.EFlags |= 0x100;
	SetThreadContext(thr, &context);
	ResumeThread(thr);
}
