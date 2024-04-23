#pragma once
#include <Windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <string>
#include <filesystem>
#include <vector>

class MemoryUtils
{
private:
	std::wstring dllName{ L" There is no .DLL in the folder. Please insert .DLL file" };
	void SetDllName();

public:
	std::wstring GetDllName();
	std::vector<PROCESSENTRY32> GetProcList();
	void InjectDllIntoProc(DWORD pProcId);
	std::wstring GetDllCurrDirectory();
};