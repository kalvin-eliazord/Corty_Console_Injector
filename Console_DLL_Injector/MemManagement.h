#pragma once
namespace MemManagement
{
	std::vector<PROCESSENTRY32> GetProcList();
	std::wstring GetDllName();
	std::wstring GetDllCurrDirectory();
	void InjectDllInto(PROCESSENTRY32 pProcess);
};