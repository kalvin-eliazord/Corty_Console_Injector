#include "ConsolePrinter.h"
#include "ScreenState.h"
#include "MemoryUtils.h"
#include "PagesManager.h"
#include <cassert>

int main()
{
	// Get process entries
	MemoryUtils memUtils{};
	std::vector<PROCESSENTRY32> procEntryList{ memUtils.GetProcList() };
	assert(!procEntryList.empty() && "No process found.");

	PagesManager pagesManager(procEntryList);

	// First screen
	ConsolePrinter::PrintProcessPage(&pagesManager);

	while (!GetAsyncKeyState(VK_DELETE) & 1)
	{
		if (ScreenState::bScreenProcess)
		{
			// PREVIOUS page
			if (GetAsyncKeyState(VK_F1) & 1)
			{
				pagesManager.GoPreviousPage();
				ConsolePrinter::PrintProcessPage(&pagesManager);

			} // NEXT page
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
				do
				{
					std::string procIdChosen{};
					std::cin >> procIdChosen;

					// Parse the hexadecimal string as an integer
					DWORD userProcId = std::stoi(procIdChosen, nullptr, 16);

					if (userProcId >= 0)
					{
						pagesManager.SetProcKeyChosen(userProcId);

						if (!pagesManager.GetProcKeyChosen().empty())
						{
							memUtils.SetDllName();
							ConsolePrinter::PrintDllPage(&pagesManager, memUtils.GetDllName());

							//  Switching to Dll screen
							ScreenState::bScreenProcess = false;
							ScreenState::bScreenDLL = true;
							ScreenState::bStillChoosingProc = false;
						}
					}
					else
					{
						std::cout << "Wrong entry, please retry. \r";
						Sleep(2000);
					}
				} while (ScreenState::bStillChoosingProc);
			}
		}
		else if (ScreenState::bScreenDLL)
		{
			// REFRESH screen
			if (GetAsyncKeyState(VK_F5) & 1)
			{
				memUtils.SetDllName();
				ConsolePrinter::PrintDllPage(&pagesManager, memUtils.GetDllName());

			} // INJECT Dll
			else if (GetAsyncKeyState(VK_F6) & 1)
			{
				if (memUtils.InjectDllIntoProc(pagesManager.GetProcIdChosen()))
					ConsolePrinter::PrintDllInjected(pagesManager.GetProcKeyChosen(), memUtils.GetDllName());
				else
					std::cout << "Error ----------> Dll not injected.\n";

				// To-do: feature to go back to proc list or dll injection screen
			}
		}

		Sleep(5);
	}

	return 0;
}