#pragma once
#include <Windows.h>
#include <iostream>
#include <algorithm>
#include <map>
#include "MemoryUtils.h"
#include "PagesManager.h"

namespace ConsolePrinter
{
	void PrintHeaderProc();
	void PrintHeaderDll();
	void PrintFooterDll();
	void PrintDLL(std::wstring_view pDllName);
	void PrintFooterProc(int pCurrPage, int pMaxPageNb);
	void PrintProcess(PagesManager* pPagesManager);
};