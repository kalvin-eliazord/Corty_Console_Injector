#pragma once
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <map>
#include "MemoryUtils.h"
#include "PagesManager.h"

namespace ConsolePrinter
{
	void PrintHeaderDll();
	void PrintFooterDll();
	void PrintHeaderProc();
	void PrintTheDll(std::string_view pDllName);
	void PrintEachProcess(PagesManager* pPagesManager);
	void PrintProcessPage(PagesManager* pPagesManager);
	void PrintFooterProc(const int pCurrPage,const int pMaxPageNb);
	void PrintProcKeyChosen(PagesManager* pPagesManager);
	void PrintDllPage(PagesManager* pPagesManager, std::string_view pDllName);
	void PrintDllInjected(std::wstring_view pProcKeyChosen, std::string_view pDllName);
};