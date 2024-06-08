#include "Console.h"
#include "ScreenController.h"
#include "MemUtils.h"
#include "PageProcess.h"
#include <iostream>
#include <conio.h>

int main()
{
	SetConsoleTitle(L"Corty Injector");

	// Get process entries
	MemUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };

	if (procEntryList.empty())
	{
		std::cerr << "[!] No process found.";
		system("PAUSE");
		return -1;
	}

	PageProcess pagesManager(procEntryList);
	ScreenController screenManager;

	// First screen
	Console::PrintProcessPage(&pagesManager);

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (screenManager.bScreenProcess)
			screenManager.RunProcPage(&pagesManager, &memUtils);
		else if (screenManager.bScreenDLL)
			screenManager.RunDLLPage(&pagesManager, &memUtils);

		Sleep(5);
	}

	if(memUtils.dataProc.hProc)
		CloseHandle(memUtils.dataProc.hProc);

	delete[] memUtils.dataDll.buffer;

	return 0;
}