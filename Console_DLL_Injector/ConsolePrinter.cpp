#include "ConsolePrinter.h"

void ConsolePrinter::PrintHeaderProc()
{
	system("CLS");

	std::cout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "--------->> /!\\ Put the DLL into the same folder as the injector /!\\ <<--------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "-------------->> [ [F5] to REFRESH | [F6] to SELECT or cancel ] <<---------------- \n";
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
		std::wcout << "[+]" << procIt->first << " -------------> [ID]: " << std::hex << procIt->second << "\n";
		++procIt;
	}
}

void ConsolePrinter::PrintFooterProc(const int pCurrPage, const int pMaxPageNb)
{
	std::cout << "------------------------------->> [PAGE " << pCurrPage << "/" << pMaxPageNb << "] <<---------------------------------- \n";
	std::cout << "--------------------->> PREVIOUS page [F1] | NEXT page [F2] <<------------------- \n";
	std::cout << "[+] INPUT: ";

}

bool ConsolePrinter::GetUserInput(PagesManager* pPagesManager)
{
	std::string procId_input{};
	std::cin >> procId_input;

	// Input error check
	if (!std::cin.eof() && std::cin.peek() != '\n')
	{
		std::cerr << "[!] Wrong entry. \r";
		std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); 
		return false;
	}

	int iProcId_input;

	try
	{
		// Parse the (hexadecimal) string input as an integer
		iProcId_input = std::stoi(procId_input, nullptr, 16);
	}
	catch (const std::invalid_argument)
	{
		std::cerr << "[!] Wrong entry. \r";
		return false;
	}
	catch (const std::out_of_range&)
	{
		std::cerr << "[!] The value is outside the valid range. \r";
		return false;
	}

	pPagesManager->SetProcKeyChosen(iProcId_input);

	// Checking if the procId entered exist
	if (pPagesManager->GetProcKeyChosen().empty())
		return false;

	return true;
}

void ConsolePrinter::PrintDllPage(PagesManager* pPagesManager, std::string_view pDllName)
{
	PrintHeaderDll();
	PrintProcKeyChosen(pPagesManager);
	PrintDll(pDllName);
	PrintFooterDll();
}

void ConsolePrinter::PrintHeaderDll()
{
	system("CLS");

	std::cout << "---------------------->> CONSOLE INJECTOR by KALVIN <<---------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "------------------->> [ [F5] to REFRESH | [F6] to INJECT ] <<--------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void ConsolePrinter::PrintProcKeyChosen(PagesManager* pPagesManager)
{
	std::wstring procKey{ pPagesManager->GetProcKeyChosen() };
	std::map<std::wstring, DWORD> procMap{ pPagesManager->GetProcessMap() };

	std::wcout << "[+] ------> Process: \"" << procKey << "\" - ID: " << std::hex << procMap[procKey] << " selected. \n";
}

void ConsolePrinter::PrintDll(std::string_view pDllName)
{
	std::string_view dllOutput{ "[!] No .Dll in the folder. Please insert .Dll file and REFRESH." };

	if (!pDllName.empty()) dllOutput = pDllName;

	std::cout << "[+] ------> The directory contain: \n";
	std::cout << "[+] ------> " << dllOutput << "\n";
}

void ConsolePrinter::PrintFooterDll()
{
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "[+] ------------->> [ [F1] to go BACK to the process list ] <<-------------------- \n";
}

void ConsolePrinter::PrintDllInjected(std::wstring_view pProcKeyChosen, std::string_view pDllName)
{
	std::cout << "[+] The Dll \"" << pDllName << "\" is loaded into "; std::wcout << pProcKeyChosen << " process! \n";
}