#pragma once
#include <Windows.h>
#include <iostream>
#include <map>
#include "MemoryUtils.h"
#include "PagesManager.h"
// Undefine max macro
#undef max
#include <limits>

namespace Console
{
	// process page
	void PrintHeaderProc();
	void PrintEachProcess(PagesManager* pPagesManager);
	void PrintProcessPage(PagesManager* pPagesManager);
	void PrintFooterProc(const int pCurrPage,const int pMaxPageNb);
	bool GetUserInput(PagesManager* pPagesManager);

	// DLl page
	void PrintHeaderDLL();
	void PrintFooterDLL();
	void PrintBodyDLL(std::string_view pDLLName, PagesManager* pPagesManager, bool pManualMap);
	void PrintDLLPage(PagesManager* pPagesManager, std::string_view pDLLName, bool pManualMap);
	void PrintDLLInjected(std::wstring_view pProcKeyChosen, std::string_view pDLLName);
};