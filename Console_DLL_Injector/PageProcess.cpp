#include "PageProcess.h"

PageProcess::PageProcess(std::vector<PROCESSENTRY32> pProcList)
	: maxProcessNb { static_cast<int>(pProcList.size())}
{
	// Storing name and ID of each process
	SetProcessMap(pProcList);
	SetProcIterator();
	SetTotalProcess();
}

void PageProcess::SetProcessMap(std::vector<PROCESSENTRY32> pProcList)
{
	if (pProcList.empty()) return;

	for (auto& currProc : pProcList)
		processMap[currProc.szExeFile] = currProc.th32ProcessID;
}

void PageProcess::SetTotalProcess()
{
	if (processMap.empty()) return;

	totalPages = (static_cast<int>(processMap.size()) - 1) / totalProcPerPage;
}

int PageProcess::GetCurrentPage()
{
	return currPage;
}

int PageProcess::GetTotalPages()
{
	return totalPages;
}

int PageProcess::GetTotalProcess() const
{
	return maxProcessNb;
}

int PageProcess::GetTotalProcPerPage() const
{
	return totalProcPerPage;
}

std::map<std::wstring, DWORD> PageProcess::GetProcessMap()
{
	return processMap;
}

void PageProcess::GoPreviousPage()
{
	--currPage;
	
	// Looping to the last page
	currPage = (currPage < firstPage) ? totalPages : currPage;

	const int offset = (currPage - firstPage) * totalProcPerPage;
	auto it = processMap.begin();
	std::advance(it, offset);
	procIterator = it;
}

DWORD PageProcess::GetUserIdProc()
{
	return processMap[userProcess];
}

std::wstring_view PageProcess::GetUserProcName()
{
	return userProcess;
}

void PageProcess::SetUserProcess(DWORD pIdChosen)
{
	userProcess = FindUserProcess(pIdChosen);
}

std::wstring PageProcess::FindUserProcess(DWORD pIdChosen)
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

void PageProcess::GoNextPage()
{
	++currPage;

	// Looping to the first page
	currPage = (currPage > totalPages) ? firstPage : currPage;

	const int offset = (currPage - firstPage) * totalProcPerPage;
	auto it = processMap.begin();
	std::advance(it, offset);
	procIterator = it;
}

std::map<std::wstring, DWORD>::iterator PageProcess::GetProcIterator()
{
	return procIterator;
}

void PageProcess::SetProcIterator()
{
	procIterator = processMap.begin();
}