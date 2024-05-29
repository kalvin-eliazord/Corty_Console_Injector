#include "PagesManager.h"

PagesManager::PagesManager(std::vector<PROCESSENTRY32> pProcList)
	: maxProcessNb { static_cast<int>(pProcList.size())}
{
	// Storing name and ID of each process
	SetProcessMap(pProcList);
	SetProcIterator();
	SetTotalProcess();
}

void PagesManager::SetProcessMap(std::vector<PROCESSENTRY32> pProcList)
{
	if (pProcList.empty()) return;

	for (auto& currProc : pProcList)
		processMap[currProc.szExeFile] = currProc.th32ProcessID;
}

void PagesManager::SetTotalProcess()
{
	if (processMap.empty()) return;

	totalPages = (static_cast<int>(processMap.size()) - 1) / totalProcPerPage;
}

int PagesManager::GetCurrentPage()
{
	return currPage;
}

int PagesManager::GetTotalPages()
{
	return totalPages;
}

int PagesManager::GetTotalProcess() const
{
	return maxProcessNb;
}

int PagesManager::GetTotalProcPerPage() const
{
	return totalProcPerPage;
}

std::map<std::wstring, DWORD> PagesManager::GetProcessMap()
{
	return processMap;
}

void PagesManager::GoPreviousPage()
{
	--currPage;
	
	// Looping to the last page
	currPage = (currPage < firstPage) ? totalPages : currPage;

	const int offset = (currPage - firstPage) * totalProcPerPage;
	auto it = processMap.begin();
	std::advance(it, offset);
	procIterator = it;
}

DWORD PagesManager::GetUserIdProc()
{
	return processMap[userProcess];
}

std::wstring_view PagesManager::GetUserProcess()
{
	return userProcess;
}

void PagesManager::SetUserProcess(DWORD pIdChosen)
{
	userProcess = FindUserProcess(pIdChosen);
}

std::wstring PagesManager::FindUserProcess(DWORD pIdChosen)
{
	auto it{ std::find_if(processMap.begin(), processMap.end(),
		[pIdChosen](const std::pair<const std::wstring, DWORD>& entry)
		{
			 return entry.second == pIdChosen;
		}) };

	if (it != processMap.end())
		return it->first;

	// Return an empty string if the value is not found
	return std::wstring();
}

void PagesManager::GoNextPage()
{
	++currPage;

	// Looping to the first page
	currPage = (currPage > totalPages) ? firstPage : currPage;

	const int offset = (currPage - firstPage) * totalProcPerPage;
	auto it = processMap.begin();
	std::advance(it, offset);
	procIterator = it;
}

std::map<std::wstring, DWORD>::iterator PagesManager::GetProcIterator()
{
	return procIterator;
}

void PagesManager::SetProcIterator()
{
	procIterator = processMap.begin();
}