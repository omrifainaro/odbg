#include "Utils.h"

void PrintCSBackupAPIErrorMessage(DWORD dwErr) {
	char buf[512];
	char* ptr;
	DWORD dwChars;

	dwChars = FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwErr,
		0,
		buf,
		512,
		NULL);
	ptr = strchr(buf, '\n');
	if (ptr)
		*ptr = '\0';
	printf("[Error] Error value: %d Message: %s\n",
		dwErr,
		dwChars ? buf : L"Error message not found.");

}
