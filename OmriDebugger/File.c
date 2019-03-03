#include "File.h"

char* SECTION_NAMES[15] = { "Export Table", "Import Table", "Resource Table",
"Exception Table", "Certificate Table", "Base Relocation Table", "DEBUG", "Architecture Table",
"Global Pointer", "Tls Table", "Load Config Table", "Bound Import", "Import Addresss Table",
"Delay Import Descriptor", "CLR Runtime Header" };


//(Virtual address) - (base address) = RVA
unsigned long rvaToOffset(IMAGE_NT_HEADERS *ntHeader, unsigned long rva) {
	IMAGE_SECTION_HEADER* sections = IMAGE_FIRST_SECTION(ntHeader);
	int i = 0;
	for (; i < ntHeader->FileHeader.NumberOfSections; i++) {
		if (sections[i].VirtualAddress <= rva && sections[i].VirtualAddress + sections[i].SizeOfRawData >= rva) {
			return rva - (sections[i].VirtualAddress - sections[i].PointerToRawData);
		}
	}
}

void dumpSectionData(IMAGE_SECTION_HEADER* sections, unsigned char* baseOfFile, unsigned char* imageBase, unsigned int noData) {
	int i = 0;
	printf("%-12s%-24s%-24s%-24s%-16s\n", "Name", "Physical Address", "Virtual Address", "Pointer Raw Data", "Size Raw Data");
	for (; i < noData; i++) {
		printf("%-12s%-24p%-24p%-24p%-6d%-10s\n", sections[i].Name,
			baseOfFile + sections[i].Misc.PhysicalAddress, imageBase + sections[i].VirtualAddress,
			sections[i].PointerToRawData, sections[i].SizeOfRawData, "bytes");
	}
	printf("\n");
}

void printPEData(char* filename) {
	DWORD lowSize = 0;
	DWORD highSize = 0;
	HANDLE fMap;
	PIMAGE_DOS_HEADER dosHeader;
	PIMAGE_NT_HEADERS ntHeader;
	PIMAGE_SECTION_HEADER sections;
	ULONG imageBase;
	ULONG entrypoint;
	PUCHAR physicalImageBase;
	LPVOID fileView = NULL;
	DWORD bytesRead;

	HANDLE hFile = CreateFileA(filename, 
		GENERIC_READ, 
		0, 
		NULL, 
		OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, 
		NULL);

	lowSize = GetFileSize(hFile, &highSize);
	fileView = VirtualAlloc(NULL, lowSize, MEM_COMMIT, PAGE_READWRITE);
	ReadFile(hFile, fileView, lowSize, &bytesRead, NULL);
	CloseHandle(hFile);
	dosHeader = fileView;
	if (!(dosHeader->e_magic == IMAGE_DOS_SIGNATURE)) {
		printf("Couldn't find dos header!\n");
		return 0;
	}
	ntHeader = (IMAGE_NT_HEADERS*)((unsigned char*)dosHeader + dosHeader->e_lfanew);
	if (!(ntHeader->Signature == PE_SIGNATURE)) { 
		printf("Couldnt find PE header!\n");
		PrintCSBackupAPIErrorMessage(GetLastError());
		return 0;
	}
	printf("[FILE DATA] ImageBase: 0x%x\n", ntHeader->OptionalHeader.ImageBase);
	imageBase = ntHeader->OptionalHeader.ImageBase;
	entrypoint = imageBase + ntHeader->OptionalHeader.AddressOfEntryPoint;
	physicalImageBase = (PUCHAR)&ntHeader->OptionalHeader.ImageBase;
	printf("[*] Address of entry point: 0x%x\n", entrypoint);

	printf("[FILE DATA] Number of sections: %d\n", ntHeader->FileHeader.NumberOfSections);
	printf("[FILE DATA] Pointer to symbol table: %p\n", imageBase + ntHeader->FileHeader.PointerToSymbolTable);
	printf("[FILE DATA] Number of symbols: %d\n", ntHeader->FileHeader.NumberOfSymbols);
	printf("[FILE DATA] .text section size: %d bytes\n", ntHeader->OptionalHeader.SizeOfCode);
	printf("[FILE DATA] Intitialized data size: %d bytes\n", ntHeader->OptionalHeader.SizeOfInitializedData);
	printf("[FILE DATA] Intitialized data size: %d bytes\n", ntHeader->OptionalHeader.SizeOfUninitializedData);
	printf("[FILE DATA] Code starts: 0x%x\n", imageBase + ntHeader->OptionalHeader.BaseOfCode);
	printf("[FILE DATA] Data starts: 0x%x\n", imageBase + ntHeader->OptionalHeader.BaseOfData);
	
	sections = (PIMAGE_SECTION_HEADER)((unsigned char*)ntHeader + FIELD_OFFSET(IMAGE_NT_HEADERS, OptionalHeader) + ntHeader->FileHeader.SizeOfOptionalHeader);
	dumpSectionData(sections, physicalImageBase, imageBase, ntHeader->FileHeader.NumberOfSections);
	//dumpImportTable(ntHeader, physicalImageBase + rvaToOffset(ntHeader, ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress), fileView);

	return 1;
}