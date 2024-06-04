#pragma once
#include <Windows.h>
#include <iostream>
#include <map>
#include "MemUtils.h"
#include "PageProcess.h"
// Undefine max macro
#undef max
#include <limits>

namespace Console
{
	// process page
	void PrintHeaderProc();
	void PrintEachProcess(PageProcess* pPagesManager);
	void PrintProcessPage(PageProcess* pPagesManager);
	void PrintFooterProc(const int pCurrPage,const int pMaxPageNb);
	bool GetUserInput(PageProcess* pPagesManager);

	// DLl page
	void PrintHeaderDLL();
	void PrintFooterDLL();
	void PrintBodyDLL(std::string_view pDLLName, PageProcess* pPagesManager, bool pManualMap);
	void PrintDLLPage(PageProcess* pPagesManager, std::string_view pDLLName, bool pManualMap);
	void PrintDLLInjected(std::wstring_view pProcKeyChosen, std::string_view pDLLName);
};