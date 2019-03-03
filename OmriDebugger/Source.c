#include <stdio.h>
#include <Windows.h>
#include <psapi.h>
#include "Handlers.h"

#define MAX_BUF 500

int debugLoop(STARTUPINFO* si, PROCESS_INFORMATION* pi);
void printBanner();


int main(int argc, char *argv[]) {
	printBanner();
	userInputLoop();
	return 1;
}

void printBanner() {
	printf("\n");
	printf("\n");
	printf("     OOOOOOOOO     DDDDDDDDDDDDD      BBBBBBBBBBBBBBBBB           GGGGGGGGGGGGG\n");
	printf("   OO:::::::::OO   D::::::::::::DDD   B::::::::::::::::B       GGG::::::::::::G\n");
	printf(" OO:::::::::::::OO D:::::::::::::::DD B::::::BBBBBB:::::B    GG:::::::::::::::G\n");
	printf("O:::::::OOO:::::::ODDD:::::DDDDD:::::DBB:::::B     B:::::B  G:::::GGGGGGGG::::G\n");
	printf("O::::::O   O::::::O  D:::::D    D:::::D B::::B     B:::::B G:::::G       GGGGGG\n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB::::B     B:::::BG:::::G              \n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB::::BBBBBB:::::B G:::::G              \n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB:::::::::::::BB  G:::::G    GGGGGGGGGG\n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB::::BBBBBB:::::B G:::::G    G::::::::G\n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB::::B     B:::::BG:::::G    GGGGG::::G\n");
	printf("O:::::O     O:::::O  D:::::D     D:::::DB::::B     B:::::BG:::::G        G::::G\n");
	printf("O::::::O   O::::::O  D:::::D    D:::::D B::::B     B:::::B G:::::G       G::::G\n");
	printf("O:::::::OOO:::::::ODDD:::::DDDDD:::::DBB:::::BBBBBB::::::B  G:::::GGGGGGGG::::G\n");
	printf(" OO:::::::::::::OO D:::::::::::::::DD B:::::::::::::::::B    GG:::::::::::::::G\n");
	printf("   OO:::::::::OO   D::::::::::::DDD   B::::::::::::::::B       GGG::::::GGG:::G\n");
	printf("     OOOOOOOOO     DDDDDDDDDDDDD      BBBBBBBBBBBBBBBBB           GGGGGG   GGGG\n");
	printf("\n");
	printf("\n");
	printf("\n");
	printf("\n");
}

