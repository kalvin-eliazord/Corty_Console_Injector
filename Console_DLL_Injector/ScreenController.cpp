#include "ScreenController.h"

void ScreenController::GetDllScreen()
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

void ScreenController::RunDLLPage(PageProcess* pPagesManager, MemUtils* pMemUtils)
{
	// REFRESH screen DLL
	if (GetAsyncKeyState(VK_F5) & 1)
	{
		pMemUtils->SetDllPath();
		Console::PrintDLLPage(pPagesManager, pMemUtils->dataDll.name, bManualMap);

	} // INJECT DLL
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		if (bManualMap)
		{
			if (pMemUtils->ManualMap_Start())
				Console::PrintDLLInjected(pPagesManager->GetUserProcName(), pMemUtils->dataDll.name);
		}
		else
		{
			if (pMemUtils->WinAPI_Inject_Start())
				Console::PrintDLLInjected(pPagesManager->GetUserProcName(), pMemUtils->dataDll.name);
		}

	} // Go BACK to process list screen
	else if (GetAsyncKeyState(VK_F1) & 1)
	{
		SwitchToProcessScreen();

		PageProcess pagesManager(pMemUtils->GetProcList());
		Console::PrintProcessPage(&pagesManager);

	} // INJECTION TYPE SWITCH
	else if (GetAsyncKeyState(VK_F2) & 1)
	{
		bManualMap = !bManualMap;
		Console::PrintDLLPage(pPagesManager, pMemUtils->dataDll.name, bManualMap);
	}
}

void ScreenController::RunProcPage(PageProcess* pPagesManager, MemUtils* pMemUtils)
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
		PageProcess pPagesManager(pMemUtils->GetProcList());
		Console::PrintProcessPage(&pPagesManager);

	} // INPUT process ID
	else if (GetAsyncKeyState(VK_F6) & 1)
	{
		bValidInput = false;

		do
		{
			if (Console::GetUserInput(pPagesManager))
			{
				pMemUtils->InitDataProc(pPagesManager);
				GetDllScreen();
				pMemUtils->SetDllPath();
				Console::PrintDLLPage(pPagesManager, pMemUtils->dataDll.name, bManualMap);
			}

			// CANCEL input process
		//	if (GetAsyncKeyState(VK_ESCAPE) & 1) break;

		} while (!bValidInput);
	}
}
