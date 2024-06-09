#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>
#include <direct.h> 
#include "PageProcess.h"

#define RELOC_FLAG32(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_HIGHLOW)
#define RELOC_FLAG64(RelInfo) ((RelInfo >> 0x0C) == IMAGE_REL_BASED_DIR64)

#ifdef _WIN64
#define RELOC_FLAG RELOC_FLAG64
#else
#define RELOC_FLAG RELOC_FLAG32
#endif

#ifdef _WIN64
#define CURRENT_ARCH IMAGE_FILE_MACHINE_AMD64
#else
#define CURRENT_ARCH IMAGE_FILE_MACHINE_I386
#endif

struct DataDLL
{
	std::string path{};
	std::string name{};
	bool isWow64{};
};

struct DataProc
{
	int id{};
	std::wstring name{};
	HANDLE hProc{};
};

struct PE_Header
{
	bool Init(BYTE* pModBase);
	IMAGE_NT_HEADERS* nt{ nullptr };
	IMAGE_OPTIONAL_HEADER* opt{ nullptr };
	IMAGE_FILE_HEADER* file{ nullptr };
};

class MemUtils
{
public:
	bool DllMap();
	bool ImportAndShellCodeMap();
	BYTE* mAllocProc{ nullptr };
	DataProc dataProc;
	DataDLL dataDll;
	PE_Header pe_head;
	bool SetDllName();
	void SetDllPath();
	void InitDataProc(PageProcess* pPageProc);
	std::string GetCurrDirectory();
	bool WinAPI_Inject_Start();
	bool ManualMap_Start();
	bool WriteMem(auto* pSrc, auto* pDst, auto pSize);
	std::vector<PROCESSENTRY32> GetProcList();
};

using t_LoadLibraryA = HINSTANCE(WINAPI*)(const char* lpLibFilename);
using t_GetProcAddress = FARPROC(WINAPI*)(HINSTANCE hModule, const char* lpProcName);
using t_DLL_ENTRY_POINT = BOOL(WINAPI*)(void* hDll, DWORD dwReason, void* pReserved);

struct DataImport
{
	t_LoadLibraryA _LoadLibraryA{};
	t_GetProcAddress _GetProcAddress{};
	BYTE* baseAddr;
	HINSTANCE hMod{};
	DWORD fdwReasonParam{ DLL_PROCESS_ATTACH };
	LPVOID reservedParam{ nullptr };
};

void _stdcall ShellLoader(DataImport* pDataImport);