#include "ConsolePrinter.h"

void ConsolePrinter::PrintHeaderProc()
{
	system("CLS");

	std::cout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "-------------------->> ( [F5] to REFRESH | [F6] to SELECT) <<--------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintProcessPage(PagesManager* pPagesManager)
{
	ConsolePrinter::PrintHeaderProc();

	ConsolePrinter::PrintEachProcess(pPagesManager);

	ConsolePrinter::PrintFooterProc(pPagesManager->GetCurrentPage(), pPagesManager->GetTotalPages());
}

void ConsolePrinter::PrintEachProcess(PagesManager* pPagesManager)
{
	std::map<std::wstring, DWORD>::iterator procIt{ pPagesManager->GetProcIterator() };

	for (int i{ 0 }; i < pPagesManager->GetTotalProcPerPage(); ++i)
	{
		std::wcout << procIt->first << " -------------> [ID]: " << std::hex << procIt->second << "\n";
		++procIt;
	}
}

void ConsolePrinter::PrintFooterProc(int pCurrPage, int pMaxPageNb)
{
	std::cout << "------------------------------->> [PAGE " << pCurrPage << "/" << pMaxPageNb << "] <<---------------------------------- \n";
	std::cout << "--------------------->> PREVIOUS page [F1] | NEXT page [F2] <<------------------- \n";
}

void ConsolePrinter::PrintDllPage(PagesManager* pPagesManager, std::string_view pDllName)
{
	PrintHeaderDll();
	PrintProcKeyChosen(pPagesManager);
	PrintTheDll(pDllName);
	PrintFooterDll();
}

void ConsolePrinter::PrintHeaderDll()
{
	system("CLS");

	std::cout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "-------------------->> ( [F5] to REFRESH | [F6] to INJECT) <<--------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintProcKeyChosen(PagesManager* pPagesManager)
{
	std::wstring procKey{ pPagesManager->GetProcKeyChosen() };
	std::map<std::wstring, DWORD> procMap{ pPagesManager->GetProcessMap() };

	std::wcout << "------> Process: \"" << procKey << "\" - ID: " << std::hex << procMap[procKey] << " selected. \n";
}

void ConsolePrinter::PrintTheDll(std::string_view pDllName)
{
	std::string_view dllOutput{ " No .Dll in the folder. Please insert .Dll file and REFRESH." };

	if (!pDllName.empty()) dllOutput = pDllName;

	std::cout << "------> The directory contain: \n";
	std::cout << "------> " << dllOutput << "\n";
}

void ConsolePrinter::PrintFooterDll()
{
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintDllInjected(std::wstring_view pProcKeyChosen, std::string_view pDllName)
{
	std::cout << "The Dll \"" << pDllName << "\" is loaded into "; std::wcout << pProcKeyChosen << " process! \r";
	std::cout << "-->> ( [F5] to go back to process list | [F6] to go back to Dll injection) <<-- \n";
}