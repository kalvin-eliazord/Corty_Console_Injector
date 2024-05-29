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
	void SetDLLName();
	std::string_view GetDLLName();
	std::string GetDLLCurrDirectory();
	bool WinAPI_Injection(DWORD pProcId);
	bool ManualMapping_Injection(DWORD pProcId);
	std::vector<PROCESSENTRY32> GetProcList();
};