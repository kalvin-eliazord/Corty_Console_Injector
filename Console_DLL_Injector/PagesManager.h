#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <algorithm>
#include <vector>
#include <string>
#include <map>

class PagesManager
{
private:
	const int maxProcessNb;
	std::wstring userProcess;
	const int totalProcPerPage{ 21 };
	std::map<std::wstring, DWORD> processMap;

	int totalPages;
	const int firstPage{ 1 };
	int currPage{ firstPage };
	std::map< std::wstring, DWORD>::iterator procIterator;

	void SetTotalProcess();
	void SetProcIterator();
	void SetProcessMap(std::vector<PROCESSENTRY32> pProcList);
	std::wstring FindUserProcess(DWORD pIdChosen);

public:
	void GoNextPage();
	int GetTotalPages();
	int GetCurrentPage();
	void GoPreviousPage();
	DWORD GetUserIdProc();
	int GetTotalProcess() const;
	int GetTotalProcPerPage() const;
	std::wstring_view GetUserProcess();
	void SetUserProcess(DWORD pIdChosen);
	std::map<std::wstring, DWORD> GetProcessMap();
	PagesManager(std::vector<PROCESSENTRY32> pProcList);
	std::map< std::wstring, DWORD>::iterator GetProcIterator();
};