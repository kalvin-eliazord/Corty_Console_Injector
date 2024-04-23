#pragma once
#include <vector>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <map>

class PagesManager
{
private:
	int procIdChosen;
	const int maxProcessNb;
	const int processPerPage{ 21 };
	std::map<std::wstring, DWORD> procNamesAndId;

	int totalPages;
	const int firstPage{ 1 };
	int currPage{ firstPage };
	std::map< std::wstring, DWORD>::iterator procIterator;

	void SetTotalProcess();
	void SetProcIterator();
	void SetProcNamesAndId(std::vector<PROCESSENTRY32> pProcList);

public:
	void GoNextPage();
	int GetTotalPages();
	int GetCurrentPage();
	void GoPreviousPage();
	int GetTotalProcess() const;
	int GetTotalProcPerPage() const;
	std::map<std::wstring, DWORD> GetProcNamesAndId();
	std::map<std::wstring, DWORD> GetCurrPageProcess();
	PagesManager(std::vector<PROCESSENTRY32> pProcList);
	std::map< std::wstring, DWORD>::iterator GetProcIterator();
};