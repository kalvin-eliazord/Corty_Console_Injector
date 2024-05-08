#include "ConsolePrinter.h"
#include "ScreenManager.h"
#include "MemoryUtils.h"
#include "PagesManager.h"
#include <iostream>
#include <conio.h>

int main()
{
	// Get process entries
	MemoryUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };

	if (procEntryList.empty())
	{
		std::cerr << "[!] No process found.";
		Sleep(5000);
		return -1;
	} 

	PagesManager pagesManager(procEntryList);
	ScreenManager screenManager;

	// First screen
	ConsolePrinter::PrintProcessPage(&pagesManager);

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (screenManager.bScreenProcess)
		{
			screenManager.RunProcPage(&pagesManager, &memUtils);
		}
		else if (screenManager.bScreenDLL)
		{
			screenManager.RunDllPage(&pagesManager, &memUtils);
		}

		Sleep(5);
	}

	return 0;
}