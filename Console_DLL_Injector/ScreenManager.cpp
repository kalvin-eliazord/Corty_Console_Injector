#include "ScreenManager.h"

void ScreenManager::SwitchToDllScreen()
{
	bScreenProcess = false;
	bScreenDLL = true;
	bValidInput = true;
}

void ScreenManager::SwitchToProcessScreen()
{
	bScreenDLL = false;
	bScreenProcess = true;
}

void ScreenManager::RunDllPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils)
{
	// REFRESH screen Dll
	if (GetAsyncKeyState(VK_F5) & 1)
	{
		pMemUtils->SetDllName();
		ConsolePrinter::PrintDllPage(pPagesManager, pMemUtils->GetDllName());

	} // INJECT Dll
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		if (pMemUtils->InjectDllIntoProc(pPagesManager->GetProcIdChosen()))
			ConsolePrinter::PrintDllInjected(pPagesManager->GetProcKeyChosen(), pMemUtils->GetDllName());

	} // Go BACK to process list screen
	else if (GetAsyncKeyState(VK_F1) & 1)
	{
		SwitchToProcessScreen();

		PagesManager pagesManager(pMemUtils->GetProcList());
		ConsolePrinter::PrintProcessPage(&pagesManager);
	}
}

void ScreenManager::RunProcPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils)
{
	// PREVIOUS page process
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		pPagesManager->GoPreviousPage();
		ConsolePrinter::PrintProcessPage(pPagesManager);

	} // NEXT page process
	else if (GetAsyncKeyState(VK_F2) & 1)
	{
		pPagesManager->GoNextPage();
		ConsolePrinter::PrintProcessPage(pPagesManager);

	} // REFRESH process entries
	else if (GetAsyncKeyState(VK_F5) & 1)
	{
		PagesManager pPagesManager(pMemUtils->GetProcList());
		ConsolePrinter::PrintProcessPage(&pPagesManager);

	} // SELECT process 
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		bValidInput = false;

		do
		{
			if (ConsolePrinter::GetUserInput(pPagesManager))
			{
				SwitchToDllScreen();
				pMemUtils->SetDllName();
				ConsolePrinter::PrintDllPage(pPagesManager, pMemUtils->GetDllName());
			}

			// CANCEL input process
			if (GetAsyncKeyState(VK_ESCAPE) & 1) break;

		} while (!bValidInput);
	}
}
