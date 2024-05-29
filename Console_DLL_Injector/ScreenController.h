#pragma once
#include <Windows.h>
#include "PagesManager.h"
#include "MemoryUtils.h"
#include "Console.h"

struct ScreenController
{
	bool bScreenDLL{ false };
	bool bScreenProcess{ true };
	bool bValidInput;
	bool bManualMap{ false };

	void SwitchToDLLScreen();
	void SwitchToProcessScreen();
	void RunDLLPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils);
	void RunProcPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils);
};