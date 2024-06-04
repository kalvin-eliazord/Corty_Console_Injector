#pragma once
#include <Windows.h>
#include "PageProcess.h"
#include "MemUtils.h"
#include "Console.h"

struct ScreenController
{
	bool bScreenDLL{ false };
	bool bScreenProcess{ true };
	bool bValidInput{};
	bool bManualMap{ false };

	void GetDllScreen();
	void SwitchToProcessScreen();
	void RunDLLPage(PageProcess* pPagesManager, MemUtils* pMemUtils);
	void RunProcPage(PageProcess* pPagesManager, MemUtils* pMemUtils);
};