#include "Console.h"

void Console::PrintHeaderProc()
{
	system("CLS");

	std::cout << "------------------------>> CORTY CONSOLE INJECTOR <<------------------------------ \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "--------->> /!\\ Put the DLL into the same folder as the injector /!\\ <<--------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "-------------->> [ [F5] to REFRESH | [F6] to SELECT or cancel ] <<---------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void Console::PrintProcessPage(PageProcess* pPagesManager)
{
	Console::PrintHeaderProc();

	Console::PrintEachProcess(pPagesManager);

	Console::PrintFooterProc(pPagesManager->GetCurrentPage(), pPagesManager->GetTotalPages());
}

void Console::PrintEachProcess(PageProcess* pPagesManager)
{
	std::map<std::wstring, DWORD>::iterator procIt{ pPagesManager->GetProcIterator() };

	for (int i{ 0 }; i < pPagesManager->GetTotalProcPerPage(); ++i)
	{
		std::wcout << "[+]" << procIt->first << " -------------> [ID]: " << std::hex << procIt->second << '\n';
		++procIt;
	}
}

void Console::PrintFooterProc(const int pCurrPage, const int pMaxPageNb)
{
	std::cout << "------------------------------->> [PAGE " << pCurrPage << "/" << pMaxPageNb << "] <<---------------------------------- \n";
	std::cout << "--------------------->> PREVIOUS page [F1] | NEXT page [F2] <<------------------- \n";
	std::cout << "[+] INPUT: ";

}

bool Console::GetUserInput(PageProcess* pPagesManager)
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

	int userProcId;

	try
	{
		// Parse the string input as an hex integer
		userProcId = std::stoi(procId_input, nullptr, 16);
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

	pPagesManager->SetUserProcess(userProcId);

	// Checking if the procId entered exist
	if (pPagesManager->GetUserProcName().empty())
		return false;

	return true;
}

void Console::PrintDLLPage(PageProcess* pPagesManager, std::string_view pDLLName, bool pManualMap)
{
	PrintHeaderDLL();
	PrintBodyDLL(pDLLName, pPagesManager, pManualMap);
	PrintFooterDLL();
}

void Console::PrintHeaderDLL()
{
	system("CLS");

	std::cout << "------------------------>> CORTY CONSOLE INJECTOR <<------------------------------ \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << " >> /!\\ Put the DLL and the .EXE shorcut in the same folder of the injector /!\\ <<\n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "------->> [ [F5] to REFRESH | [F6] to INJECT | [F2] SWITCH injection ] <<--------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
}

void Console::PrintBodyDLL(std::string_view pDLLName, PageProcess* pPagesManager, bool pManualMap)
{
	std::string_view dllOutput{ "[!] Please insert a DLL file and press [F5] (REFRESH)." };
	std::string_view injecType{ "LoadLibraryA" };

	std::wstring nameProc{ pPagesManager->GetUserProcName() };
	std::map<std::wstring, DWORD> procMap{ pPagesManager->GetProcessMap() };

	if (!pDLLName.empty()) dllOutput = pDLLName;
	if (pManualMap) injecType = "Manual Map";

	std::cout  << "[+] Injection Type: " << injecType << '\n';
	std::wcout << "[+] Process: \"" << nameProc << "\" - ID: " << std::hex << procMap[nameProc] << '\n';
	std::cout  << "[+] The directory contain: \n";
	std::cout  << "[+] " << dllOutput << '\n';
}

void Console::PrintFooterDLL()
{
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "---------------------------------------------------------------------------------- \n";
	std::cout << "[+] ------------->> [ [F1] to go BACK to the process list ] <<-------------------- \n";
}

void Console::PrintDLLInjected(std::wstring_view pProcKeyChosen, std::string_view pDLLName)
{
	std::cout << "[+] The DLL \"" << pDLLName << "\" is loaded into "; std::wcout << pProcKeyChosen << " process! \n";
}