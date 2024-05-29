#include "ScreenController.h"

void ScreenController::SwitchToDLLScreen()
{
	bScreenProcess = false;
	bScreenDLL = true;
	bValidInput = true;
}

void ScreenController::SwitchToProcessScreen()
{
	bScreenDLL = false;
	bScreenProcess = true;
}

void ScreenController::RunDLLPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils)
{
	// REFRESH screen DLL
	if (GetAsyncKeyState(VK_F5) & 1)
	{
		pMemUtils->SetDLLName();
		Console::PrintDLLPage(pPagesManager, pMemUtils->GetDLLName(), bManualMap);

	} // INJECT DLL
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		if (bManualMap)
		{
			if (pMemUtils->ManualMapping_Injection(pPagesManager->GetUserIdProc()))
				Console::PrintDLLInjected(pPagesManager->GetUserProcess(), pMemUtils->GetDLLName());
		}
		else
		{
			if (pMemUtils->WinAPI_Injection(pPagesManager->GetUserIdProc()))
				Console::PrintDLLInjected(pPagesManager->GetUserProcess(), pMemUtils->GetDLLName());
		}

	} // Go BACK to process list screen
	else if (GetAsyncKeyState(VK_F1) & 1)
	{
		SwitchToProcessScreen();

		PagesManager pagesManager(pMemUtils->GetProcList());
		Console::PrintProcessPage(&pagesManager);

	} // INJECTION TYPE SWITCH
	else if (GetAsyncKeyState(VK_F2) & 1)
	{
		bManualMap = !bManualMap;
		Console::PrintDLLPage(pPagesManager, pMemUtils->GetDLLName(), bManualMap);
	}
}

void ScreenController::RunProcPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils)
{
	// PREVIOUS page process
	if (GetAsyncKeyState(VK_F1) & 1)
	{
		pPagesManager->GoPreviousPage();
		Console::PrintProcessPage(pPagesManager);

	} // NEXT page process
	else if (GetAsyncKeyState(VK_F2) & 1)
	{
		pPagesManager->GoNextPage();
		Console::PrintProcessPage(pPagesManager);

	} // REFRESH process entries
	else if (GetAsyncKeyState(VK_F5) & 1)
	{
		PagesManager pPagesManager(pMemUtils->GetProcList());
		Console::PrintProcessPage(&pPagesManager);

	} // SELECT process 
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		bValidInput = false;

		do
		{
			if (Console::GetUserInput(pPagesManager))
			{
				SwitchToDLLScreen();
				pMemUtils->SetDLLName();
				Console::PrintDLLPage(pPagesManager, pMemUtils->GetDLLName(), bManualMap);
			}

			// CANCEL input process
			if (GetAsyncKeyState(VK_ESCAPE) & 1) break;

		} while (!bValidInput);
	}
}
