#pragma once
namespace MemManagement
{
	std::vector<PROCESSENTRY32> GetProcList();
	std::wstring GetDLLName();
	void InjectDllInto(PROCESSENTRY32 pProcess);
};