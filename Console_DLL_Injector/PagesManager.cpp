#include "PagesManager.h"

PagesManager::PagesManager(std::vector<PROCESSENTRY32> pProcList)
	: maxProcessNb { static_cast<int>(pProcList.size())}
{
	// Storing name and ID of each process
	SetProcNamesAndId(pProcList);
	SetProcIterator();
	SetTotalProcess();
}

void PagesManager::SetProcNamesAndId(std::vector<PROCESSENTRY32> pProcList)
{
	if (pProcList.empty()) return;

	for (auto& currProc : pProcList)
		procNamesAndId[currProc.szExeFile] = currProc.th32ProcessID;
}

void PagesManager::SetTotalProcess()
{
	if (procNamesAndId.empty()) return;

	totalPages = (static_cast<int>(procNamesAndId.size()) - 1) / processPerPage;
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
	return processPerPage;
}

std::map<std::wstring, DWORD> PagesManager::GetProcNamesAndId()
{
	return procNamesAndId;
}

void PagesManager::GoPreviousPage()
{
	--currPage;
	
	// Looping to the last page
	currPage = (currPage < firstPage) ? totalPages : currPage;

	// Looping to the last iterator position
	if (currPage == totalPages)
		procIterator = procNamesAndId.end();

	const int distPosIt{ std::distance(procNamesAndId.begin(), procIterator) };

	if(distPosIt < processPerPage)
		std::advance(procIterator, -distPosIt);
	else
		std::advance(procIterator, -processPerPage);
}

void PagesManager::GoNextPage()
{
	++currPage;

	// Looping to the first page
	currPage = (currPage > totalPages) ? firstPage : currPage;

	// Looping to the first iterator position
	if (currPage == firstPage)
		procIterator = procNamesAndId.begin();

	const int distPosIt{ std::distance(procIterator, procNamesAndId.end()) };

	if (distPosIt < processPerPage)
		std::advance(procIterator, distPosIt);
	else 
		std::advance(procIterator, processPerPage);
}

std::map<std::wstring, DWORD>::iterator PagesManager::GetProcIterator()
{
	return procIterator;
}

void PagesManager::SetProcIterator()
{
	procIterator = procNamesAndId.begin();
}

std::map<std::wstring, DWORD> PagesManager::GetCurrPageProcess()
{
	return std::map<std::wstring, DWORD>();
}
