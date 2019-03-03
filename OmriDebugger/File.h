#pragma once
#include <Windows.h>
#include <stdio.h>
#include "Utils.h"

#define PE_SIGNATURE 0x4550

/*
Taken from https://github.com/omrifainaro/PE-parser/blob/master/Main.c
Many thanks to 2017 Omri Fainaro!
But today you are better just so u know...
*/


void printPEData(char* filename);
