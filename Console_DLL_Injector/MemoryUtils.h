#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <filesystem>
#include <vector>
#include <direct.h> 

class MemoryUtils
{
private:
	std::string dllName{""};
public:
	MemoryUtils();
	void SetDllName();
	std::string_view GetDllName();
	std::string GetDllCurrDirectory();
	bool InjectDllIntoProc(DWORD pProcId);
	std::vector<PROCESSENTRY32> GetProcList();
};