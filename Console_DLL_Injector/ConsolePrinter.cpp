#include "ConsolePrinter.h"

void ConsolePrinter::PrintHeaderProc()
{
	system("CLS");

	std::wcout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << "--------------->> ( [F5] to REFRESH | [F6] to SELECT) <<--------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintProcess(PagesManager* pPagesManager)
{
	std::map<std::wstring, DWORD>::iterator procIt{ pPagesManager->GetProcIterator() };

	for (int i{ 0 }; i < pPagesManager->GetTotalProcPerPage(); ++i)
	{
		std::wcout << procIt->first << " -------------> [ID]: " << procIt->second << "\n";
		++procIt;
	}
}

void ConsolePrinter::PrintFooterProc(int pCurrPage, int pMaxPageNb)
{
	std::wcout << "------------------------------->> [PAGE " << pCurrPage << "/" << pMaxPageNb << "] <<---------------------------------- \n";
	std::wcout << "--------------->> PREVIOUS page [F1] | NEXT page [F2] <<------------- \n";
}

void ConsolePrinter::PrintHeaderDll()
{
	system("CLS");

	std::wcout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << "------------------------>> ( [F5] to REFRESH ) <<---------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintDLL(std::wstring_view pDllName)
{
	std::wcout << "----- The directory contain: \n";
	std::wcout << "-----> " << pDllName << "\n";
}

void ConsolePrinter::PrintFooterDll()
{
	std::wcout << "---------------------------------------------------------------------------------- \n";
	std::wcout << "---------------------------------------------------------------------------------- \n";
}

