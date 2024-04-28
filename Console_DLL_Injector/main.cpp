#include "ConsolePrinter.h"
#include "ScreenState.h"
#include "MemoryUtils.h"
#include "PagesManager.h"
#include <iostream>
#include <conio.h>
#include <cassert>

int main()
{
	// Get process entries
	MemoryUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };
	assert(!procEntryList.empty() && "[!] No process found.");

	PagesManager pagesManager(procEntryList);
	ScreenState screenState;

	// First screen
	ConsolePrinter::PrintProcessPage(&pagesManager);

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (screenState.bScreenProcess)
		{
			// PREVIOUS page process
			if (GetAsyncKeyState(VK_F1) & 1)
			{
				pagesManager.GoPreviousPage();
				ConsolePrinter::PrintProcessPage(&pagesManager);

			} // NEXT page process
			else if (GetAsyncKeyState(VK_F2) & 1)
			{
				pagesManager.GoNextPage();
				ConsolePrinter::PrintProcessPage(&pagesManager);

			} // REFRESH process entries
			else if (GetAsyncKeyState(VK_F5) & 1)
			{
				PagesManager pagesManager(memUtils.GetProcList());
				ConsolePrinter::PrintProcessPage(&pagesManager);

			} // SELECT process 
			else if (GetAsyncKeyState(VK_F6) & 1)
			{
				screenState.bStillChoosingProc = true;

				do
				{
					if(ConsolePrinter::GetUserInput(&pagesManager))
					{
						screenState.SwitchToDllScreen();
						memUtils.SetDllName();
						ConsolePrinter::PrintDllPage(&pagesManager, memUtils.GetDllName());
					}

					// CANCEL input process
					if (GetAsyncKeyState(VK_F6) & 1)
						break;

				} while (screenState.bStillChoosingProc);
			}
		}
		else if (screenState.bScreenDLL)
		{
			// REFRESH screen Dll
			if (GetAsyncKeyState(VK_F5) & 1)
			{
				memUtils.SetDllName();
				ConsolePrinter::PrintDllPage(&pagesManager, memUtils.GetDllName());

			} // INJECT Dll
			else if (GetAsyncKeyState(VK_F6) & 1)
			{
				if (memUtils.InjectDllIntoProc(pagesManager.GetProcIdChosen()))
					ConsolePrinter::PrintDllInjected(pagesManager.GetProcKeyChosen(), memUtils.GetDllName());

			} // Go BACK to process list screen
			else if (GetAsyncKeyState(VK_F1) & 1)
			{
				screenState.SwitchToProcessScreen();

				PagesManager pagesManager(memUtils.GetProcList());
				ConsolePrinter::PrintProcessPage(&pagesManager);
			}

			Sleep(5);
		}
	}

	return 0;
}