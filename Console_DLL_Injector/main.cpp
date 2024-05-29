#include "Console.h"
#include "ScreenController.h"
#include "MemoryUtils.h"
#include "PagesManager.h"
#include <iostream>
#include <conio.h>

int main()
{
	SetConsoleTitle(L"Corty Injector");

	// Get process entries
	MemoryUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };

	if (procEntryList.empty())
	{
		std::cerr << "[!] No process found.";
		system("PAUSE");
		return -1;
	} 

	PagesManager pagesManager(procEntryList);
	ScreenController screenManager;

	// First screen
	Console::PrintProcessPage(&pagesManager);

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (screenManager.bScreenProcess)
		{
			screenManager.RunProcPage(&pagesManager, &memUtils);
		}
		else if (screenManager.bScreenDLL)
		{
			screenManager.RunDLLPage(&pagesManager, &memUtils);
		}

		Sleep(5);
	}

	return 0;
}