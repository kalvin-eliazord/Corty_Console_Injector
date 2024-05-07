#pragma once
#include <Windows.h>
#include "PagesManager.h"
#include "MemoryUtils.h"
#include "ConsolePrinter.h"

struct ScreenManager
{
	bool bScreenDLL{ false };
	bool bScreenProcess{ true };
	bool bValidInput;

	void SwitchToDllScreen();
	void SwitchToProcessScreen();
	void RunDllPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils);
	void RunProcPage(PagesManager* pPagesManager, MemoryUtils* pMemUtils);
};